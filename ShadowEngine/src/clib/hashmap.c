/*
 * Copyright (c) 2016-2018 David Leeds <davidesleeds@gmail.com>
 *
 * Hashmap is free software; you can redistribute it and/or modify
 * it under the terms of the MIT license. See LICENSE for details.
 */
#include "hashmap.h"

#ifndef CLIB_HASHMAP_NOASSERT
#include <assert.h>
#define CLIB_HASHMAP_ASSERT(expr)           assert(expr)
#else
#define CLIB_HASHMAP_ASSERT(expr)
#endif

/* Table sizes must be powers of 2 */
#define CLIB_HASHMAP_SIZE_MIN               (1 << 5)    /* 32 */
#define CLIB_HASHMAP_SIZE_DEFAULT           (1 << 8)    /* 256 */
#define CLIB_HASHMAP_SIZE_MOD(map, val)     ((val) & ((map)->table_size - 1))

/* Limit for probing is 1/2 of table_size */
#define CLIB_HASHMAP_PROBE_LEN(map)         ((map)->table_size >> 1)
/* Return the next linear probe index */
#define CLIB_HASHMAP_PROBE_NEXT(map, index) CLIB_HASHMAP_SIZE_MOD(map, (index) + 1)

/* Check if index b is less than or equal to index a */
#define CLIB_HASHMAP_INDEX_LE(map, a, b)    ((a) == (b) || (((b) - (a)) & ((map)->table_size >> 1)) != 0)

/*
 * Enforce a maximum 0.75 load factor.
 */
static inline size_t clib_hashmap_table_min_size_calc(size_t num_entries)
{
    return num_entries + (num_entries / 3);
}

/*
 * Calculate the optimal table size, given the specified max number
 * of elements.
 */
static size_t clib_hashmap_table_size_calc(size_t num_entries)
{
    size_t table_size = clib_hashmap_table_min_size_calc(num_entries);

    /* Table size is always a power of 2 */
    size_t min_size = CLIB_HASHMAP_SIZE_MIN;
    while (min_size < table_size)
        min_size <<= 1;

    return min_size;
}

/*
 * Get a valid hash table index from a key.
 */
static inline size_t clib_hashmap_calc_index(const clib_hashmap* map, const void* key)
{
    return CLIB_HASHMAP_SIZE_MOD(map, map->hash(key));
}

/*
 * Return the next populated entry, starting with the specified one.
 * Returns NULL if there are no more valid entries.
 */
static clib_hashmap_entry* clib_hashmap_entry_get_populated(const clib_hashmap* map, clib_hashmap_entry* entry)
{
    for (; entry < &map->table[map->table_size]; ++entry) 
    {
        if (entry->key)
            return entry;
    }
    return NULL;
}

/*
 * Find the hashmap entry with the specified key, or an empty slot.
 * Returns NULL if the entire table has been searched without finding a match.
 */
static clib_hashmap_entry* clib_hashmap_entry_find(const clib_hashmap* map, const void* key, bool find_empty)
{
    size_t probe_len = CLIB_HASHMAP_PROBE_LEN(map);
    size_t index = clib_hashmap_calc_index(map, key);

    /* Linear probing */
    for (size_t i = 0; i < probe_len; ++i)
    {
        clib_hashmap_entry* entry = &map->table[index];
        if (!entry->key)
        {
            if (find_empty)
            {
#ifdef CLIB_HASHMAP_METRICS
                entry->num_collisions = i;
#endif
                return entry;
            }
            return NULL;
        }

        if (map->key_compare(key, entry->key) == 0)
            return entry;

        index = CLIB_HASHMAP_PROBE_NEXT(map, index);
    }
    return NULL;
}

/*
 * Removes the specified entry and processes the proceeding entries to reduce
 * the load factor and keep the chain continuous.  This is a required
 * step for hash maps using linear probing.
 */
