// CRender.cpp: implementation of the CRender class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "xr_ioconsole.h"
#include "render.h"
#include "fbasicvisual.h"
#include "xr_creator.h"
#include "CustomHUD.h"

ENGINE_API	CRender				Render_Implementation;
ENGINE_API	CRender_interface*	Render = &Render_Implementation;

// Implementation
CVisual*	CRender::model_CreatePS		(LPCSTR name, PS::SEmitter* E)	{ return Models.CreatePS(name,E);	}
CVisual*	CRender::model_Create		(LPCSTR name)					{ return Models.Create(name);		}
CVisual*	CRender::model_Create		(CStream* data)					{ return Models.Create(data);		}
void		CRender::model_Delete		(CVisual* &V)					{ Models.Delete(V);					}

int			CRender::getVisualsCount	()					{ return Visuals.size();							}
CPortal*	CRender::getPortal			(int id)			{ VERIFY(id<Portals.size()); return &Portals[id];	}
CSector*	CRender::getSector			(int id)			{ VERIFY(id<Sectors.size()); return Sectors[id];	}
CVisual*	CRender::getVisual			(int id)			{ VERIFY(id<Visuals.size()); return Visuals[id];	}

void		CRender::set_Object			(CObject*		O )	{ L_Shadows.set_object(O);							}
void		CRender::add_Visual			(CVisual*		V )	{ add_leafs_Dynamic(V);								}
void		CRender::add_Lights			(vector<WORD> &	V )	{ L_DB.add_sector_lights(V);						}
void		CRender::add_Glows			(vector<WORD> &	V )	{ Glows.add(V);										}
void		CRender::add_Patch			(Shader* S, Fvector& P1, float s, float a, BOOL bNearer)
{
	vecPatches.push_back(SceneGraph::_PatchItem());
	SceneGraph::_PatchItem& P = vecPatches.back();
	P.S		= S;
	P.P		= P1;
	P.size	= s;
	P.angle = a;
	P.nearer= bNearer;
}


