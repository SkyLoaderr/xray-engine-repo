#include "stdafx.h"
#include "xrCDB.h"
#include "frustum.h"

using namespace CDB;
using namespace Opcode;

template <bool bClass3, bool bFirst>
class frustum_collider
{
public:
	COLLIDER*		dest;
	TRI*			tris;
	
	const CFrustum*	F;
	
	IC void			_init		(COLLIDER* CL, TRI* T, const CFrustum* _F)
	{
		dest		= CL;
		tris		= T;
		F			= _F;
	}
	IC EFC_Visible	_box		(Fvector& C, Fvector& E, u32& mask)
	{
		Fvector		m,M;
		m.sub		(C,E);
		M.add		(C,E);
		return F->testAABB		(m,M,mask);
	}
	void			_prim		(DWORD prim)
	{
		if (bClass3)	{
			sPoly		src,dst;
			src.resize	(3);
			src[0]		= *tris[prim].verts[0];
			src[1]		= *tris[prim].verts[1];
			src[2]		= *tris[prim].verts[2];
			if (F->ClipPoly(src,dst))
			{
				RESULT& R	= dest->r_add();
				R.id		= prim;
			}
		} else {
			RESULT& R	= dest->r_add();
			R.id		= prim;
		}
	}
	
	void			_stab		(const AABBNoLeafNode* node, u32 mask)
	{
		// Actual frustum/aabb test
		EFC_Visible	result		= _box((Fvector&)node->mAABB.mCenter,(Fvector&)node->mAABB.mExtents,mask);
		if (fcvNone == result)	return;
		
		// 1st chield
		if (node->HasLeaf())	_prim	(node->GetPrimitive());
		else					_stab	(node->GetPos(),mask);
		
		// Early exit for "only first"
		if (bFirst && dest->r_count())												return;
		
		// 2nd chield
		if (node->HasLeaf2())	_prim	(node->GetPrimitive2());
		else					_stab	(node->GetNeg(),mask);
	}
};

void COLLIDER::frustum_query(const MODEL *m_def, const CFrustum& F)
{
	// Get nodes
	const AABBNoLeafTree*	T	= (const AABBNoLeafTree*)m_def->tree->GetTree();
	const AABBNoLeafNode*	N	= T->GetNodes();
	const DWORD				mask= F.getMask();
	r_clear	();
	
	// Binary dispatcher
	if (frustum_mode&OPT_FULL_TEST) 
	{
		if (frustum_mode&OPT_ONLYFIRST)
		{
			frustum_collider<true,true> BC;
			BC._init	(this,m_def->tris,&F);
			BC._stab	(N,mask);
		} else {
			frustum_collider<true,false> BC;
			BC._init	(this,m_def->tris,&F);
			BC._stab	(N,mask);
		}
	} else {
		if (frustum_mode&OPT_ONLYFIRST)
		{
			frustum_collider<false,true> BC;
			BC._init	(this,m_def->tris,&F);
			BC._stab	(N,mask);
		} else {
			frustum_collider<false,false> BC;
			BC._init	(this,m_def->tris,&F);
			BC._stab	(N,mask);
		}
	}
}