static void clib_hashmap_entry_remove(clib_hashmap* map, clib_hashmap_entry* removed_entry)
{
    size_t removed_index = (removed_entry - map->table);

#ifdef CLIB_HASHMAP_METRICS
    size_t removed_i = 0;
#endif

    /* Free the key */
    if (map->key_free)
        map->key_free(removed_entry->key);

    --map->num_entries;

    /* Fill the free slot in the chain */
    size_t index = CLIB_HASHMAP_PROBE_NEXT(map, removed_index);
    for (size_t i = 1; i < map->table_size; ++i)
    {
        clib_hashmap_entry* entry = &map->table[index];
        if (!entry->key)
            break; /* Reached end of chain */

        size_t entry_index = clib_hashmap_calc_index(map, entry->key);
        /* Shift in entries with an index <= to the removed slot */
        if (CLIB_HASHMAP_INDEX_LE(map, removed_index, entry_index))
        {
#ifdef CLIB_HASHMAP_METRICS
            entry->num_collisions -= (i - removed_i);
            removed_i = i;
#endif
            memcpy(removed_entry, entry, sizeof(*removed_entry));
            removed_index = index;
            removed_entry = entry;
        }
        index = CLIB_HASHMAP_PROBE_NEXT(map, index);
    }
    /* Clear the last removed entry */
    memset(removed_entry, 0, sizeof(*removed_entry));
}

/*
 * Reallocates the hash table to the new size and rehashes all entries.
 * new_size MUST be a power of 2.
 * Returns 0 on success and -errno on allocation or hash function failure.
 */
static int clib_hashmap_rehash(clib_hashmap* map, size_t new_size)
{
    CLIB_HASHMAP_ASSERT(new_size >= CLIB_HASHMAP_SIZE_MIN);
    CLIB_HASHMAP_ASSERT((new_size & (new_size - 1)) == 0);

    clib_hashmap_entry* new_table = (clib_hashmap_entry*)calloc(new_size, sizeof(clib_hashmap_entry));
    if (!new_table) return -ENOMEM;

    /* Backup old elements in case of rehash failure */
    size_t old_size = map->table_size;
    clib_hashmap_entry* old_table = map->table;
    map->table_size = new_size;
    map->table = new_table;
    /* Rehash */
    for (clib_hashmap_entry* entry = old_table; entry < &old_table[old_size]; ++entry)
    {
        if (!entry->value) continue; /* Only copy entries with value */

        clib_hashmap_entry* new_entry = clib_hashmap_entry_find(map, entry->key, true);
        if (!new_entry)
        {
            /*
             * The load factor is too high with the new table
             * size, or a poor hash function was used.
             */
            map->table_size = old_size;
            map->table = old_table;
            free(new_table);
            return -EINVAL;
        }
        /* Shallow copy (intentionally omits num_collisions) */
        new_entry->key = entry->key;
        new_entry->value = entry->value;
    }
    free(old_table);
    return 0;
}

/*
 * Iterate through all entries and free all keys.
 */
static void clib_hashmap_free_keys(clib_hashmap* map)
{
    if (!map->key_free)
        return;

    for (struct clib_hashmap_iter* iter = clib_hashmap_iter(map); iter; iter = clib_hashmap_iter_next(map, iter))
        map->key_free((void *)clib_hashmap_iter_get_key(iter));
}

/*
 * Initialize an empty hashmap.
 *
 * hash_func should return an even distribution of numbers between 0
 * and SIZE_MAX varying on the key provided.  If set to NULL, the default
 * case-sensitive string hash function is used: hashmap_hash_string
 *
 * key_compare_func should return 0 if the keys match, and non-zero otherwise.
 * If set to NULL, the default case-sensitive string comparator function is
 * used: hashmap_compare_string
 *
 * initial_size is optional, and may be set to the max number of entries
 * expected to be put in the hash table.  This is used as a hint to
 * pre-allocate the hash table to the minimum size needed to avoid
 * gratuitous rehashes.  If initial_size is 0, a default size will be used.
 *
 * Returns 0 on success and -errno on failure.
 */
