// Portal.cpp: implementation of the CPortal class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Portal.h"
#include "fstaticrender.h"
#include "xrLevel.h"
#include "xr_object.h"
#include "tempobject.h"

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
		Device.Primitive.dbg_DrawTRI(precalc_identity,P[0],P[i-1],P[i],C);
		Center.add(P[i]);
	}
	Center.div(float(P.size()));
	End.direct(Center,PL.n,.5f);
	Device.Primitive.dbg_DrawLINE(precalc_identity,Center,End,0xffffffff);
	HW.pDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);
}

CSector::~CSector()
{
	for (int i=0; i<int(tempObjects.size()); i++) 
		_DELETE(tempObjects[i]);
	tempObjects.clear();
}

void CSector::Render(CFrustum &F)
{
//	CHK_DX(HW.pDevice->SetRenderState(D3DRS_AMBIENT,COLORS[SelfID%8]));

//	num_portals++;

	// Occluders
	Occluders.Select	(F,Device.vCameraPosition,Device.mFullTransform);
	CVisiCache vcc; 
	Occluders.InitCache	(vcc);

	// Render everything
//	float l_f = Device.fTimeDelta*fLightSmoothFactor;
//	float l_i = 1.f-l_f;
	{
		Fvector	Tpos;
		::Render.set_Occluders	(&Occluders);
		::Render.add_Glows		(Glows);
		::Render.add_Lights		(Lights);
		::Render.add_Static		(pRoot,vcc);
		
		vector<CObject*>::iterator I=Objects.begin(), E=Objects.end();
		for (; I!=E; I++) {
			CObject* O = *I;
			if (O->bVisible) 
			{
				FBasicVisual*	pV = O->Visual();
				O->clTransform.transform_tiny(Tpos, pV->bv_Position);
				if (Occluders.visibleSphereNC(Tpos,pV->bv_Radius)!=fcvNone)
				{
					float LL					= O->OnVisible	(); 
					::Render.set_Transform		(&(O->clTransform));
					::Render.set_LightLevel		(iFloor(LL));
					::Render.add_leafs_Dynamic	(pV);
				}
			}
		}

		// Visuals
		{
			for (int i=0; i<int(tempObjects.size()); i++) 
			{
				CTempObject* pV = tempObjects[i];
				if (pV->Alive())
				{
					Occluders.InitCache	(vcc);
					if (Occluders.visibleVisual(vcc,pV->Visual())!=fcvNone)
						::Render.add_leafs_Static(pV->Visual());
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
	for (DWORD I=0; I<Portals.size(); I++)
	{
		sPoly	S,D;
		if (Portals[I]->dwFrame != Device.dwFrame) {
			CPortal* PORTAL = Portals[I];
			CSector* pSector;

			if (PORTAL->bDualRender) {
				pSector = PORTAL->getSector(this);
			} else {
				pSector = PORTAL->getSectorBack(Device.vCameraPosition);
				if (pSector==this) continue;
				if (pSector==::Render.pLastSector) continue;
			}
			vector<Fvector> &POLY = PORTAL->getPoly();
			S.assign(POLY.begin(),POLY.size()); D.clear();

			sPoly*	P = Occluders.clipPortal(S,D);
			if (P) {
				CFrustum Clip;
				Clip.CreateFromPortal(P,Device.vCameraPosition,Device.mFullTransform);
				PORTAL->dwFrame		= Device.dwFrame;
				PORTAL->bDualRender	= FALSE;
				pSector->Render(Clip);
			}
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
	// Occluders
	Occluders.Select(F,vBase,mFullXFORM);

	// Traverse objects
	{
		vector<CObject*>::iterator I=Objects.begin(), E=Objects.end();
		for (; I!=E; I++) 
		{
			CObject*	O	= *I;
			if (!oQuery.Qualifier(O,oQuery.Param))	continue;

			Fvector		Pos;
			O->clTransform.transform_tiny	(Pos,O->Visual()->bv_Position);
			if (Occluders.visibleSphereNC	(Pos,O->Radius()))
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
			sPoly*	P = Occluders.clipPortal(S,D);
			if (P) {
				CFrustum Clip;
				Clip.CreateFromPortal		(P,vBase,mFullXFORM);
				Portals[I]->dwFrameObject	= oQuery.dwMark;
				Portals[I]->getSector(this)->ll_GetObjects(Clip,vBase,mFullXFORM);
			}
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
		CPortal* P = Portals[i];
		vector<Fvector>& V = P->getPoly();
		Fplane PL; PL.build(V[0],V[1],V[2]);

		Msg("#%d --- verts: %d, front(%d), back(%d), F(%d), B(%d)",
			i,
			V.size(),
			P->Front()->SelfID,
			P->Back()->SelfID,
			P->getSectorFacing(C)->SelfID,
			P->getSectorBack(C)->SelfID
			);
//		if (i==2 || i==3 || i==4) reverse(V.begin(),V.end());
	}
}

void CSector::Load(CStream& fs)
{
	// Assign portal polygons
	DWORD size	= fs.FindChunk(fsP_Portals); R_ASSERT(0==(size&1));
	DWORD count	= size/2;
	while (count) {
		WORD ID		= fs.Rword();
		CPortal* P	= ::Render.getPortal(ID);
		Portals.push_back(P);
		count--;
	}

	// Assign visual
	size = fs.FindChunk(fsP_Root);	R_ASSERT(size==4);
	pRoot = ::Render.getVisual(fs.Rdword());

	// Load occluders
	CStream* O = fs.OpenChunk(fsP_Occluders);
	if (O) {
		Occluders.Load(O);
		O->Close();
	}

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
