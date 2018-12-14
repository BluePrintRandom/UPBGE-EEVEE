/*
 * ***** BEGIN GPL LICENSE BLOCK *****
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software  Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * Copyright (c) 2006 The Zdeno Ash Miklas
 *
 * This source file is part of VideoTexture library
 *
 * Contributor(s):
 *
 * ***** END GPL LICENSE BLOCK *****
 */

/** \file VideoTexture/Texture.h
 *  \ingroup bgevideotex
 */

#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include "EXP_Value.h"

#include "DNA_image_types.h"

#include "ImageBase.h"
#include "Exception.h"

struct ImBuf;
class RAS_Texture;
class RAS_IMaterial;
class KX_Scene;
class KX_GameObject;

// type Texture declaration
class Texture : public EXP_Value
{
	Py_Header
protected:
	virtual void DestructFromPython();

public:
	// texture is using blender material
	bool m_useMatTexture;

	// video texture bind code
	unsigned int m_actTex;
	// original texture bind code
	unsigned int m_orgTex;
	// original image bind code
	unsigned int m_orgImg;
	// original texture saved
	bool m_orgSaved;

	// kernel image buffer, to make sure the image is loaded before we swap the bindcode
	struct ImBuf *m_imgBuf;
	// texture image for game materials
	Image * m_imgTexture;
	// texture for blender materials
	RAS_Texture * m_matTexture;

	KX_Scene *m_scene;

	// use mipmapping
	bool m_mipmap;

	// scaled image buffer
	ImBuf * m_scaledImBuf;
	// last refresh
	double m_lastClock;

	// image source
	PyImage * m_source;

	Texture();
	virtual ~Texture();

	virtual std::string GetName();

	void Close();
	void SetSource(PyImage *source);

	static void FreeAllTextures(KX_Scene *scene);

	EXP_PYMETHOD_DOC(Texture, close);
	EXP_PYMETHOD_DOC(Texture, refresh);

	static PyObject *pyattr_get_mipmap(EXP_PyObjectPlus *self_v, const EXP_PYATTRIBUTE_DEF *attrdef);
	static int pyattr_set_mipmap(EXP_PyObjectPlus *self_v, const EXP_PYATTRIBUTE_DEF *attrdef, PyObject *value);
	static PyObject *pyattr_get_source(EXP_PyObjectPlus *self_v, const EXP_PYATTRIBUTE_DEF *attrdef);
	static int pyattr_set_source(EXP_PyObjectPlus *self_v, const EXP_PYATTRIBUTE_DEF *attrdef, PyObject *value);
	static PyObject *pyattr_get_bindId(EXP_PyObjectPlus *self_v, const EXP_PYATTRIBUTE_DEF *attrdef);
	static int pyattr_set_bindId(EXP_PyObjectPlus *self_v, const EXP_PYATTRIBUTE_DEF *attrdef, PyObject *value);
};

// load texture
void loadTexture(unsigned int texId, unsigned int *texture, short *size,
				 bool mipmap, unsigned int internalFormat);

// get material
RAS_IMaterial *getMaterial(KX_GameObject *gameObj, short matID);

// get material ID
short getMaterialID(PyObject *obj, const char *name);

// Exceptions
extern ExceptionID MaterialNotAvail;
extern ExceptionID TextureNotAvail;

#endif