int clib_hashmap_init(clib_hashmap* map, size_t (*hash_func)(const void*), int (*key_compare_func)(const void*, const void*), size_t initial_size)
{
    CLIB_HASHMAP_ASSERT(map != NULL);

    if (!initial_size)
        initial_size = CLIB_HASHMAP_SIZE_DEFAULT;
    else
        initial_size = clib_hashmap_table_size_calc(initial_size); /* Convert init size to valid table size */
    
    map->table_size_init = initial_size;
    map->table_size = initial_size;
    map->num_entries = 0;
    map->table = (clib_hashmap_entry*)calloc(initial_size, sizeof(clib_hashmap_entry));

    if (!map->table)
        return -ENOMEM;

    map->hash = hash_func ? hash_func : clib_hashmap_hash_string;
    map->key_compare = key_compare_func ? key_compare_func : clib_hashmap_compare_string;
    map->key_alloc = NULL;
    map->key_free = NULL;
    return 0;
}

/*
 * Free the hashmap and all associated memory.
 */
void clib_hashmap_destroy(clib_hashmap* map)
{
    if (!map)
        return;

    clib_hashmap_free_keys(map);
    free(map->table);
    memset(map, 0, sizeof(*map));
}

/*
 * Enable internal memory management of hash keys.
 */
void clib_hashmap_set_key_alloc_funcs(clib_hashmap* map, void* (*key_alloc_func)(const void*), void (*key_free_func)(void*))
{
    CLIB_HASHMAP_ASSERT(map != NULL);

    map->key_alloc = key_alloc_func;
    map->key_free = key_free_func;
}

/*
 * Add an entry to the hashmap.  If an entry with a matching key already
 * exists and has a value pointer associated with it, the existing value
 * pointer is returned, instead of assigning the new value.  Compare
 * the return value with the value passed in to determine if a new entry was
 * created.  Returns NULL if memory allocation failed.
 */
void* clib_hashmap_put(clib_hashmap* map, const void* key, void* value)
{
    CLIB_HASHMAP_ASSERT(map != NULL);
    CLIB_HASHMAP_ASSERT(key != NULL);

    /* Rehash with 2x capacity if load factor is approaching 0.75 */
    if (map->table_size <= clib_hashmap_table_min_size_calc(map->num_entries))
        clib_hashmap_rehash(map, map->table_size << 1);

    clib_hashmap_entry* entry = clib_hashmap_entry_find(map, key, true);
    if (!entry)
    {
        /*
         * Cannot find an empty slot.  Either out of memory, or using
         * a poor hash function.  Attempt to rehash once to reduce
         * chain length.
         */
        if (clib_hashmap_rehash(map, map->table_size << 1) < 0)
            return NULL;

        entry = clib_hashmap_entry_find(map, key, true);
        if (!entry)
            return NULL;
    }
    if (!entry->key) 
    {
        /* Allocate copy of key to simplify memory management */
        if (map->key_alloc)
        {
            entry->key = map->key_alloc(key);
            if (!entry->key)
                return NULL;
        } 
        else 
        {
            entry->key = (void *)key;
        }
        ++map->num_entries;
    }
    else if (entry->value)
    {
        /* Do not overwrite existing value */
        return entry->value;
    }

    entry->value = value;
    return value;
}

/*
 * Return the value pointer, or NULL if no entry exists.
 */
void* clib_hashmap_get(const clib_hashmap* map, const void* key)
{
    CLIB_HASHMAP_ASSERT(map != NULL);
    CLIB_HASHMAP_ASSERT(key != NULL);

    clib_hashmap_entry* entry = clib_hashmap_entry_find(map, key, false);

    if (!entry) return NULL;

    return entry->value;
}

/*
 * Remove an entry with the specified key from the map.
 * Returns the value pointer, or NULL, if no entry was found.
 */
void* clib_hashmap_remove(clib_hashmap* map, const void *key)
{
    CLIB_HASHMAP_ASSERT(map != NULL);
    CLIB_HASHMAP_ASSERT(key != NULL);

    clib_hashmap_entry* entry = clib_hashmap_entry_find(map, key, false);
    if (!entry)
        return NULL;

    void* value = entry->value;
    /* Clear the entry and make the chain contiguous */
    clib_hashmap_entry_remove(map, entry);
    return value;
}

