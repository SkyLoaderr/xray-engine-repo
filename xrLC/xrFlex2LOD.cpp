#include "stdafx.h"

void	CBuild::Flex2LOD()
{
	int complete = g_tree.size();
	for (int it=0; it<int(lods.size()); it++)
	{
		Progress(float(it)/float(lods.size()));

		// Create Node and fill it with information
		b_lod&		LOD		= lods[it];
		OGF_LOD*	pNode	= new OGF_LOD(1,pBuild->materials[LOD.dwMaterial].sector);
		pNode->lod_Material	= LOD.dwMaterial;
		for (int lf=0; lf<8; lf++)
		{
			b_lod_face&		F = LOD.faces[lf];
			OGF_LOD::_face& D = pNode->lod_faces[lf];
			for (int lv=0; lv<4; lv++)
			{
				D.v[lv].v	= F.v[lv];
				D.v[lv].t	= F.t[lv];
				D.v[lv].c	= 0xffffffff;
			}
		}

		// Search all 'OGFs' with such LOD-id
		for (int o=0; o<complete; o++)
		{
			OGF*	P		= (OGF*)g_tree[o];
			int		lod_id	= pBuild->materials[P->material].lod_id;
			if (it!=lod_id)	continue;
			pNode->AddChield(o);
		}

		// Register node
		R_ASSERT						(pNode->chields.size());
		pNode->CalcBounds				();
		g_tree.push_back				(pNode);
	}
}
