// CRender.cpp: implementation of the CRender class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "xr_ioconsole.h"
#include "fstaticrender.h"
#include "xr_creator.h"
#include "CustomHUD.h"
 
// Font out
#include "x_ray.h"
#include "xr_smallfont.h"
#include "xr_mac.h"

ENGINE_API CRender	Render;
 
_FpsController		QualityControl;
static	float		g_fGLOD, g_fFarSq, g_fPOWER;
float				g_fSCREEN;
static	Fmaterial	gm_Data;
static	int			gm_Level	= 0;
static	DWORD		gm_Ambient	= 0;

IC		void		gm_SetLevel		(int iLevel)
{
	if (_abs(gm_Level-s32(iLevel))>2) {
		gm_Level	= iLevel;
		float c		= 0.1f+float(gm_Level)/255.f;
		gm_Data.diffuse.set				(c,c,c,c);
		CHK_DX(HW.pDevice->SetMaterial	(gm_Data.d3d()));
	}
}

IC		void		gm_SetAmbient	(DWORD C)
{
	if (C!=gm_Ambient)	{
		gm_Ambient	= C;
		CHK_DX(HW.pDevice->SetRenderState(D3DRS_AMBIENT, C));
	}
}

IC		void		gm_SetAmbientLevel(DWORD C)
{
	DWORD Camb		= C/2;
	gm_SetAmbient	(D3DCOLOR_XRGB(Camb,Camb,Camb));
	DWORD Clevel	= C/2;
	gm_SetLevel		(Clevel);
}

// Textures
// static Fmatrix		matTrans;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CRender::CRender()
{
	// Q-Control
	QualityControl.fScaleGLOD=0.0f;
}

CRender::~CRender()
{
}

void CRender::Calculate()
{
	float	diff;
 
	Device.Statistic.RenderCALC.Begin();
	diff=Device.Statistic.fFPS-QualityControl.fMinFPS;

	QualityControl.fScaleGLOD     += diff*Device.fTimeDelta*0.1f; //*0.0001f
	if (QualityControl.fScaleGLOD<0.001f) QualityControl.fScaleGLOD=0.001f;
	if (QualityControl.fScaleGLOD>1.2f) QualityControl.fScaleGLOD=1.2f;

	// ******************** Geometry detail
	float	fFar = 50.f;
	fGLOD	= QualityControl.fGeometryLOD*2.f/(fFar*fFar); // *QualityControl.fScaleGLOD
	fPOWER	= (1.f-QualityControl.fScaleGLOD)*40.f + 1.f;

	ViewBase.CreateFromViewMatrix(Device.mFullTransform);
	View = 0;

	// Transfer to global space to avoid deep pointer access
	g_fFarSq	=	fFar; 
	g_fFarSq	*=	g_fFarSq;
	g_fGLOD		=	fGLOD;
	g_fPOWER	=	fPOWER;
	g_fSCREEN	=	float(Device.dwWidth*Device.dwHeight);

	// Sleep
/*
	float time_to_sleep = 3.f*(1.0f/QualityControl.fMaxFPS-1.0f/Device.Statistic.fFPS);
	if (time_to_sleep>0)
	{
		CTimer T; T.Start();
//		while (T.GetAsync()<time_to_sleep) Sleep(0);
	}
*/

	// Build lights visibility & perform basic initialization
	// Lights
	Lights.UnselectAll				();
	Lights.BuildSelection			();
	gm_Data.diffuse.set				(1,1,1,1);
	gm_Data.ambient.set				(1,1,1,1);
	gm_Data.emissive.set			(0,0,0,0);
	gm_Data.specular.set			(1,1,1,1);
	gm_Data.power					= 15.f;
	gm_Level						= 255;
	gm_Ambient						= 0xffffffff;
	CHK_DX(HW.pDevice->SetMaterial	(gm_Data.d3d()));
	gm_SetAmbient					(0);

	// Render current sector and beyond
	if (!vLastCameraPos.similar(Device.vCameraPosition,EPS_S)) 
	{
		CSector* pSector = detectSector(Device.vCameraPosition);
		if (0==pSector) pSector = pLastSector;
		pLastSector = pSector;
		vLastCameraPos.set(Device.vCameraPosition);
	}

	// Check if camera is too near to some portal - if so force DualRender
	Fvector box_radius; box_radius.set(EPS_L*2,EPS_L*2,EPS_L*2);
	XRC.BBoxMode	(0);
	XRC.BBoxCollide	(precalc_identity,rmPortals,precalc_identity,Device.vCameraPosition,box_radius);
	for (DWORD K=0; K<XRC.GetBBoxContactCount(); K++)
	{
		CPortal*	pPortal	= (CPortal*) rmPortals->tris[XRC.BBoxContact[K].id].dummy;
		pPortal->bDualRender = TRUE;
	}

	// Render sector
	if (0!=pLastSector) pLastSector->Render(ViewBase);

	Device.Statistic.RenderCALC.End();
}

