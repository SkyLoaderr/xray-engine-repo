// Portal.cpp: implementation of the CPortal class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Portal.h"
#include "..\xrLevel.h"
#include "..\xr_object.h"
#include "..\tempobject.h"
#include "..\fbasicvisual.h"

//#include "std_classes.h"

static u32 COLORS[8] = 
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
//extern u32 num_portals;

void drawPoly(sPoly &P, CPortal* Portal)
{
	Fplane PL;
	PL.build(P[0],P[1],P[2]);
	CSector* S=0;
	if (PL.classify(Device.vCameraPosition)<0)	S=Portal->Back	();
	else										S=Portal->Front	();
	
	Fvector Center,End;
	u32 C		 = COLORS[S->SelfID % 8];
	HW.pDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
	Center.add(P[0],P[1]);
	for (u32 i=2; i<P.size(); i++)
	{
		RCache.dbg_DrawTRI(Fidentity,P[0],P[i-1],P[i],C);
		Center.add(P[i]);
	}
	Center.div	(float(P.size()));
	End.mad		(Center,PL.n,.5f);
	RCache.dbg_DrawLINE(Fidentity,Center,End,0xffffffff);
	HW.pDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);
}


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

	VERIFY(_abs(1-P.n.magnitude())<EPS_S);
}


//
CSector::~CSector()
{
	for (int i=0; i<int(tempObjects.size()); i++) 
		xr_delete(tempObjects[i]);
	tempObjects.clear();
}

//
extern float r_ssaDISCARD;

void CSector::Render_objects	(CFrustum& F)
{
	// Persistant models
	Fvector	Tpos;
	vector<CObject*>::iterator I=Objects.begin(), E=Objects.end();
	for (; I!=E; I++) {
		CObject* O = *I;
		if (O->getVisible()) 
		{
			vis_data&	vis				= O->Visual	()->vis;
			O->clXFORM().transform_tiny	(Tpos, vis.sphere.P);
			if (F.testSphere_dirty(Tpos,vis.sphere.R))	
			{
				RImplementation.set_Object	(O);
				O->OnVisible				();
				RImplementation.set_Object	(0);
			}
		}
	}
}

void CSector::Render_objects_s	(CFrustum& F, Fvector& __P, Fmatrix& __X)
{
	// Render everything
	{
		Fvector	Tpos;
		RImplementation.set_Frustum		(&F);
		RImplementation.add_Geometry	(pRoot);

		// Persistant models
		vector<CObject*>::iterator I=Objects.begin(), E=Objects.end();
		for (; I!=E; I++) {
			CObject* O = *I;
			if (O->getVisible()) 
			{
				vis_data&	vis				= O->Visual	()->vis;
				O->clXFORM().transform_tiny	(Tpos, vis.sphere.P);
				if (F.testSphere_dirty(Tpos,vis.sphere.R))	
				{
					RImplementation.set_Object	(O);
					O->OnVisible				();
					RImplementation.set_Object	(0);
				}
			}
		}
	}

	// Search visible portals and go through them
	CSector*	pLastSector		= (CSector*)RImplementation.getSectorActive();
	for (u32 I=0; I<Portals.size(); I++)
	{
		sPoly	S,D;
		if (Portals[I]->dwFrame != RImplementation.marker) {
			CPortal* PORTAL = Portals[I];
			CSector* pSector;

			if (PORTAL->bDualRender) {
				pSector = PORTAL->getSector			(this);
			} else {
				pSector = PORTAL->getSectorBack		(__P);
				if (pSector==this)			continue;
				if (pSector==pLastSector)	continue;
			}

			// SSA
			Fvector	dir2portal;
			dir2portal.sub		(PORTAL->S.P,__P);
			float R				=	PORTAL->S.R;
			float distSQ		=	dir2portal.square_magnitude();
			float ssa			=	R*R/distSQ;
			dir2portal.div		(_sqrt(distSQ));
			ssa					*=	_abs(PORTAL->P.n.dotproduct(dir2portal));
			if (ssa<r_ssaDISCARD)	continue;

			// Clip by frustum
			vector<Fvector> &	POLY = PORTAL->getPoly();
			S.assign			(&*POLY.begin(),POLY.size()); D.clear();
			sPoly* P			= F.ClipPoly(S,D);
			if (0==P)			continue;

			// Cull by HOM
			if (!RImplementation.occ_visible(*P))	continue;

			// Create _new_ frustum and recurse
			CFrustum			Clip;
			Clip.CreateFromPortal(P,__P,__X);
			PORTAL->dwFrame		= RImplementation.marker;
			PORTAL->bDualRender	= FALSE;
			pSector->Render_objects_s(Clip,__P,__X);
		}
	}
}

void CSector::Render_prepare	(CFrustum &F)
{
	// Render prepare
	// Ётот кусочек переехал временно из ф-ции Render
	// дл€ того чтобы показать ѕрофу что отрисовать тень в 
	// принципе возможно(но не нужно - выгл€дит хреново)
	// св€зано с тем что лайты нужно заполнить чуть раньше
	RImplementation.add_Lights	(Lights);
}

