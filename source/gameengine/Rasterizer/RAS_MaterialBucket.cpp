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
 * The Original Code is Copyright (C) 2001-2002 by NaN Holding BV.
 * All rights reserved.
 *
 * The Original Code is: all of this file.
 *
 * Contributor(s): none yet.
 *
 * ***** END GPL LICENSE BLOCK *****
 */

/** \file gameengine/Rasterizer/RAS_MaterialBucket.cpp
 *  \ingroup bgerast
 */

#include "RAS_MaterialBucket.h"
#include "RAS_IMaterial.h"
#include "RAS_Rasterizer.h"
#include "RAS_Mesh.h"
#include "RAS_MeshUser.h"
#include "RAS_Deformer.h"

#include "CM_List.h"

#ifdef _MSC_VER
#  pragma warning (disable:4786)
#endif

#ifdef WIN32
#  include <windows.h>
#endif // WIN32

RAS_MaterialBucket::RAS_MaterialBucket(RAS_IMaterial *mat)
	:m_material(mat),
	m_downwardNode(this, &m_nodeData, &RAS_MaterialBucket::BindNode, &RAS_MaterialBucket::UnbindNode),
	m_upwardNode(this, &m_nodeData, &RAS_MaterialBucket::BindNode, &RAS_MaterialBucket::UnbindNode)
{
	m_nodeData.m_material = m_material;
	m_nodeData.m_drawingMode = m_material->GetDrawingMode();
	m_nodeData.m_cullFace = m_material->IsCullFace();
	m_nodeData.m_zsort = m_material->IsZSort();
	m_nodeData.m_text = m_material->IsText();
	m_nodeData.m_zoffset = m_material->GetZOffset();
}

RAS_MaterialBucket::~RAS_MaterialBucket()
{
}

RAS_IMaterial *RAS_MaterialBucket::GetMaterial() const
{
	return m_material;
}

bool RAS_MaterialBucket::IsAlpha() const
{
	return (m_material->IsAlpha());
}

bool RAS_MaterialBucket::IsZSort() const
{
	return (m_material->IsZSort());
}

bool RAS_MaterialBucket::IsWire() const
{
	return (m_material->IsWire());
}

bool RAS_MaterialBucket::UseInstancing() const
{
	return (m_material->UseInstancing());
}

void RAS_MaterialBucket::RemoveActiveMeshSlots()
{
	for (RAS_DisplayArrayBucket *arrayBucket : m_displayArrayBucketList) {
		arrayBucket->RemoveActiveMeshSlots();
	}
}

void RAS_MaterialBucket::ActivateMaterial(RAS_Rasterizer *rasty)
{
	m_material->Activate(rasty);
}

void RAS_MaterialBucket::DesactivateMaterial(RAS_Rasterizer *rasty)
{
	m_material->Desactivate(rasty);
}

void RAS_MaterialBucket::GenerateTree(RAS_ManagerDownwardNode& downwardRoot, RAS_ManagerUpwardNode& upwardRoot,
                                      RAS_UpwardTreeLeafs& upwardLeafs, RAS_Rasterizer::DrawType drawingMode, bool sort)
{
	if (m_displayArrayBucketList.empty()) {
		return;
	}

	const bool instancing = UseInstancing();
	for (RAS_DisplayArrayBucket *displayArrayBucket : m_displayArrayBucketList) {
		displayArrayBucket->GenerateTree(m_downwardNode, m_upwardNode, upwardLeafs, drawingMode, sort, instancing);
	}

	downwardRoot.AddChild(&m_downwardNode);

	if (sort) {
		m_upwardNode.SetParent(&upwardRoot);
	}
}

void RAS_MaterialBucket::BindNode(const RAS_MaterialNodeTuple& tuple)
{
	RAS_ManagerNodeData *managerData = tuple.m_managerData;
	RAS_Rasterizer *rasty = managerData->m_rasty;
	rasty->SetCullFace(m_nodeData.m_cullFace);
	rasty->SetPolygonOffset(managerData->m_drawingMode, -m_nodeData.m_zoffset, 0.0f);

	if (!managerData->m_shaderOverride) {
		ActivateMaterial(managerData->m_rasty);
	}
}

void RAS_MaterialBucket::UnbindNode(const RAS_MaterialNodeTuple& tuple)
{
	RAS_ManagerNodeData *managerData = tuple.m_managerData;
	if (!managerData->m_shaderOverride) {
		DesactivateMaterial(managerData->m_rasty);
	}
}

void RAS_MaterialBucket::AddDisplayArrayBucket(RAS_DisplayArrayBucket *bucket)
{
	m_displayArrayBucketList.push_back(bucket);
}

void RAS_MaterialBucket::RemoveDisplayArrayBucket(RAS_DisplayArrayBucket *bucket)
{
	CM_ListRemoveIfFound(m_displayArrayBucketList, bucket);
}

RAS_DisplayArrayBucketList& RAS_MaterialBucket::GetDisplayArrayBucketList()
{
	return m_displayArrayBucketList;
}

void RAS_MaterialBucket::MoveDisplayArrayBucket(RAS_MeshMaterial *meshmat, RAS_MaterialBucket *bucket)
{
	for (RAS_DisplayArrayBucketList::iterator dit = m_displayArrayBucketList.begin(); dit != m_displayArrayBucketList.end(); ) {
		// In case of deformers, multiple display array bucket can use the same mesh and material.
		RAS_DisplayArrayBucket *displayArrayBucket = *dit;
		if (displayArrayBucket->GetMeshMaterial() != meshmat) {
			++dit;
			continue;
		}

		displayArrayBucket->ChangeMaterialBucket(bucket);
		bucket->AddDisplayArrayBucket(displayArrayBucket);
		dit = m_displayArrayBucketList.erase(dit);
	}
}
