#include "Framebuffer.h"

#include "Ignis.h"

int ignisGenerateFrameBuffer(IgnisFrameBuffer* fbo, int width, int height)
{
	fbo->width = width;
	fbo->height = height;

	/* create a color attachment texture */
	ignisGenerateTexture2D(&fbo->texture, width, height, NULL, NULL);

	glGenFramebuffers(1, &fbo->name);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo->name);

	/* create the actual framebuffer object */
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, fbo->texture.name, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		_ignisErrorCallback(IGNIS_ERROR, "[Framebuffer] Framebuffer is not complete!");
		ignisDeleteFrameBuffer(fbo);
		return IGNIS_FAILURE;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return IGNIS_SUCCESS;
}

void ignisDeleteFrameBuffer(IgnisFrameBuffer* fbo)
{
	ignisDeleteTexture2D(&fbo->texture);

	glDeleteFramebuffers(1, &fbo->name);
}

void ignisBindFrameBuffer(IgnisFrameBuffer* fbo)
{
	glBindFramebuffer(GL_FRAMEBUFFER, (fbo) ? fbo->name : 0);
}
