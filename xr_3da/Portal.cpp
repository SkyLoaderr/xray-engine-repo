// Portal.cpp: implementation of the CPortal class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Portal.h"
#include "render.h"
#include "xrLevel.h"
#include "xr_object.h"
#include "tempobject.h"
#include "fbasicvisual.h"

#include "xr_creator.h"
#include "std_classes.h"

static DWORD COLORS[8] = 
{
	D3DCOLOR_XRGB(0x7f,0x7f,0x7f),
	D3DCOLOR_XRGB(0xff,0x00,0x00),
	D3DCOLOR_XRGB(0x00,0xff,0x00),
	D3DCOLOR_XRGB(0x00,0x00,0xff),
	D3DCOLOR_XRGB(0xff,0xff,0x00),
	D3DCOLOR_XRGB(0x7f,0x7f,0xff),
	D3DCOLOR_XRGB(0xff,0x7f,0x00),
	D3DCOLOR_XRGB(0xff,0xff,0xff)
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//extern DWORD num_portals;

void drawPoly(sPoly &P, CPortal* Portal)
{
	Fplane PL;
	PL.build(P[0],P[1],P[2]);
	CSector* S=0;
	if (PL.classify(Device.vCameraPosition)<0)	S=Portal->Back	();
	else										S=Portal->Front	();
	
	Fvector Center,End;
	DWORD A_Mask = D3DCOLOR_RGBA(0xff,0xff,0xff,0x7f);
	DWORD C		 = COLORS[S->SelfID % 8];
	HW.pDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
	Center.add(P[0],P[1]);
	for (DWORD i=2; i<P.size(); i++)
	{
		Device.Primitive.dbg_DrawTRI(Fidentity,P[0],P[i-1],P[i],C);
		Center.add(P[i]);
	}
	Center.div	(float(P.size()));
	End.mad		(Center,PL.n,.5f);
	Device.Primitive.dbg_DrawLINE(Fidentity,Center,End,0xffffffff);
	HW.pDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);
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
	poly.assign			(V,V+vcnt);
	pFace				= face; 
	pBack				= back;
	dwFrame				= 0xffffffff; 

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

	VERIFY(fabsf(1-P.n.magnitude())<EPS_S);
}


//
CSector::~CSector()
{
	for (int i=0; i<int(tempObjects.size()); i++) 
		_DELETE(tempObjects[i]);
	tempObjects.clear();
}

//
extern float g_fSCREEN;
extern float ssaLIMIT;

void CSector::Render(CFrustum &F)
{
	// Render everything
	{
		Fvector	Tpos;
		::Render->set_Frustum	(&F);
		::Render->add_Glows		(Glows);
		::Render->add_Lights	(Lights);
		::Render->add_Geometry	(pRoot);
		
		// 1 sorting-pass on objects
		for (int s=0; s<int(Objects.size())-1; s++)
		{
			Fvector		C;
			Objects[s+0]->clCenter(C);	float	D1 = Device.vCameraPosition.distance_to_sqr(C);
			Objects[s+1]->clCenter(C);	float	D2 = Device.vCameraPosition.distance_to_sqr(C);
			if (D2<D1)	swap(Objects[s+0],Objects[s+1]);
		}

		// Persistant models
		vector<CObject*>::iterator I=Objects.begin(), E=Objects.end();
		for (; I!=E; I++) {
			CObject* O = *I;
			if (O->getVisible()) 
			{
				CVisual*	pV = O->Visual	();
				O->clXFORM().transform_tiny	(Tpos, pV->bv_Position);
				if (F.testSphere_dirty(Tpos,pV->bv_Radius))	
				{
					::Render->set_Object	(O);
					O->OnVisible			();
					::Render->set_Object	(0);
				}
			}
		}

		// Temporary models
		{
			for (int i=0; i<int(tempObjects.size()); i++) 
			{
				CTempObject* pV = tempObjects[i];
				if (pV->Alive())
				{
					DWORD planes	=	F.getMask();
					CVisual* V	=	pV->Visual();
					if (F.testSAABB(V->bv_Position,V->bv_Radius,V->bv_BBox.min,V->bv_BBox.max,planes)!=fcvNone)
						::Render->add_Geometry	(pV->Visual());
				}
				else
				{
					if (pV->IsAutomatic())
					{
						tempObjects.erase(tempObjects.begin()+i);
						_DELETE(pV);
						i--;
					}
				}
			}
		}
	}

	// Search visible portals and go through them
	CSector*	pLastSector		= ::Render->getSectorActive();
	for (DWORD I=0; I<Portals.size(); I++)
	{
		sPoly	S,D;
		if (Portals[I]->dwFrame != Device.dwFrame) {
			CPortal* PORTAL = Portals[I];
			CSector* pSector;

			if (PORTAL->bDualRender) {
				pSector = PORTAL->getSector(this);
			} else {
				pSector = PORTAL->getSectorBack		(Device.vCameraPosition);
				if (pSector==this)			continue;
				if (pSector==pLastSector)	continue;
			}

			// SSA
			Fvector	dir2portal;
			dir2portal.sub		(PORTAL->S.P,Device.vCameraPosition);
			float R				=	PORTAL->S.R;
			float distSQ		=	dir2portal.square_magnitude();
			float ssa			=	g_fSCREEN*R*R/distSQ;
			dir2portal.div		(_sqrt(distSQ));
			ssa					*=	fabsf(PORTAL->P.n.dotproduct(dir2portal));
			if (ssa<ssaLIMIT)	continue;

			// Clip by frustum
			vector<Fvector> &POLY = PORTAL->getPoly();
			S.assign			(POLY.begin(),POLY.size()); D.clear();
			sPoly* P			= F.ClipPoly(S,D);
			if (0==P)			continue;
			
			// Cull by HOM
			if (!::Render->occ_visible(*P))	continue;

			// Create new frustum and recurse
			CFrustum Clip;
			Clip.CreateFromPortal(P,Device.vCameraPosition,Device.mFullTransform);
			PORTAL->dwFrame		= Device.dwFrame;
			PORTAL->bDualRender	= FALSE;
			pSector->Render		(Clip);
		}
	}
}