// Misc
Shader*				shDEBUG = 0;
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
	DWORD Camb		= (C*3)/4;
	gm_SetAmbient	(D3DCOLOR_XRGB(Camb,Camb,Camb));
	DWORD Clevel	= (C*4)/3;
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
	ViewBase.CreateFromMatrix(Device.mFullTransform,FRUSTUM_P_LRTB|FRUSTUM_P_FAR);
	View			= 0;

	// Transfer to global space to avoid deep pointer access
	float fFar	=	50.f;
	g_fFarSq	=	fFar; 
	g_fFarSq	*=	g_fFarSq;
	g_fSCREEN	=	float(Device.dwWidth*Device.dwHeight);
	
	// HOM rendering
	HOM.Render	(ViewBase);

	// Build lights visibility & perform basic initialization
	// Lights
	Lights.UnselectAll				();
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
	XRC.box_options	(0);
	XRC.box_query	(rmPortals,Device.vCameraPosition,box_radius);
	for (DWORD K=0; K<XRC.r_count(); K++)
	{
		CPortal*	pPortal	= (CPortal*) rmPortals->get_tris()[XRC.r_begin()[K].id].dummy;
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
void __fastcall normal_L2(FixedMAP<float,CVisual*>::TNode *N)
{
	CVisual *V = N->val;
	V->Render(calcLOD(N->key,V->bv_Radius));
}

extern void __fastcall render_Cached(CList<FCached*>& cache);
void __fastcall mapNormal_Render	(SceneGraph::mapNormalItems& N)
{
	// *** DIRECT ***
	{
		// DIRECT:SORTED
		N.direct.sorted.traverseLR		(normal_L2);
		N.direct.sorted.clear			();
		
		// DIRECT:UNSORTED
		CList<CVisual*>&	L			= N.direct.unsorted;
		CVisual **I=L.begin(), **E = L.end();
		for (; I!=E; I++)
		{
			CVisual *V = *I;
			V->Render	(0);	// zero lod 'cause it is too small onscreen
		}
		L.clear	();
	}

	// *** CACHED ***
	{
		Device.Statistic.RenderDUMP_Cached.Begin();

		// CACHED:SORTED
		CList<FCached*>& CS				= ::Render.vecCached;
		N.cached.sorted.getLR			(CS);
		if (!CS.empty())				render_Cached(CS);
		CS.clear						();
		N.cached.sorted.clear			();

		// CACHED:UNSORTED
		CList<FCached*>& CU				= N.cached.unsorted;
		if (!CU.empty())				render_Cached(CU);
		CU.clear						();

		Device.Statistic.RenderDUMP_Cached.End	();
	}
}

// MATRIX
void __fastcall matrix_L2(SceneGraph::mapMatrixItem::TNode *N)
{
	CVisual *V = N->val.pVisual;
	CHK_DX(HW.pDevice->SetTransform(D3DTS_WORLD,(D3DMATRIX*)N->val.Matrix.d3d()));
	::Render.Lights.Select(N->val.vCenter,V->bv_Radius);
	gm_SetAmbientLevel(N->val.iLighting);
	V->Render(calcLOD(N->key,V->bv_Radius));
}

void __fastcall matrix_L1(SceneGraph::mapMatrix_Node *N)
{
	Device.Shader.set_Shader(N->key);
	N->val.traverseLR		(matrix_L2);
	N->val.clear			();
}

// ALPHA
void __fastcall sorted_L1(SceneGraph::mapSorted_Node *N)
{
	CVisual *V = N->val.pVisual;
	Device.Shader.set_Shader		(V->hShader);
	CHK_DX(HW.pDevice->SetTransform	(D3DTS_WORLD,N->val.Matrix.d3d()));
	::Render.Lights.Select			(N->val.vCenter,V->bv_Radius);
	gm_SetAmbientLevel				(N->val.iLighting);
	V->Render						(calcLOD(N->key,V->bv_Radius));
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
							TL.p.z, TL.p.w, 0xffffffff, 0,1 );		V++;
		V->set			(	cx - size * _sin2,	// sx
							cy - size * _cos2,	// sy
							TL.p.z, TL.p.w, 0xffffffff, 0,0 );		V++;
		V->set			(	cx + size * _sin2,	// sx
							cy + size * _cos2,	// sy
							TL.p.z, TL.p.w, 0xffffffff, 1,1 );		V++;
		V->set			(	cx - size * _sin1,	// sx
							cy - size * _cos1,	// sy
							TL.p.z, TL.p.w, 0xffffffff, 1,0 );		V++;
	}
	groups.push_back	(cur_count);
	vsPatches->Unlock	(vecPatches.size()*4);
	
	// *** Render
	int current=0;
	for (DWORD g=0; g<groups.size(); g++)
	{
		int p_count				= groups[g];
		Device.Shader.set_Shader(vecPatches[current].S);
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

IC	bool	cmp_codes		(SceneGraph::mapNormalCodes::TNode* N1, SceneGraph::mapNormalCodes::TNode* N2)
{	return (N1->val.ssa > N2->val.ssa);		}

IC	bool	cmp_textures	(SceneGraph::mapNormalTextures::TNode* N1, SceneGraph::mapNormalTextures::TNode* N2)
{	return (N1->val.ssa > N2->val.ssa);		}

IC	bool	cmp_matrices	(SceneGraph::mapNormalMatrices::TNode* N1, SceneGraph::mapNormalMatrices::TNode* N2)
{	return (N1->val.ssa > N2->val.ssa);		}

IC	bool	cmp_constants	(SceneGraph::mapNormalConstants::TNode* N1, SceneGraph::mapNormalConstants::TNode* N2)
{	return (N1->val.ssa > N2->val.ssa);		}

void	CRender::Render		()
{
	Device.Statistic.RenderDUMP.Begin();

	Target.Begin	();

	// Environment render
	pCreator->Environment.RenderFirst		();
	
	// NORMAL			*** mostly the main level
	// Perform sorting based on ScreenSpaceArea
	CHK_DX(HW.pDevice->SetTransform(D3DTS_WORLD,precalc_identity.d3d()));

	// Sorting by SSA
	for (DWORD pr=0; pr<4; pr++)
	{
		if (0==mapNormal[pr][0].size())	continue;

		for (DWORD pass_id=0; pass_id<8; pass_id++)	{
			SceneGraph::mapNormalCodes&		codes	= mapNormal	[pr][pass_id];
			if (0==codes.size())	break;
			BOOL sort	= (pass_id==0);
				
			codes.getANY_P		(lstCodes);
			if (sort) std::sort	(lstCodes.begin(), lstCodes.end(), cmp_codes);
			for (DWORD code_id=0; code_id<lstCodes.size(); code_id++)
			{
				SceneGraph::mapNormalCodes::TNode*	Ncode	= lstCodes[code_id];
				SceneGraph::mapNormalTextures&	textures	= Ncode->val;
				Device.Shader.set_Code	(Ncode->key);

				textures.getANY_P	(lstTextures);
				if (sort) std::sort	(lstTextures.begin(),lstTextures.end(), cmp_textures);
				for (DWORD texture_id=0; texture_id<lstTextures.size(); texture_id++)
				{
					SceneGraph::mapNormalTextures::TNode*	Ntexture	= lstTextures[texture_id];
					SceneGraph::mapNormalMatrices& matrices				= Ntexture->val;
					Device.Shader.set_Textures	(Ntexture->key);

					matrices.getANY_P	(lstMatrices);
					if (sort) std::sort	(lstMatrices.begin(),lstMatrices.end(), cmp_matrices);
					for (DWORD matrix_id=0; matrix_id<lstMatrices.size(); matrix_id++) 
					{
						SceneGraph::mapNormalMatrices::TNode*	Nmatrix		= lstMatrices[matrix_id];
						SceneGraph::mapNormalConstants& constants			= Nmatrix->val;
						Device.Shader.set_Matrices	(Nmatrix->key);

						constants.getANY_P	(lstConstants);
						if (sort) std::sort	(lstConstants.begin(),lstConstants.end(), cmp_constants);
						for (DWORD constant_id=0; constant_id<lstConstants.size(); constant_id++)
						{
							SceneGraph::mapNormalConstants::TNode*	Nconstant	= lstConstants[constant_id];
							SceneGraph::mapNormalItems&	items					= Nconstant->val;
							Device.Shader.set_Constants	(Nconstant->key,FALSE);
							mapNormal_Render			(Nconstant->val);
							items.ssa					= 0;
						}
						lstConstants.clear	();
						constants.clear		();
						constants.ssa		= 0;
					}
					lstMatrices.clear	();
					matrices.clear		();
					matrices.ssa		= 0;
				}
				lstTextures.clear	();
				textures.clear		();
				textures.ssa		= 0;
			}
			lstCodes.clear		();
			codes.clear			();
		}

		if (1==pr)			{
			// NORMAL-matrix		*** actors and dyn. objects
			mapMatrix.traverseANY	(matrix_L1);
			mapMatrix.clear			();

			CHK_DX(HW.pDevice->SetTransform(D3DTS_WORLD,precalc_identity.d3d()));
			Wallmarks.Render		();		// Wallmarks has priority as normal geometry

			CHK_DX(HW.pDevice->SetTransform(D3DTS_WORLD,precalc_identity.d3d()));
			Lights_Dynamic.Render	();		// Lights has priority the same as normal geom

			CHK_DX(HW.pDevice->SetTransform(D3DTS_WORLD,precalc_identity.d3d()));
			Details.Render			(Device.vCameraPosition);
		}
	}
	
	// Sorted (back to front)
	mapSorted.traverseRL	(sorted_L1);
	mapSorted.clear			();

	// Glows
	Glows.Render			();

	// HUD
	Device.Statistic.RenderDUMP_HUD.Begin	();
	pCreator->pHUD->Render_Affected			();
	Device.Statistic.RenderDUMP_HUD.End		();

	// Patches
	if (vecPatches.size())  {
		flush_Patches	();
	}

	pCreator->Environment.RenderLast		();
	
	// Postprocess
	Target.End				();
	HOM.Debug				();
	
	// HUD
	Device.Statistic.RenderDUMP_HUD.Begin	();
	pCreator->pHUD->Render_Direct			();
	Device.Statistic.RenderDUMP_HUD.End		();

	Device.Statistic.RenderDUMP.End();
}

// Device events
void CRender::OnDeviceCreate()
{
	REQ_CREATE				();
	Target.OnDeviceCreate	();
	level_Load				();
}
void CRender::OnDeviceDestroy()
{
//	REQ_DESTROY				();
	level_Unload			();
	Target.OnDeviceDestroy	();
}