IC float calcLOD	(float fDistSq, float R)
{
	float dist	= g_fFarSq - fDistSq + R*R;
	float lod	= QualityControl.fGeometryLOD*dist/g_fFarSq;
	clamp		(lod,0.001f,0.999f);
	return		lod;
}

// NORMAL
void __fastcall normal_L2(FixedMAP<float,FBasicVisual*>::TNode *N)
{
	FBasicVisual *V = N->val;
	V->Render(calcLOD(N->key,V->bv_Radius));
}

extern void __fastcall render_Cached(CList<FCached*>& cache);
void __fastcall normal_L1(SceneGraph::mapNormal_Node *N)
{
	Device.Shader.Set		(N->key);

	// *** DIRECT ***
	{
		// DIRECT:SORTED
		N->val.direct.sorted.traverseLR	(normal_L2);
		N->val.direct.sorted.clear		();
		
		// DIRECT:UNSORTED
		CList<FBasicVisual*>&	L		= N->val.direct.unsorted;
		FBasicVisual **I=L.begin(), **E = L.end();
		for (; I!=E; I++)
		{
			FBasicVisual *V = *I;
			V->Render	(0);	// zero lod 'cause it is too small onscreen
		}
		L.clear	();
	}

	// *** CACHED ***
	{
		Device.Statistic.RenderDUMP_Cached.Begin();

		// CACHED:SORTED
		CList<FCached*>& CS				= ::Render.vecCached;
		N->val.cached.sorted.getLR		(CS);
		if (!CS.empty())				render_Cached(CS);
		CS.clear						();
		N->val.cached.sorted.clear		();

		// CACHED:UNSORTED
		CList<FCached*>& CU				= N->val.cached.unsorted;
		if (!CU.empty())				render_Cached(CU);
		CU.clear						();

		Device.Statistic.RenderDUMP_Cached.End	();
	}
}

// MATRIX
void __fastcall matrix_L2(SceneGraph::mapMatrixItem::TNode *N)
{
	FBasicVisual *V = N->val.pVisual;
	CHK_DX(HW.pDevice->SetTransform(D3DTS_WORLD,(D3DMATRIX*)N->val.Matrix.d3d()));
	::Render.Lights.SelectDynamic(N->val.vCenter,V->bv_Radius);
	gm_SetAmbientLevel(N->val.iLighting);
	V->Render(calcLOD(N->key,V->bv_Radius));
}

void __fastcall matrix_L1(SceneGraph::mapMatrix_Node *N)
{
	Device.Shader.Set(N->key);
	N->val.traverseLR(matrix_L2);
	N->val.clear();
}

// ALPHA
void __fastcall sorted_L1(SceneGraph::mapSorted_Node *N)
{
	FBasicVisual *V = N->val.pVisual;
	Device.Shader.Set(V->hShader);
	CHK_DX(HW.pDevice->SetTransform(D3DTS_WORLD,(D3DMATRIX*)N->val.Matrix.d3d()));
	::Render.Lights.SelectDynamic(N->val.vCenter,V->bv_Radius);
	gm_SetAmbientLevel(N->val.iLighting);
	V->Render(calcLOD(N->key,V->bv_Radius));
}

// LINES
void __fastcall lines_L1(SceneGraph::mapLine_Node *N)
{
	Device.Shader.Set		(N->key);
	SceneGraph::mapLineItem& L = N->val;
	Device.Primitive.Lines_Begin(L.size());
	for (SceneGraph::mapLineItem::iterator I=L.begin(); I!=L.end(); I++)
		Device.Primitive.Lines_Draw(I->P1,I->P2,I->w,I->C);
	Device.Primitive.Lines_End();
	L.clear();
}