static CSector::objQuery	oQuery = { 0xfefefefe, 0, 0, 0 };

void CSector::GetObjects	(CFrustum& F, Fvector& vBase, Fmatrix& mFullXFORM, objSET &D, objQualifier* Q, void* P)
{
	oQuery.dwMark		++;
	oQuery.Collector	= &D;
	oQuery.Qualifier	= Q;
	oQuery.Param		= P;

	ll_GetObjects		(F,vBase,mFullXFORM);
}

void CSector::ll_GetObjects	(CFrustum& F, Fvector& vBase, Fmatrix& mFullXFORM)
{
	// Traverse objects
	{
		vector<CObject*>::iterator I=Objects.begin(), E=Objects.end();
		for (; I!=E; I++) 
		{
			CObject*	O	= *I;
			if (!oQuery.Qualifier(O,oQuery.Param))	continue;

			Fvector		Pos;
			O->clXFORM().transform_tiny	(Pos,O->Visual()->bv_Position);
			if (F.testSphere_dirty	(Pos,O->Radius()))
				oQuery.Collector->push_back(O);
		}
	}

	// Search visible portals and go through them
	for (DWORD I=0; I<Portals.size(); I++)
	{
		sPoly	S,D;
		if (Portals[I]->dwFrameObject != oQuery.dwMark) {
			vector<Fvector> &POLY = Portals[I]->getPoly();
			S.assign(POLY.begin(),POLY.size()); D.clear();
			
			// Clip by frustum
			sPoly* P	= F.ClipPoly(S,D);
			if (0==P)	continue;

			// Recurse
			CFrustum Clip;
			Clip.CreateFromPortal		(P,vBase,mFullXFORM);
			Portals[I]->dwFrameObject	= oQuery.dwMark;
			Portals[I]->getSector(this)->ll_GetObjects(Clip,vBase,mFullXFORM);
		}
	}
}

void CSector::DebugDump()
{
	Fbox&	B = pRoot->bv_BBox;
	Fvector C;
	B.getcenter	(C);

	Log("-------------",SelfID);
	for (DWORD i=0; i<Portals.size(); i++)
	{
		CPortal* P			= Portals[i];
		vector<Fvector>& V	= P->getPoly();
		Fplane PL;			PL.build(V[0],V[1],V[2]);

		Msg("#%d --- verts: %d, front(%d), back(%d), F(%d), B(%d), (%.2f,%.2f,%.2f)",
			i,
			V.size(),
			P->Front()->SelfID,
			P->Back()->SelfID,
			P->getSectorFacing(C)->SelfID,
			P->getSectorBack(C)->SelfID,
			VPUSH(PL.n)
			);
	}
}

void CSector::Load(CStream& fs)
{
	// Assign portal polygons
	DWORD size	= fs.FindChunk(fsP_Portals); R_ASSERT(0==(size&1));
	DWORD count	= size/2;
	while (count) {
		WORD ID		= fs.Rword();
		CPortal* P	= ::Render->getPortal	(ID);
		Portals.push_back(P);
		count--;
	}

	// Assign visual
	size = fs.FindChunk(fsP_Root);	R_ASSERT(size==4);
	pRoot = ::Render->getVisual(fs.Rdword());

	// Load glows
	size	= fs.FindChunk(fsP_Glows);
	if (size) {
		R_ASSERT(0==(size&1));
		count	= size/sizeof(WORD);
		Glows.resize(count);
		fs.Read(Glows.begin(),size);
	}

	// Load lights
	size	= fs.FindChunk(fsP_Lights);	
	if (size) {
		R_ASSERT(0==(size&1));
		count	= size/sizeof(WORD);
		Lights.resize(count);
		fs.Read(Lights.begin(),size);
	}
}
