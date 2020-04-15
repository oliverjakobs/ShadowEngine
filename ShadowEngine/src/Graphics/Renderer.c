#include "Renderer.h"

void GenerateIndices(GLuint* indices, size_t max, size_t step)
{
	GLuint offset = 0;
	for (size_t i = 0; i < max - step; i += step)
	{
		indices[i + 0] = offset + 0;
		indices[i + 1] = offset + 1;
		indices[i + 2] = offset + 2;

		indices[i + 3] = offset + 2;
		indices[i + 4] = offset + 3;
		indices[i + 5] = offset + 0;

		offset += 4;
	}
}
