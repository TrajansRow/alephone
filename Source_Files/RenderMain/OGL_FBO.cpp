/*

	Copyright (C) 2015 and beyond by Jeremiah Morris
	and the "Aleph One" developers.
 
	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	This license is contained in the file "COPYING",
	which is included with this source code; it is available online at
	http://www.gnu.org/licenses/gpl.html
	
	Framebuffer Object utilities
*/

#include "cseries.h"
#include "OGL_FBO.h"

#ifdef HAVE_OPENGL

#include "OGL_Setup.h"
#include "OGL_Render.h"

std::vector<FBO *> FBO::active_chain;

FBO::FBO(GLuint w, GLuint h, bool srgb) : _h(h), _w(w), _srgb(srgb) {
    setup(w, h, srgb);

    /*
	glGenFramebuffers(1, &_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
	
	glGenRenderbuffers(1, &_depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, _depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, _w, _h);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _depthBuffer);
	
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_RECTANGLE, texID);
	glTexImage2D(GL_TEXTURE_RECTANGLE, 0, srgb ? GL_SRGB : GL_RGB8, _w, _h, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE, texID, 0);
	assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);*/
}

void FBO::setup(GLuint w, GLuint h, bool srgb) {
  
  _h = h; _w = w; _srgb = srgb;
  
  //Do nothing if not valid size. Call again later to initialize.
  if( w == 0 && h == 0) {
    return;
  }
  
  glGenFramebuffers(1, &_fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
  
  //Create texture and attach it to framebuffer's color attachment point
  glGenTextures(1, &texID);
  glBindTexture(GL_TEXTURE_2D, texID); //DCW was GL_TEXTURE_RECTANGE
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);  //DCW
  glTexImage2D(GL_TEXTURE_2D, 0, srgb ? GL_SRGB : GL_RGBA, _w, _h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);//DCW was GL_TEXTURE_RECTANGLE, changed GL_RGB to GL_RGBA
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texID, 0); //DCW was GL_TEXTURE_RECTANGLE
  
  //Generate depth buffer
  glGenRenderbuffers(1, &_depthBuffer);
  glBindRenderbuffer(GL_RENDERBUFFER, _depthBuffer);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, _w, _h);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _depthBuffer);
  
  glBindFramebuffer(GL_FRAMEBUFFER, 1);
  glBindRenderbuffer(GL_RENDERBUFFER, 1);
  
  glPopGroupMarkerEXT();
}

void FBO::activate(bool clear) {
	if (!active_chain.size() || active_chain.back() != this) {
		active_chain.push_back(this);
		glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
		
        //glPushAttrib(GL_VIEWPORT_BIT);
        glGetIntegerv(GL_VIEWPORT, viewportCache);
        
		glViewport(0, 0, _w, _h);
		if (_srgb)
			glEnable(GL_FRAMEBUFFER_SRGB);
		else
			glDisable(GL_FRAMEBUFFER_SRGB);
		if (clear)
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
}

void FBO::deactivate() {
	if (active_chain.size() && active_chain.back() == this) {
		active_chain.pop_back();
		
        //glPopAttrib();
		glViewport(viewportCache[0], viewportCache[1], viewportCache[2], viewportCache[3]);
        
		GLuint prev_fbo = 0;
		bool prev_srgb = Using_sRGB;
		if (active_chain.size()) {
			prev_fbo = active_chain.back()->_fbo;
			prev_srgb = active_chain.back()->_srgb;
		}
		glBindFramebuffer(GL_FRAMEBUFFER, prev_fbo);
		if (prev_srgb)
			glEnable(GL_FRAMEBUFFER_SRGB);
		else
			glDisable(GL_FRAMEBUFFER_SRGB);
	}
}

void FBO::draw() {
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, texID);
	glEnable(GL_TEXTURE_RECTANGLE_ARB);
	OGL_RenderTexturedRect(0, 0, _w, _h, 0, _h, _w, 0);
	glDisable(GL_TEXTURE_RECTANGLE_ARB);
}

void FBO::prepare_drawing_mode(bool blend) {
	MSI()->matrixMode(MS_PROJECTION);
	MSI()->pushMatrix();
	MSI()->loadIdentity();
	MSI()->matrixMode(MS_MODELVIEW);
	MSI()->pushMatrix();
	MSI()->loadIdentity();
	
	glDisable(GL_DEPTH_TEST);
	if (!blend)
		glDisable(GL_BLEND);
	
	MSI()->orthof(0, _w, _h, 0, -1, 1);
	MSI()->color4f(1.0, 1.0, 1.0, 1.0);
}

void FBO::reset_drawing_mode() {
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	MSI()->matrixMode(MS_MODELVIEW);
	MSI()->popMatrix();
	MSI()->matrixMode(MS_PROJECTION);
	MSI()->popMatrix();
}

void FBO::draw_full(bool blend) {
	prepare_drawing_mode(blend);
	draw();
	reset_drawing_mode();
}

FBO::~FBO() {
	glDeleteFramebuffers(1, &_fbo);
	glDeleteRenderbuffers(1, &_depthBuffer);
}


void FBOSwapper::activate() {
	if (active)
		return;
	if (draw_to_first)
		first.activate(clear_on_activate);
	else
		second.activate(clear_on_activate);
	active = true;
	clear_on_activate = false;
}

void FBOSwapper::deactivate() {
	if (!active)
		return;
	if (draw_to_first)
		first.deactivate();
	else
		second.deactivate();
	active = false;
}

void FBOSwapper::swap() {
	deactivate();
	draw_to_first = !draw_to_first;
	clear_on_activate = true;
}

void FBOSwapper::draw(bool blend) {
	current_contents().draw_full(blend);
}

void FBOSwapper::filter(bool blend) {
	activate();
	draw(blend);
	swap();
}

void FBOSwapper::copy(FBO& other, bool srgb) {
	clear_on_activate = true;
	activate();
	other.draw_full(false);
	swap();
}

void FBOSwapper::blend(FBO& other, bool srgb) {
	activate();
	if (!srgb)
		glDisable(GL_FRAMEBUFFER_SRGB_EXT);
	else
		glEnable(GL_FRAMEBUFFER_SRGB_EXT);
	other.draw_full(true);
	deactivate();
}

void FBOSwapper::blend_multisample(FBO& other) {
	swap();
	activate();
	
	// set up FBO passed in as texture #1
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, other.texID);
	glEnable(GL_TEXTURE_RECTANGLE_ARB);
	glActiveTexture(GL_TEXTURE0);
	
	glClientActiveTexture(GL_TEXTURE1);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	GLint multi_coordinates[8] = { 0, GLint(other._h), GLint(other._w), GLint(other._h), GLint(other._w), 0, 0, 0 };
	glTexCoordPointer(2, GL_INT, 0, multi_coordinates);
	glClientActiveTexture(GL_TEXTURE0);
	
	draw(true);
	
	// tear down multitexture stuff
	glActiveTexture(GL_TEXTURE1);
	glDisable(GL_TEXTURE_RECTANGLE_ARB);
	glActiveTexture(GL_TEXTURE0);
	
	glClientActiveTexture(GL_TEXTURE1);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glClientActiveTexture(GL_TEXTURE0);
	
	deactivate();
}

#endif
