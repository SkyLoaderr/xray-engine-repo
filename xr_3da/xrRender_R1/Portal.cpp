// Portal.cpp: implementation of the CPortal class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Portal.h"
#include "..\xrLevel.h"
#include "..\xr_object.h"
#include "..\fbasicvisual.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CPortal::~CPortal()
{
}

//
void	CPortal::Setup	(Fvector* V, int vcnt, CSector* face, CSector* back)
{
	// calc sphere
	Fbox				BB;
	BB.invalidate		();
	for (int v=0; v<vcnt; v++)
		BB.modify		(V[v]);
	BB.getsphere		(S.P,S.R);

	// 
	poly.assign			(V,vcnt);
	pFace				= face; 
	pBack				= back;
	marker				= 0xffffffff; 

	Fvector				N,T;
	N.set				(0,0,0);

	FPU::m64r();
	for (int i=2; i<vcnt; i++) {
		T.mknormal	(poly[0],poly[i-1],poly[i]);
		N.add		(T);
	}
	float	tcnt = float(vcnt)-2;
	N.div	(tcnt);
	P.build	(poly[0],N);
	FPU::m24r();

	R_ASSERT(_abs(1-P.n.magnitude())<EPS);
}

//
CSector::~CSector()
{
}

//
extern float r_ssaDISCARD;

void CSector::traverse			(CFrustum &F)
{
	// Register traversal process
	if (r_marker	!=	PortalTraverser.i_marker)	
	{
		r_marker							=	PortalTraverser.i_marker;
		PortalTraverser.r_sectors.push_back	(this);
		r_frustums.clear					();
	}
	r_frustums.push_back		(F);

	// Search visible portals and go through them
	sPoly	S,D;
	for	(u32 I=0; I<m_portals.size(); I++)
	{
		if (m_portals[I]->marker == PortalTraverser.i_marker) continue;

		CPortal* PORTAL = m_portals[I];
		CSector* pSector;

		// Select sector (allow intersecting portals to be finely classified)
		if (PORTAL->bDualRender) {
			pSector = PORTAL->getSector				(this);
		} else {
			pSector = PORTAL->getSectorBack			(PortalTraverser.i_vBase);
			if (pSector==this)						continue;
			if (pSector==PortalTraverser.i_start)	continue;
		}

		// SSA	(if required)
		if (PortalTraverser.i_options&CPortalTraverser::VQ_SSA)
		{
			Fvector	dir2portal;
			dir2portal.sub		(PORTAL->S.P,		PortalTraverser.i_vBase);
			float R				=	PORTAL->S.R;
			float distSQ		=	dir2portal.square_magnitude();
			float ssa			=	R*R/distSQ;
			dir2portal.div		(_sqrt(distSQ));
			ssa					*=	_abs(PORTAL->P.n.dotproduct(dir2portal));
			if (ssa<r_ssaDISCARD)	continue;
		}

		// Clip by frustum
		svector<Fvector,8>&	POLY = PORTAL->getPoly();
		S.assign			(&*POLY.begin(),POLY.size()); D.clear();
		sPoly* P			= F.ClipPoly(S,D);
		if (0==P)			continue;
			
		// Cull by HOM
		if ((PortalTraverser.i_options&CPortalTraverser::VQ_HOM) && (!RImplementation.occ_visible(*P)))	continue;

		// Create _new_ frustum and recurse
		CFrustum				Clip;
		Clip.CreateFromPortal	(P,PortalTraverser.i_vBase,PortalTraverser.i_mXFORM);
		PORTAL->marker			= PortalTraverser.i_marker;
		PORTAL->bDualRender		= FALSE;
		pSector->traverse		(Clip);
	}
}

void CSector::load(IReader& fs)
{
	// Assign portal polygons
	u32 size			= fs.find_chunk(fsP_Portals); R_ASSERT(0==(size&1));
	u32 count			= size/2;
	m_portals.reserve	(count);
	while (count) {
		WORD ID		= fs.r_u16();
		CPortal* P	= (CPortal*)RImplementation.getPortal	(ID);
		m_portals.push_back(P);
		count--;
	}

	// Assign visual
	size	= fs.find_chunk(fsP_Root);	R_ASSERT(size==4);
	m_root	= RImplementation.getVisual(fs.r_u32());
}
