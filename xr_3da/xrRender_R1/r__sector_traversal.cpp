#include "stdafx.h"

CPortalTraverser	PortalTraverser;

CPortalTraverser::CPortalTraverser	()
{
	i_marker			=	0xffffffff;
}
void CPortalTraverser::traverse			(IRender_Sector* start, CFrustum& F, Fvector& vBase, Fmatrix& mXFORM, u32 options)
{
	Fmatrix			m_viewport_01	= {
		1.f/2.f,			0.0f,				0.0f,		0.0f,
		0.0f,				-1.f/2.f,			0.0f,		0.0f,
		0.0f,				0.0f,				1.0f,		0.0f,
		1.f/2.f + 0 + 0,	1.f/2.f + 0 + 0,	0.0f,		1.0f
	};

	VERIFY				(start);
	i_marker			++;
	i_options			= options;
	i_vBase				= vBase;
	i_mXFORM			= mXFORM;
	i_mXFORM_01.mul		(m_viewport_01,mXFORM);
	i_start				= (CSector*)start;
	r_sectors.clear		();
	Fbox2				scissor;
	scissor.set			(0,0,1,1);
	i_start->traverse	(F,scissor);
}
