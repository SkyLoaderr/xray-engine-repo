#include "stdafx.h"
#include "xrMU_Model.h"

bool	cmp_face_material		(xrMU_Model::_face* f1, xrMU_Model::_face* f2)
{
	return f1->dwMaterial < f2->dwMaterial;
}

void xrMU_Model::calc_materials	()
{
	std::sort	(m_faces.begin(),m_faces.end(),cmp_face_material);
}
