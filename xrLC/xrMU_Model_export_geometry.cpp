#include "stdafx.h"
#include "xrMU_Model.h"

const u32	max_tile	= 16;
const s32	quant		= 32768/max_tile;

s16 QC	(float v)
{
	int t		=	iFloor(v*float(quant)); clamp(t,-quant,quant);
	return	s16	(t);
}

D3DVERTEXELEMENT9	decl[] = 
{
	{0, 0,  D3DDECLTYPE_FLOAT3,		D3DDECLMETHOD_DEFAULT, 	D3DDECLUSAGE_POSITION,	0 },
	{0, 12, D3DDECLTYPE_D3DCOLOR,	D3DDECLMETHOD_DEFAULT, 	D3DDECLUSAGE_COLOR,		0 },
	{0, 16, D3DDECLTYPE_SHORT2,		D3DDECLMETHOD_DEFAULT, 	D3DDECLUSAGE_TEXCOORD,	0 },
	D3DDECL_END()
};

void	xrMU_Model::export_geometry		()
{
	// Declarator
	VDeclarator			D;
	D.set				(decl);

	// RT-check
	R_ASSERT		(m_vertices.size()==color.size());

	for (xrMU_Model::v_subdivs_it it=m_subdivs.begin(); it!=m_subdivs.end(); it++)
	{
		// Vertices
		{
			g_VB.Begin		(D);

			vecOGF_V&	verts	= it->ogf->vertices;
			for (u32 v_it=0; v_it<verts.size(); v_it++)
			{
				OGF_Vertex&		oV	= verts[v_it];

				// Position
				g_VB.Add	(&oV.P,3*sizeof(float));

				// Color
				g_VB.Add	(&oV.Color,4);

				// TC
				s16	tu,tv;
				tu			= QC(oV.UV.begin()->x);
				tv			= QC(oV.UV.begin()->y);
				g_VB.Add	(&tu,2);
				g_VB.Add	(&tv,2);
			}

			g_VB.End		(&it->vb_id,&it->vb_start);
		}

		// Indices
		g_IB.Register	(LPWORD(it->ogf->faces.begin()),LPWORD(it->ogf->faces.end()),&it->ib_id,&it->ib_start);
	}
}
