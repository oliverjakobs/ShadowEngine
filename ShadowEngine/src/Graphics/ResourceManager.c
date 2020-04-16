#include "ResourceManager.h"

#define TB_JSON_IMPLEMENTATION
#include "json/tb_json.h"
#include "clib/clib.h"

#include "Application/Debugger.h"
#include "Application/defines.h"

typedef struct
{
	char key[APPLICATION_STR_LEN];
	IgnisTexture2D* value;
} _texkvp;

typedef struct
{
	char key[APPLICATION_STR_LEN];
	IgnisFont* value;
} _fontkvp;

/* Declare type-specific type_hashmap_* functions with this handy macro */
CLIB_HASHMAP_DEFINE_FUNCS(tex, char, _texkvp)
CLIB_HASHMAP_DEFINE_FUNCS(font, char, _fontkvp)

int ResourceManagerInit(ResourceManager* resources, const char* path)
{
	clib_hashmap_init(&resources->textures, clib_hashmap_hash_string, clib_hashmap_compare_string, 0);
	clib_hashmap_init(&resources->fonts, clib_hashmap_hash_string, clib_hashmap_compare_string, 0);

	char* json = ignisReadFile(path, NULL);

	if (!json)
	{
		DEBUG_ERROR("[Resources] Failed to read index (%s)", path);
		return 0;
	}

	/* textures */
	tb_json_element textures;
	tb_json_read(json, &textures, "{'textures'");

	for (int i = 0; i < textures.elements; i++)
	{
		char texture_name[APPLICATION_STR_LEN];
		tb_json_string((char*)textures.value, "{*", texture_name, APPLICATION_STR_LEN, &i);

		tb_json_element texture;
		tb_json_read_format((char*)textures.value, &texture, "{'%s'", texture_name);

		char texture_path[APPLICATION_PATH_LEN];
		tb_json_string((char*)texture.value, "{'path'", texture_path, APPLICATION_PATH_LEN, NULL);

		int rows = MAX(tb_json_int((char*)texture.value, "{'atlas'[0", NULL), 1);
		int columns = MAX(tb_json_int((char*)texture.value, "{'atlas'[1", NULL), 1);

		ResourceManagerAddTexture2D(resources, texture_name, texture_path, rows, columns);
	}

	/* fonts */
	tb_json_element fonts;
	tb_json_read(json, &fonts, "{'fonts'");

	for (int i = 0; i < fonts.elements; i++)
	{
		char font_name[APPLICATION_STR_LEN];
		tb_json_string((char*)fonts.value, "{*", font_name, APPLICATION_STR_LEN, &i);

		tb_json_element font;
		tb_json_read_format((char*)fonts.value, &font, "{'%s'", font_name);

		char font_path[APPLICATION_PATH_LEN];
		tb_json_string((char*)font.value, "{'path'", font_path, APPLICATION_PATH_LEN, NULL);

		float font_size = tb_json_float((char*)font.value, "{'size'", NULL);

		ResourceManagerAddFont(resources, font_name, font_path, font_size);
	}

	free(json);

	return 1;
}

void ResourceManagerDestroy(ResourceManager* manager)
{
	for (struct clib_hashmap_iter* iter = clib_hashmap_iter(&manager->textures); iter; iter = clib_hashmap_iter_next(&manager->textures, iter))
		ignisDeleteTexture2D(tex_hashmap_iter_get_value(iter)->value);

	clib_hashmap_destroy(&manager->textures);

	for (struct clib_hashmap_iter* iter = clib_hashmap_iter(&manager->fonts); iter; iter = clib_hashmap_iter_next(&manager->fonts, iter))
		ignisDeleteFont(font_hashmap_iter_get_value(iter)->value);

	clib_hashmap_destroy(&manager->fonts);
}

IgnisTexture2D* ResourceManagerAddTexture2D(ResourceManager* manager, const char* name, const char* path, int rows, int columns)
{
	if (strlen(name) > APPLICATION_STR_LEN)
	{
		DEBUG_ERROR("[Resources] Texture name (%s) too long. Max. name length is %d\n", name, APPLICATION_STR_LEN);
		return NULL;
	}

	IgnisTexture2D* texture = (IgnisTexture2D*)malloc(sizeof(IgnisTexture2D));

	if (ignisCreateTexture2D(texture, path, rows, columns, 1, NULL))
	{
		_texkvp* kvp = (_texkvp*)malloc(sizeof(_texkvp));

		if (kvp)
		{
			strcpy(kvp->key, name);
			kvp->value = texture;

			if (tex_hashmap_put(&manager->textures, kvp->key, kvp) == kvp)
				return texture;
		}

		DEBUG_ERROR("[Resources] Failed to add texture: %s (%s)\n", name, path);
		ignisDeleteTexture2D(texture);
		free(kvp);
	}
	free(texture);
	return NULL;
}

IgnisFont* ResourceManagerAddFont(ResourceManager* manager, const char* name, const char* path, float size)
{
	if (strlen(name) > APPLICATION_STR_LEN)
	{
		DEBUG_ERROR("[Resources] Font name (%s) too long. Max. name length is %d\n", name, APPLICATION_STR_LEN);
		return NULL;
	}

	IgnisFont* font = (IgnisFont*)malloc(sizeof(IgnisFont));

	if (ignisCreateFont(font, path, size))
	{
		_fontkvp* kvp = (_fontkvp*)malloc(sizeof(_fontkvp));

		if (kvp)
		{
			strcpy(kvp->key, name);
			kvp->value = font;

			if (font_hashmap_put(&manager->fonts, kvp->key, kvp) == kvp)
				return font;
		}
		DEBUG_ERROR("[Resources] Failed to add font: %s (%s)\n", name, path);
		ignisDeleteFont(font);
		free(kvp);
	}
	free(font);
	return NULL;
}

IgnisTexture2D* ResourceManagerGetTexture2D(ResourceManager* manager, const char* name)
{
	_texkvp* kvp = tex_hashmap_get(&manager->textures, name);

	if (kvp) return kvp->value;

	DEBUG_WARN("[Resources] Could not find texture: %s\n", name);
	return NULL;
}

IgnisFont* ResourceManagerGetFont(ResourceManager* manager, const char* name)
{
	_fontkvp* kvp = font_hashmap_get(&manager->fonts, name);

	if (kvp) return kvp->value;

	DEBUG_WARN("[Resources] Could not find font: %s\n", name);
	return NULL;
}
