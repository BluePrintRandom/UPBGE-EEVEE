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
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * Contributor(s): Tristan Porteries.
 *
 * ***** END GPL LICENSE BLOCK *****
 */

/** \file gameengine/Ketsji/KX_2DFilterOffScreen.cpp
 *  \ingroup ketsji
 */

#include "KX_2DFilterOffScreen.h"

#include "EXP_ListWrapper.h"

KX_2DFilterOffScreen::KX_2DFilterOffScreen(unsigned short colorSlots, Flag flag, unsigned int width, unsigned int height,
                                           RAS_Rasterizer::HdrType hdr)
	:RAS_2DFilterOffScreen(colorSlots, flag, width, height, hdr)
{
}

KX_2DFilterOffScreen::~KX_2DFilterOffScreen()
{
}

std::string KX_2DFilterOffScreen::GetName()
{
	return "KX_2DFilterOffScreen";
}

#ifdef WITH_PYTHON

PyTypeObject KX_2DFilterOffScreen::Type = {
	PyVarObject_HEAD_INIT(nullptr, 0)
	"KX_2DFilterOffScreen",
	sizeof(EXP_PyObjectPlus_Proxy),
	0,
	py_base_dealloc,
	0,
	0,
	0,
	0,
	py_base_repr,
	0, 0, 0, 0, 0, 0, 0, 0, 0,
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
	0, 0, 0, 0, 0, 0, 0,
	Methods,
	0,
	0,
	&EXP_Value::Type,
	0, 0, 0, 0, 0, 0,
	py_base_new
};

PyMethodDef KX_2DFilterOffScreen::Methods[] = {
	{nullptr, nullptr} // Sentinel
};

PyAttributeDef KX_2DFilterOffScreen::Attributes[] = {
	EXP_PYATTRIBUTE_RO_FUNCTION("width", KX_2DFilterOffScreen, pyattr_get_width),
	EXP_PYATTRIBUTE_RO_FUNCTION("height", KX_2DFilterOffScreen, pyattr_get_height),
	EXP_PYATTRIBUTE_RO_FUNCTION("colorBindCodes", KX_2DFilterOffScreen, pyattr_get_colorBindCodes),
	EXP_PYATTRIBUTE_RO_FUNCTION("depthBindCode", KX_2DFilterOffScreen, pyattr_get_depthBindCode),
	EXP_PYATTRIBUTE_NULL // Sentinel
};

PyObject *KX_2DFilterOffScreen::pyattr_get_width(EXP_PyObjectPlus *self_v, const EXP_PYATTRIBUTE_DEF *attrdef)
{
	KX_2DFilterOffScreen *self = static_cast<KX_2DFilterOffScreen *>(self_v);
	return PyLong_FromLong(self->GetWidth());
}

PyObject *KX_2DFilterOffScreen::pyattr_get_height(EXP_PyObjectPlus *self_v, const EXP_PYATTRIBUTE_DEF *attrdef)
{
	KX_2DFilterOffScreen *self = static_cast<KX_2DFilterOffScreen *>(self_v);
	return PyLong_FromLong(self->GetHeight());
}

unsigned int KX_2DFilterOffScreen::py_get_textures_size()
{
	return RAS_2DFilterOffScreen::NUM_COLOR_SLOTS;
}

PyObject *KX_2DFilterOffScreen::py_get_textures_item(unsigned int index)
{
	const int bindCode = GetColorBindCode(index);
	return PyLong_FromLong(bindCode);
}

PyObject *KX_2DFilterOffScreen::pyattr_get_colorBindCodes(EXP_PyObjectPlus *self_v, const EXP_PYATTRIBUTE_DEF *attrdef)
{
	return (new EXP_ListWrapper<KX_2DFilterOffScreen, &KX_2DFilterOffScreen::py_get_textures_size, &KX_2DFilterOffScreen::py_get_textures_item>(self_v))->NewProxy(true);
}

PyObject *KX_2DFilterOffScreen::pyattr_get_depthBindCode(EXP_PyObjectPlus *self_v, const EXP_PYATTRIBUTE_DEF *attrdef)
{
	KX_2DFilterOffScreen *self = static_cast<KX_2DFilterOffScreen *>(self_v);
	return PyLong_FromLong(self->GetDepthBindCode());
}

#endif  // WITH_PYTHON