void CRender::flush_Static()
{
	CHK_DX(HW.pDevice->SetTransform(D3DTS_WORLD,precalc_identity.d3d()));
	mapNormal[0].traverseANY	(normal_L1);
	mapNormal[0].clear			();
	mapNormal[1].traverseANY	(normal_L1);
	mapNormal[1].clear			();
	mapNormal[2].traverseANY	(normal_L1);
	mapNormal[2].clear			();
	mapNormal[3].traverseANY	(normal_L1);
	mapNormal[3].clear			();
}

void CRender::flush_Models()
{
	mapMatrix.traverseANY	(matrix_L1);
	mapMatrix.clear			();
}

void CRender::flush_Patches()
{
	// *** Fill VB
	DWORD						vOffset;
	FVF::TL*					V = (FVF::TL*)vsPatches->Lock(vecPatches.size()*4,vOffset);
	svector<int,max_patches>	groups;
	Shader*						cur_S=vecPatches[0].S;
	int							cur_count=0;
	for (DWORD i=0; i<vecPatches.size(); i++)
	{
		// sort out redundancy
		SceneGraph::_PatchItem	&P = vecPatches[i];
		if (P.S==cur_S)	cur_count++;
		else {
			groups.push_back(cur_count);
			cur_S			= P.S;
			cur_count		= 1;
		}
		
		// xform
		FVF::TL			TL;
		TL.transform	(P.P,Device.mFullTransform);
		if (P.nearer)	TL.p.z*=0.04f;
		float size		= Device.dwWidth * P.size / TL.p.w;
		
		// Convert to screen coords
		float cx        = Device._x2real(TL.p.x);
		float cy        = Device._y2real(TL.p.y);

		// Rotation
		float			_sin1,_cos1,_sin2,_cos2;
		float			da	= P.angle;
		_sincos			(da,_sin1,_cos1);
		da				+= PI_DIV_2;
		_sincos			(da,_sin2,_cos2);

		V->set			(	cx + size * _sin1,	// sx
							cy + size * _cos1,	// sy
							TL.p.z, TL.p.w, 0, 0,1 );		V++;
		V->set			(	cx - size * _sin2,	// sx
							cy - size * _cos2,	// sy
							TL.p.z, TL.p.w, 0, 0,0 );		V++;
		V->set			(	cx + size * _sin2,	// sx
							cy + size * _cos2,	// sy
							TL.p.z, TL.p.w, 0, 1,1 );		V++;
		V->set			(	cx - size * _sin1,	// sx
							cy - size * _cos1,	// sy
							TL.p.z, TL.p.w, 0, 1,0 );		V++;
	}
	groups.push_back	(cur_count);
	vsPatches->Unlock	(vecPatches.size()*4);
	
	// *** Render
	int current=0;
	for (DWORD g=0; g<groups.size(); g++)
	{
		int p_count				= groups[g];
		Device.Shader.Set		(vecPatches[current].S);
		Device.Primitive.Draw	(vsPatches,4*p_count,2*p_count,vOffset,Device.Streams_QuadIB);
		current	+=	p_count;
		vOffset	+=	4*p_count;
	}

	// *** Cleanup
	vecPatches.clear	();
}

void	CRender::rmNear		()
{
	D3DVIEWPORT8 VP = {0,0,Device.dwWidth,Device.dwHeight,0,0.02f };
	CHK_DX(HW.pDevice->SetViewport(&VP));
}
void	CRender::rmFar		()
{
	D3DVIEWPORT8 VP = {0,0,Device.dwWidth,Device.dwHeight,0.99f,1.f };
	CHK_DX(HW.pDevice->SetViewport(&VP));
}
void	CRender::rmNormal	()
{
	D3DVIEWPORT8 VP = {0,0,Device.dwWidth,Device.dwHeight,0,1.f };
	CHK_DX(HW.pDevice->SetViewport(&VP));
}