/*
 * Remove all entries.
 */
void clib_hashmap_clear(clib_hashmap* map)
{
    CLIB_HASHMAP_ASSERT(map != NULL);

    clib_hashmap_free_keys(map);
    map->num_entries = 0;
    memset(map->table, 0, sizeof(clib_hashmap_entry) * map->table_size);
}

/*
 * Remove all entries and reset the hash table to its initial size.
 */
void clib_hashmap_reset(clib_hashmap* map)
{
    CLIB_HASHMAP_ASSERT(map != NULL);

    clib_hashmap_clear(map);
    if (map->table_size == map->table_size_init)
        return;

    clib_hashmap_entry* new_table = (clib_hashmap_entry*)realloc(map->table, sizeof(clib_hashmap_entry) * map->table_size_init);
    if (!new_table)
        return;

    map->table = new_table;
    map->table_size = map->table_size_init;
}

/*
 * Return the number of entries in the hash map.
 */
size_t clib_hashmap_size(const clib_hashmap* map)
{
    CLIB_HASHMAP_ASSERT(map != NULL);

    return map->num_entries;
}

/*
 * Get a new hashmap iterator.  The iterator is an opaque
 * pointer that may be used with hashmap_iter_*() functions.
 * Hashmap iterators are INVALID after a put or remove operation is performed.
 * hashmap_iter_remove() allows safe removal during iteration.
 */
struct clib_hashmap_iter* clib_hashmap_iter(const clib_hashmap* map)
{
    CLIB_HASHMAP_ASSERT(map != NULL);

    if (!map->num_entries)
        return NULL;

    return (struct clib_hashmap_iter*)clib_hashmap_entry_get_populated(map, map->table);
}

/*
 * Return an iterator to the next hashmap entry.  Returns NULL if there are
 * no more entries.
 */
struct clib_hashmap_iter* clib_hashmap_iter_next(const clib_hashmap* map, const struct clib_hashmap_iter* iter)
{
    CLIB_HASHMAP_ASSERT(map != NULL);

    if (!iter) return NULL;

    clib_hashmap_entry* entry = (clib_hashmap_entry*)iter;

    return (struct clib_hashmap_iter*)clib_hashmap_entry_get_populated(map, entry + 1);
}

/*
 * Remove the hashmap entry pointed to by this iterator and return an
 * iterator to the next entry.  Returns NULL if there are no more entries.
 */
struct clib_hashmap_iter* clib_hashmap_iter_remove(clib_hashmap* map, const struct clib_hashmap_iter* iter)
{
    CLIB_HASHMAP_ASSERT(map != NULL);

    if (!iter)
        return NULL;

    clib_hashmap_entry* entry = (clib_hashmap_entry*)iter;

    if (!entry->key)
        return clib_hashmap_iter_next(map, iter); /* Iterator is invalid, so just return the next valid entry */

    clib_hashmap_entry_remove(map, entry);
    return (struct clib_hashmap_iter*)clib_hashmap_entry_get_populated(map, entry);
}

/*
 * Return the key of the entry pointed to by the iterator.
 */
const void* clib_hashmap_iter_get_key(const struct clib_hashmap_iter* iter)
{
    if (!iter)
        return NULL;

    return (const void*)((clib_hashmap_entry*)iter)->key;
}

/*
 * Return the value of the entry pointed to by the iterator.
 */
void* clib_hashmap_iter_get_value(const struct clib_hashmap_iter* iter)
{
    if (!iter)
        return NULL;

    return ((clib_hashmap_entry*)iter)->value;
}

/*
 * Set the value pointer of the entry pointed to by the iterator.
 */
void clib_hashmap_iter_set_value(const struct clib_hashmap_iter* iter, void* value)
{
    if (!iter)
        return;

    ((clib_hashmap_entry*)iter)->value = value;
}

