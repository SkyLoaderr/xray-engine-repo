#include "stdafx.h"

void xrMU_Reference::export_cform_rcast	(CDB::CollectorPacked& CL)
{
	xrMU_Model::v_faces&		m_faces		= model->m_faces;

	for		(u32 fit=0; fit<m_faces.size(); fit++)	m_faces[fit]->bProcessed = FALSE;

	xrMU_Model::v_faces			adjacent;	adjacent.reserve(6*2*3);

	for (xrMU_Model::v_faces_it it = m_faces.begin(); it!=m_faces.end(); it++)
	{
		xrMU_Model::_face*	F	= (*it);

		// Collect
		adjacent.clear	();
		for (int vit=0; vit<3; vit++)
		{
			xrMU_Model::_vertex* V	= F->v[vit];
			for (u32 adj=0; adj<V->adjacent.size(); adj++)
			{
				adjacent.push_back(V->adjacent[adj]);
			}
		}

		// Unique
		std::sort		(adjacent.begin(),adjacent.end());
		adjacent.erase	(std::unique(adjacent.begin(),adjacent.end()),adjacent.end());
		BOOL			bAlready	= FALSE;
		for (DWORD ait=0; ait<adjacent.size(); ait++)
		{
			xrMU_Model::_face*	Test	= adjacent[ait];
			if (Test==F)				continue;
			if (!Test->bProcessed)		continue;
			if (F->isEqual(*Test))
			{
				bAlready			= TRUE;
				break;
			}
		}

		//
		if (!bAlready) 
		{
			F->bProcessed			= true;
			Fvector					P[3];
			xform.transform_tiny	(P[0],F->v[0]->P);
			xform.transform_tiny	(P[1],F->v[1]->P);
			xform.transform_tiny	(P[2],F->v[2]->P);
			CL.add_face(
				P[0],P[1],P[2],
				CDB::edge_open,CDB::edge_open,CDB::edge_open,
				0,0,DWORD(F)
				);
		}
	}
}