void CSector::Render			(CFrustum &F)
{
	// Render everything
	{
		Fvector	Tpos;
		RImplementation.set_Frustum		(&F);
		RImplementation.add_Glows		(Glows);
		RImplementation.add_Lights		(Lights);
		RImplementation.add_Geometry	(pRoot);

		// R2-lights/spots
#if RENDER==R_R2
		{
			vector<light*>::iterator I=tempLights.begin(), E=tempLights.end();
			for (; I!=E; I++) {
				light* O = *I;
				if (!O->get_active())	continue;
				switch	(O->flags.type)
				{
				case IRender_Light::POINT:
					if (F.testSphere_dirty(O->position,O->range))
						RImplementation.Lights.add_sector_dlight(O);
					break;
				case IRender_Light::SPOT:
					{
						Fvector P;
						P.mad	(O->position,O->direction,O->range/2);
						if (F.testSphere_dirty(P,O->range/2))
							RImplementation.Lights.add_sector_dlight(O);
					}
					break;
				}
			}
		}
#endif

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
				vis_data&	vis				= O->Visual	()->vis;
				O->clXFORM().transform_tiny	(Tpos, vis.sphere.P);
				if (F.testSphere_dirty(Tpos,vis.sphere.R))	
				{
					RImplementation.set_Object	(O);
					O->OnVisible				();
					RImplementation.set_Object	(0);
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
					u32 planes		=	F.getMask();
					vis_data&	vis	=	pV->Visual()->vis;
					if (F.testSAABB(vis.sphere.P,vis.sphere.R,vis.box.min,vis.box.max,planes)!=fcvNone)
						RImplementation.add_Geometry	(pV->Visual());
				}
				else
				{
					if (pV->IsAutomatic())
					{
						tempObjects.erase	(tempObjects.begin()+i);
						xr_delete			(pV);
						i--;
					}
				}
			}
		}
	}

	// Search visible portals and go through them
	CSector*	pLastSector		= (CSector*)RImplementation.getSectorActive();
	for (u32 I=0; I<Portals.size(); I++)
	{
		sPoly	S,D;
		if (Portals[I]->dwFrame != RImplementation.marker) {
			CPortal* PORTAL = Portals[I];
			CSector* pSector;

			if (PORTAL->bDualRender) {
				pSector = PORTAL->getSector			(this);
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
			float ssa			=	R*R/distSQ;
			dir2portal.div		(_sqrt(distSQ));
			ssa					*=	_abs(PORTAL->P.n.dotproduct(dir2portal));
			if (ssa<r_ssaDISCARD)	continue;

			// Clip by frustum
			vector<Fvector> &	POLY = PORTAL->getPoly();
			S.assign			(&*POLY.begin(),POLY.size()); D.clear();
			sPoly* P			= F.ClipPoly(S,D);
			if (0==P)			continue;
			
			// Cull by HOM
			if (!RImplementation.occ_visible(*P))	continue;

			// Create _new_ frustum and recurse
			CFrustum			Clip;
			Clip.CreateFromPortal(P,Device.vCameraPosition,Device.mFullTransform);
			PORTAL->dwFrame		= RImplementation.marker;
			PORTAL->bDualRender	= FALSE;
			pSector->Render		(Clip);
		}
	}
}

static CSector::objQuery	oQuery = { 0xfefefefe, 0, 0, 0 };

void CSector::get_objects	(CFrustum& F, Fvector& vBase, Fmatrix& mFullXFORM, objSET &D, objQualifier* Q, void* P)
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
			O->clCenter	(Pos);
			if (F.testSphere_dirty	(Pos,O->Radius()))
				oQuery.Collector->push_back(O);
		}
	}

	// Search visible portals and go through them
	for (u32 I=0; I<Portals.size(); I++)
	{
		sPoly	S,D;
		if (Portals[I]->dwFrameObject != oQuery.dwMark) {
			vector<Fvector> &POLY = Portals[I]->getPoly();
			S.assign(&*POLY.begin(),POLY.size()); D.clear();
			
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
	Fbox&	B = pRoot->vis.box;
	Fvector C;
	B.getcenter	(C);

	Log("-------------",SelfID);
	for (u32 i=0; i<Portals.size(); i++)
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

void CSector::Load(IReader& fs)
{
	// Assign portal polygons
	u32 size	= fs.find_chunk(fsP_Portals); R_ASSERT(0==(size&1));
	u32 count	= size/2;
	while (count) {
		WORD ID		= fs.r_u16();
		CPortal* P	= (CPortal*)RImplementation.getPortal	(ID);
		Portals.push_back(P);
		count--;
	}

	// Assign visual
	size	= fs.find_chunk(fsP_Root);	R_ASSERT(size==4);
	pRoot	= RImplementation.getVisual(fs.r_u32());

	// Load glows
	size	= fs.find_chunk(fsP_Glows);
	if (size) {
		R_ASSERT	(0==(size&1));
		count		= size/sizeof(WORD);
		Glows.resize(count);
		fs.r		(&*Glows.begin(),size);
	}

	// Load lights
	size	= fs.find_chunk(fsP_Lights);	
	if (size) {
		R_ASSERT		(0==(size&1));
		count			= size/sizeof(WORD);
		Lights.resize	(count);
		fs.r			(&*Lights.begin(),size);
	}
}