IC	bool	cmp_nodes(SceneGraph::mapNormal_Node* N1, SceneGraph::mapNormal_Node* N2)
{
	return (N1->val.ssa > N2->val.ssa);
}
/*
	CHK_DX(HW.pDevice->SetTransform(D3DTS_WORLD,precalc_identity.d3d()));
	Details.Render			(Device.vCameraPosition);
	
	// NORMAL			*** mostly the main level
	// Perform sorting based on "shader" minimal distance
	for (DWORD pr=0; pr<4; pr++)	
	{
		if (0==mapNormal[pr].size())	continue;

		CHK_DX					(HW.pDevice->SetTransform(D3DTS_WORLD,precalc_identity.d3d()));
		Lights.BeginStatic		();
		mapNormal[pr].getANY_P	(vecNormalNodes);
		std::sort				(vecNormalNodes.begin(),vecNormalNodes.end(),cmp_nodes);
		for (DWORD I=0; I<vecNormalNodes.size(); I++)	{
			SceneGraph::mapNormal_Node*	N = vecNormalNodes[I];
			normal_L1				(N);
			N->val.ssa_valid		= FALSE;
		}
		vecNormalNodes.clear	();
		mapNormal[pr].clear		();

		if (1==pr)			{
			Wallmarks.Render		();		// Wallmarks has priority as normal geometry
			Lights_Dynamic.Render	();		// Lights has priority the same as normal geom
		}
		if (2==pr)			{
			// NORMAL-matrix	*** actors and dyn. objects
			mapMatrix.traverseANY	(matrix_L1);
			mapMatrix.clear			();
		}
	}
*/
void	CRender::Render()
{
	Device.Statistic.RenderDUMP.Begin();
	
	CHK_DX(HW.pDevice->SetTransform(D3DTS_WORLD,precalc_identity.d3d()));
	Details.Render			(Device.vCameraPosition);
	
	// NORMAL			*** mostly the main level
	// Perform sorting based on "shader" minimal distance
	CHK_DX(HW.pDevice->SetTransform(D3DTS_WORLD,precalc_identity.d3d()));
	for (DWORD pr=0; pr<4; pr++)	
	{
		if (0==mapNormal[pr].size())	continue;

		mapNormal[pr].getANY_P	(vecNormalNodes);
		std::sort				(vecNormalNodes.begin(),vecNormalNodes.end(),cmp_nodes);
		for (DWORD I=0; I<vecNormalNodes.size(); I++)	{
			SceneGraph::mapNormal_Node*	N = vecNormalNodes[I];
			normal_L1				(N);
			N->val.ssa_valid		= FALSE;
		}
		vecNormalNodes.clear	();
		mapNormal[pr].clear		();

		if (1==pr)			{
			// NORMAL-matrix	*** actors and dyn. objects
			mapMatrix.traverseANY	(matrix_L1);
			mapMatrix.clear			();
			Lights.BeginStatic		();
			CHK_DX(HW.pDevice->SetTransform(D3DTS_WORLD,precalc_identity.d3d()));

			Wallmarks.Render		();		// Wallmarks has priority as normal geometry
			Lights_Dynamic.Render	();		// Lights has priority the same as normal geom
		}
	}
	
	// Sorted (back to front)
	Lights.BeginStatic		();
	mapSorted.traverseRL	(sorted_L1);
	mapSorted.clear			();

	// Glows
	Glows.Render			();

	// HUD
	Device.Statistic.RenderDUMP_HUD.Begin	();
	pCreator->pHUD->Render	();
	Device.Statistic.RenderDUMP_HUD.End		();

	// Lines
	mapLines.traverseANY	(lines_L1);

	// Patches
	if (vecPatches.size())  {
		flush_Patches	();
	}

	Device.Statistic.RenderDUMP.End();
}

// Device events
void CRender::OnDeviceDestroy()
{
	level_Unload();

	// normal map
	for (DWORD pr=0; pr<4; pr++)
	{
		SceneGraph::mapNormal_Node	*cur=mapNormal[pr].begin(), *_end = mapNormal[pr].begin()+mapNormal[pr].allocated();
		for (; cur!=_end; cur++)	
		{
			cur->val.direct.sorted.discard();
			cur->val.cached.sorted.discard();
		}
		mapNormal[pr].discard();
	}
	// matrix map
	{
		SceneGraph::mapMatrix_Node	*cur=mapMatrix.begin(), *_end = mapMatrix.begin()+mapMatrix.allocated();
		for (; cur!=_end; cur++)	cur->val.discard();
		mapMatrix.discard();
	}

	mapSorted.discard();	// sorted map
	mapLines.discard();		// lines map
	vecPatches.clear();		// patches
}

void CRender::OnDeviceCreate()
{
	level_Load	();
}
