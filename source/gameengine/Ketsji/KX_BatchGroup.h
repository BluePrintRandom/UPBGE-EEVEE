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

/** \file KX_BatchGroup.h
*  \ingroup ketsji
*/

#ifndef __KX_BATCH_GROUP_H__
#define __KX_BATCH_GROUP_H__

#include "RAS_BatchGroup.h"
#include "EXP_Value.h"

class KX_GameObject;
template <class ItemType>
class EXP_ListValue;

class KX_BatchGroup : public EXP_Value, public RAS_BatchGroup
{
	Py_Header
private:
	/// The objects currently merged in the batch group.
	EXP_ListValue<KX_GameObject> *m_objects;
	/// Reference object used to retrieve layer and color.
	KX_GameObject *m_referenceObject;

public:
	KX_BatchGroup();
	virtual ~KX_BatchGroup();

	virtual std::string GetName();

	EXP_ListValue<KX_GameObject> *GetObjects() const;

	KX_GameObject *GetReferenceObject() const;
	/// Set reference object with error checking. Return false on error.
	bool SetReferenceObject(KX_GameObject *object);

	/** Merge a list of objects using their mesh user and transformation.
	 * \param objects The list of objects to merge.
	 */
	void MergeObjects(const std::vector<KX_GameObject *>& objects);

	/** Split a list of objects.
	 * \param objects The object to split, remove.
	 */
	void SplitObjects(const std::vector<KX_GameObject *>& objects);

#ifdef WITH_PYTHON

	static PyObject *pyattr_get_objects(EXP_PyObjectPlus *self_v, const EXP_PYATTRIBUTE_DEF *attrdef);
	static PyObject *pyattr_get_referenceObject(EXP_PyObjectPlus *self_v, const EXP_PYATTRIBUTE_DEF *attrdef);
	static int pyattr_set_referenceObject(EXP_PyObjectPlus *self_v, const EXP_PYATTRIBUTE_DEF *attrdef, PyObject *value);

	EXP_PYMETHOD_DOC(KX_BatchGroup, merge);
	EXP_PYMETHOD_DOC(KX_BatchGroup, split);
	EXP_PYMETHOD_DOC(KX_BatchGroup, destruct);

#endif
};

#endif  // __KX_BATCH_GROUP_H__