/*
 * Invoke func for each entry in the hashmap.  Unlike the hashmap_iter_*()
 * interface, this function supports calls to hashmap_remove() during iteration.
 * However, it is an error to put or remove an entry other than the current one,
 * and doing so will immediately halt iteration and return an error.
 * Iteration is stopped if func returns non-zero. 
 * Returns func's return value if it is < 0, otherwise, 0.
 */
int clib_hashmap_foreach(const clib_hashmap* map, int (*func)(const void*, void*, void*), void* arg)
{
    CLIB_HASHMAP_ASSERT(map != NULL);
    CLIB_HASHMAP_ASSERT(func != NULL);

    for (clib_hashmap_entry* entry = map->table; entry < &map->table[map->table_size]; ++entry)
    {
        if (!entry->key)
            continue;

        size_t num_entries = map->num_entries;
        const void* key = entry->key;
        int rc = func(entry->key, entry->value, arg);

        if (rc < 0)
            return rc;

        if (rc > 0)
            return 0;

        /* Run this entry again if func() deleted it */
        if (entry->key != key)
            --entry;
        else if (num_entries != map->num_entries)
            return -1; /* Stop immediately if func put/removed another entry */
    }

    return 0;
}

/*
 * Default hash function for string keys.
 * This is an implementation of the well-documented Jenkins one-at-a-time
 * hash function.
 */
size_t clib_hashmap_hash_string(const void* key)
{
    size_t hash = 0;

    for (const char* key_str = (const char*)key; *key_str; ++key_str)
    {
        hash += *key_str;
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }

    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);
    return hash;
}

/*
 * Default key comparator function for string keys.
 */
int clib_hashmap_compare_string(const void *a, const void *b)
{
    return strcmp((const char *)a, (const char *)b);
}

/*
 * Default key allocation function for string keys.  Use free() for the
 * key_free_func.
 */
void* clib_hashmap_alloc_key_string(const void* key)
{
    return (void*)strdup((const char*)key);
}

/*
 * Case insensitive hash function for string keys.
 */
size_t clib_hashmap_hash_string_i(const void *key)
{
    size_t hash = 0;

    for (const char* key_str = (const char*)key; *key_str; ++key_str)
    {
        hash += tolower(*key_str);
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }

    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);
    return hash;
}

/*
 * Case insensitive key comparator function for string keys.
 */
int clib_hashmap_compare_string_i(const void* a, const void* b)
{
    return strcmp((const char*)a, (const char*)b);
}


#ifdef CLIB_HASHMAP_METRICS
/*
 * Return the load factor.
 */
double clib_hashmap_load_factor(const clib_hashmap* map)
{
    CLIB_HASHMAP_ASSERT(map != NULL);

    if (!map->table_size)
        return 0;

    return (double)map->num_entries / map->table_size;
}

/*
 * Return the average number of collisions per entry.
 */
double clib_hashmap_collisions_mean(const clib_hashmap *map)
{
    clib_hashmap_entry *entry;
    size_t total_collisions = 0;

    CLIB_HASHMAP_ASSERT(map != NULL);

    if (!map->num_entries)
        return 0;

    for (entry = map->table; entry < &map->table[map->table_size]; ++entry) 
    {
        if (!entry->key)
            continue;

        total_collisions += entry->num_collisions;
    }

    return (double)total_collisions / map->num_entries;
}

/*
 * Return the variance between entry collisions.  The higher the variance,
 * the more likely the hash function is poor and is resulting in clustering.
 */
double clib_hashmap_collisions_variance(const clib_hashmap *map)
{
    clib_hashmap_entry *entry;
    double mean_collisions;
    double variance;
    double total_variance = 0;

    CLIB_HASHMAP_ASSERT(map != NULL);

    if (!map->num_entries)
        return 0;

    mean_collisions = clib_hashmap_collisions_mean(map);
    for (entry = map->table; entry < &map->table[map->table_size]; ++entry) 
    {
        if (!entry->key)
            continue;

        variance = (double)entry->num_collisions - mean_collisions;
        total_variance += variance * variance;
    }
    return total_variance / map->num_entries;
}
#endif
