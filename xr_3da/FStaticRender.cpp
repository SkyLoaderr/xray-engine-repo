// CRender.cpp: implementation of the CRender class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "xr_ioconsole.h"
#include "render.h"
#include "fstaticrender.h"
#include "fbasicvisual.h"
#include "xr_creator.h"
#include "xr_object.h"
#include "CustomHUD.h"
#include "lighttrack.h"

CRender_interface::CRender_interface()		{};
CRender_interface::~CRender_interface()		{};

ENGINE_API	CRender				Render_Implementation;
ENGINE_API	CRender_interface*	Render = &Render_Implementation;

// Implementation
CVisual*	CRender::model_Create		(LPCSTR name)					{ return Models.Create(name);			}
CVisual*	CRender::model_Create		(LPCSTR name, CStream* data)	{ return Models.Create(name,data);		}
CVisual*	CRender::model_Duplicate	(CVisual* V)					{ return Models.Instance_Duplicate(V);	}
void		CRender::model_Delete		(CVisual* &V)					{ Models.Delete(V);						}

int			CRender::getVisualsCount	()					{ return Visuals.size();								}
CPortal*	CRender::getPortal			(int id)			{ VERIFY(id<int(Portals.size()));	return &Portals[id];}
CSector*	CRender::getSector			(int id)			{ VERIFY(id<int(Sectors.size()));	return Sectors[id];	}
CSector*	CRender::getSectorActive	()					{ return pLastSector;									}
CVisual*	CRender::getVisual			(int id)			{ VERIFY(id<int(Visuals.size()));	return Visuals[id];	}
DWORD		CRender::getFVF				(int id)			{ VERIFY(id<int(FVF.size()));		return FVF[id];		}
IDirect3DVertexBuffer8*	CRender::getVB	(int id)			{ VERIFY(id<int(VB.size()));		return VB[id];		}
IDirect3DIndexBuffer8*	CRender::getIB	(int id)			{ VERIFY(id<int(IB.size()));		return IB[id];		}
CRender_target* CRender::getTarget		()					{ return &Target;										}

void		CRender::L_add				(CLightPPA* L)		{ VERIFY(L); L_Dynamic.Add(L);						}
void		CRender::L_select			(Fvector &pos, float fRadius, vector<xrLIGHT*>& dest)
{	L_DB.Select	(pos,fRadius,dest);		}

void		CRender::flush				()					{ flush_Models();									}

BOOL		CRender::occ_visible		(sPoly& P)			{ return HOM.visible(P);							}
BOOL		CRender::occ_visible		(Fbox& P)			{ return HOM.visible(P);							}

void		CRender::add_Visual			(CVisual*		V )	{ add_leafs_Dynamic(V);								}
void		CRender::add_Geometry		(CVisual*		V )	{ add_Static(V,View->getMask());					}
void		CRender::add_Lights			(vector<WORD> &	V )	{ L_DB.add_sector_lights(V);						}
void		CRender::add_Glows			(vector<WORD> &	V )	{ Glows.add(V);										}
void		CRender::add_Patch			(Shader* S, const Fvector& P1, float s, float a, BOOL bNearer)
{
	vecPatches.push_back(SceneGraph::_PatchItem());
	SceneGraph::_PatchItem& P = vecPatches.back();
	P.S		= S->lod0;
	P.P		= P1;
	P.size	= s;
	P.angle = a;
	P.nearer= bNearer;
}
void		CRender::add_Wallmark		(Shader* S, const Fvector& P, float s, CDB::TRI* T)
{
	Wallmarks.AddWallmark	(T,P,S,s);
}
void		CRender::set_Object			(CObject*		O )	
{ 
	val_pObject				= O;
	L_Shadows.set_object	(O);
	L_Projector.set_object	(O);
	L_DB.Track				(O);
}
CVisual*	CRender::model_CreatePS		(LPCSTR name, PS::SEmitter* E)	
{ 
	PS::SDef_RT*	source	= PSystems.FindPS	(name);
	VERIFY					(source);
	return Models.CreatePS	(source,E);
}


// Misc
Shader*				shDEBUG = 0;
_FpsController		QualityControl;
static	float		g_fGLOD, g_fFarSq, g_fPOWER;
float				g_fSCREEN;
float				g_fLOD,g_fLOD_scale=1.f;
static	Fmaterial	gm_Data;
static	int			gm_Level	= 0;
static	DWORD		gm_Ambient	= 0;
static	BOOL		gm_Nearer	= 0;
static	CObject*	gm_Object	= 0;
static	int			gm_Lcount	= 0;

IC		void		gm_SetLevel			(int iLevel)
{
	if (_abs(s32(gm_Level)-s32(iLevel))>2) {
		gm_Level	= iLevel;
		float c		= 0.1f+float(gm_Level)/255.f;
		gm_Data.diffuse.set				(c,c,c,c);
		CHK_DX(HW.pDevice->SetMaterial	(gm_Data.d3d()));
	}
}

IC		void		gm_SetAmbient		(DWORD C)
{
	if (C!=gm_Ambient)	{
		gm_Ambient	= C;
		CHK_DX(HW.pDevice->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(C,C,C)));
	}
}

IC		void		gm_SetAmbientLevel	(DWORD C)
{
	return;

	DWORD Camb		= (C*3)/4;
	gm_SetAmbient	(D3DCOLOR_XRGB(Camb,Camb,Camb));
	DWORD Clevel	= (C*4)/3;
	gm_SetLevel		(Clevel);
}
IC		void		gm_SetNearer		(BOOL bNearer)
{
	if (bNearer	!= gm_Nearer)
	{
		gm_Nearer	= bNearer;
		if (gm_Nearer)	Render->rmNear	();
		else			Render->rmNormal();
	}
}
IC		void		gm_SetLighting		(CObject* O)
{
	if (O != gm_Object)
	{
		gm_Object			= O;
		if (0==gm_Object)	return;
		CLightTrack& LT		= *O->Lights	();
		
		// shadowing
		if (LT.Shadowed_dwFrame==Device.dwFrame)	{
			gm_SetAmbient		(0);
			Render_Implementation.L_Projector.setup	(LT.Shadowed_Slot);
		} else {
			gm_SetAmbient		(iFloor(LT.ambient)/2);
		}

		// ambience
		// gm_SetAmbient		(iFloor(LT.ambient)/2);

		// set up to 8 lights to device
		int			 max	= _min	(int(LT.lights.size()),8);
		for (int L=0; L<max; L++)
			CHK_DX(HW.pDevice->SetLight(L, LT.lights[L].L.d3d()) );

		// enable them, disable others
		for (L=gm_Lcount; L<max; L++)	{ CHK_DX(HW.pDevice->LightEnable(L,TRUE));	}
		for (L=max;	L<gm_Lcount; L++)	{ CHK_DX(HW.pDevice->LightEnable(L,FALSE)); }
		gm_Lcount			= max;
	}
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CRender::CRender()
{
}

CRender::~CRender()
{
}

extern float		ssaDISCARD,	r_ssaDISCARD;
extern float		ssaDONTSORT,r_ssaDONTSORT;
extern float		ssaLOD_A,	r_ssaLOD_A;
extern float		ssaLOD_B,	r_ssaLOD_B;
extern float		ssaHZBvsTEX,r_ssaHZBvsTEX;

void CRender::Calculate()
{
	Device.Statistic.RenderCALC.Begin();

	// Transfer to global space to avoid deep pointer access
	CRender_target* T				=	getTarget	();
	g_fFarSq						=	75.f;
	g_fFarSq						*=	g_fFarSq;
	g_fSCREEN						=	float(T->get_width()*T->get_height());
	g_fLOD							=	QualityControl.fGeometryLOD*g_fLOD_scale;
	r_ssaDISCARD					=	(ssaDISCARD*ssaDISCARD)/g_fSCREEN;
	r_ssaDONTSORT					=	(ssaDONTSORT*ssaDONTSORT)/g_fSCREEN;
	r_ssaLOD_A						=	(ssaLOD_A*ssaLOD_A)/g_fSCREEN;
	r_ssaLOD_B						=	(ssaLOD_B*ssaLOD_B)/g_fSCREEN;
	r_ssaHZBvsTEX					=	(ssaHZBvsTEX*ssaHZBvsTEX)/g_fSCREEN;
	
	// Frustum & HOM rendering
	ViewBase.CreateFromMatrix		(Device.mFullTransform,FRUSTUM_P_LRTB|FRUSTUM_P_FAR);
	View							= 0;
	HOM.Render						(ViewBase);

	// Build L_DB visibility & perform basic initialization
	L_DB.UnselectAll				();
	gm_Data.diffuse.set				(1,1,1,1);
	gm_Data.ambient.set				(1,1,1,1);
	gm_Data.emissive.set			(0,0,0,0);
	gm_Data.specular.set			(1,1,1,1);
	gm_Data.power					= 15.f;
	gm_Level						= 255;
	gm_Ambient						= 0xffffffff;
	CHK_DX(HW.pDevice->SetMaterial	(gm_Data.d3d()));
	gm_SetAmbient					(0);
	gm_SetNearer					(FALSE);
	gm_SetLighting					(0);
	
	// Detect camera-sector
	if (!vLastCameraPos.similar(Device.vCameraPosition,EPS_S)) 
	{
		CSector* pSector = detectSector(Device.vCameraPosition);
		if (0==pSector) pSector = pLastSector;
		pLastSector = pSector;
		vLastCameraPos.set(Device.vCameraPosition);
	}

	// Check if camera is too near to some portal - if so force DualRender
	if (rmPortals) 
	{
		Fvector box_radius; box_radius.set(EPS_L*2,EPS_L*2,EPS_L*2);
		XRC.box_options	(0);
		XRC.box_query	(rmPortals,Device.vCameraPosition,box_radius);
		for (int K=0; K<XRC.r_count(); K++)
		{
			CPortal*	pPortal	= (CPortal*) rmPortals->get_tris()[XRC.r_begin()[K].id].dummy;
			pPortal->bDualRender = TRUE;
		}
	}

	// Calculate sector(s) and their objects
	calc_DetailTexturing					();
	set_Object								(0);
	if (0!=pLastSector) pLastSector->Render	(ViewBase);
	pCreator->pHUD->Render_Calcualte		();	
	L_Shadows.calculate						();
	L_Projector.calculate					();
	
	// End calc
	Device.Statistic.RenderCALC.End	();
}

IC float calcLOD	(float fDistSq, float R)
{
	float dist	= g_fFarSq - fDistSq + R*R;
	float lod	= g_fLOD*dist/g_fFarSq;
	clamp		(lod,0.001f,0.999f);
	return		lod;
}

// NORMAL
void __fastcall normal_L2(FixedMAP<float,CVisual*>::TNode *N)
{
	CVisual *V = N->val;
	V->Render(calcLOD(N->key,V->bv_Radius));
}

extern void __fastcall render_Cached(vector<FCached*>& cache);
void __fastcall mapNormal_Render	(SceneGraph::mapNormalItems& N)
{
	// *** DIRECT ***
	{
		// DIRECT:SORTED
		N.direct.sorted.traverseLR		(normal_L2);
		N.direct.sorted.clear			();
		
		// DIRECT:UNSORTED
		vector<CVisual*>&	L			= N.direct.unsorted;
		CVisual **I=&*L.begin(), **E = &*L.end();
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
		vector<FCached*>& CS			= ::Render_Implementation.vecCached;
		N.cached.sorted.getLR			(CS);
		if (!CS.empty())				render_Cached(CS);
		CS.clear						();
		N.cached.sorted.clear			();

		// CACHED:UNSORTED
		vector<FCached*>& CU			= N.cached.unsorted;
		if (!CU.empty())				render_Cached(CU);
		CU.clear						();

		Device.Statistic.RenderDUMP_Cached.End	();
	}
}

// MATRIX
void __fastcall matrix_L2	(SceneGraph::mapMatrixItem::TNode *N)
{
	CVisual *V				= N->val.pVisual;
	Device.set_xform_world	(N->val.Matrix);
	gm_SetLighting			(N->val.pObject);
	V->Render				(calcLOD(N->key,V->bv_Radius));
}

void __fastcall matrix_L1	(SceneGraph::mapMatrix_Node *N)
{
	Device.Shader.set_Element	(N->key);
	N->val.traverseLR			(matrix_L2);
	N->val.clear				();
}

// ALPHA
void __fastcall sorted_L1	(SceneGraph::mapSorted_Node *N)
{
	CVisual *V = N->val.pVisual;
	Device.Shader.set_Shader(V->hShader);
	Device.set_xform_world	(N->val.Matrix);
	gm_SetLighting			(N->val.pObject);
	V->Render				(calcLOD(N->key,V->bv_Radius));
}

void CRender::flush_Models	()
{
	mapMatrix.traverseANY	(matrix_L1);
	mapMatrix.clear			();
}

void CRender::flush_Patches	()
{
	// *** Fill VB
	DWORD						vOffset;
	FVF::TL*					V		= (FVF::TL*)Device.Streams.Vertex.Lock	(vecPatches.size()*4,vsPatches->dwStride, vOffset);
	svector<int,max_patches>	groups;
	ShaderElement*				cur_S=vecPatches[0].S;
	int							cur_count=0;
	float						scale	= float	(getTarget()->get_width());
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
		float size		= scale * P.size / TL.p.w;
		
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
	groups.push_back				(cur_count);
	Device.Streams.Vertex.Unlock	(vecPatches.size()*4,vsPatches->dwStride);
	
	// *** Render
	int current=0;
	for (DWORD g=0; g<groups.size(); g++)
	{
		int p_count						= groups[g];
		Device.Shader.set_Element		(vecPatches[current].S);

		Device.Primitive.setVertices	(vsPatches->dwHandle,vsPatches->dwStride,Device.Streams.Vertex.Buffer());
		Device.Primitive.setIndices		(vOffset,Device.Streams.QuadIB);;
		Device.Primitive.Render			(D3DPT_TRIANGLELIST,0,4*p_count,0,2*p_count);
		current	+=	p_count;
		vOffset	+=	4*p_count;
	}

	// *** Cleanup
	vecPatches.clear	();
}

void	CRender::rmNear		()
{
	CRender_target* T	=	getTarget	();
	D3DVIEWPORT8 VP		=	{0,0,T->get_width(),T->get_height(),0,0.02f };
	CHK_DX				(HW.pDevice->SetViewport(&VP));
}
void	CRender::rmFar		()
{
	CRender_target* T	=	getTarget	();
	D3DVIEWPORT8 VP		=	{0,0,T->get_width(),T->get_height(),0.99999f,1.f };
	CHK_DX				(HW.pDevice->SetViewport(&VP));
}
void	CRender::rmNormal	()
{
	CRender_target* T	=	getTarget	();
	D3DVIEWPORT8 VP		= {0,0,T->get_width(),T->get_height(),0,1.f };
	CHK_DX				(HW.pDevice->SetViewport(&VP));
}

IC	bool	cmp_codes			(SceneGraph::mapNormalCodes::TNode* N1, SceneGraph::mapNormalCodes::TNode* N2)
{	return (N1->val.ssa > N2->val.ssa);		}

IC	bool	cmp_matrices		(SceneGraph::mapNormalMatrices::TNode* N1, SceneGraph::mapNormalMatrices::TNode* N2)
{	return (N1->val.ssa > N2->val.ssa);		}

IC	bool	cmp_constants		(SceneGraph::mapNormalConstants::TNode* N1, SceneGraph::mapNormalConstants::TNode* N2)
{	return (N1->val.ssa > N2->val.ssa);		}

IC	bool	cmp_textures_lex2	(SceneGraph::mapNormalTextures::TNode* N1, SceneGraph::mapNormalTextures::TNode* N2)
{	
	STextureList*	t1			= N1->key;
	STextureList*	t2			= N2->key;
	if ((*t1)[0] < (*t2)[0])	return true;
	if ((*t1)[0] > (*t2)[0])	return false;
	if ((*t1)[1] < (*t2)[1])	return true;
	else						return false;
}
IC	bool	cmp_textures_lex3	(SceneGraph::mapNormalTextures::TNode* N1, SceneGraph::mapNormalTextures::TNode* N2)
{	
	STextureList*	t1			= N1->key;
	STextureList*	t2			= N2->key;
	if ((*t1)[0] < (*t2)[0])	return true;
	if ((*t1)[0] > (*t2)[0])	return false;
	if ((*t1)[1] < (*t2)[1])	return true;
	if ((*t1)[1] > (*t2)[1])	return false;
	if ((*t1)[2] < (*t2)[2])	return true;
	else						return false;
}
IC	bool	cmp_textures_lexN	(SceneGraph::mapNormalTextures::TNode* N1, SceneGraph::mapNormalTextures::TNode* N2)
{	
	STextureList*	t1			= N1->key;
	STextureList*	t2			= N2->key;
	return lexicographical_compare(t1->begin(),t1->end(),t2->begin(),t2->end());
}
IC	bool	cmp_textures_ssa	(SceneGraph::mapNormalTextures::TNode* N1, SceneGraph::mapNormalTextures::TNode* N2)
{	
	return (N1->val.ssa > N2->val.ssa);		
}

void		sort_tlist			
	(
	vector<SceneGraph::mapNormalTextures::TNode*>& lst, 
	vector<SceneGraph::mapNormalTextures::TNode*>& temp, 
	SceneGraph::mapNormalTextures& textures, 
	BOOL	bSSA
	)
{
	int amount			= textures.begin()->key->size();
	if (bSSA)	
	{
		if (amount<=1)
		{
			// Just sort by SSA
			textures.getANY_P			(lst);
			std::sort					(lst.begin(), lst.end(), cmp_textures_ssa);
		} 
		else 
		{
			// Split into 2 parts
			SceneGraph::mapNormalTextures::TNode* _it	= textures.begin	();
			SceneGraph::mapNormalTextures::TNode* _end	= textures.end		();
			for (; _it!=_end; _it++)	{
				if (_it->val.ssa > r_ssaHZBvsTEX)	lst.push_back	(_it);
				else								temp.push_back	(_it);
			}

			// 1st - part - SSA, 2nd - lexicographically
			std::sort					(lst.begin(),	lst.end(),	cmp_textures_ssa);
			if (2==amount)				std::sort	(temp.begin(),	temp.end(),	cmp_textures_lex2);
			else if (3==amount)			std::sort	(temp.begin(),	temp.end(),	cmp_textures_lex3);
			else						std::sort	(temp.begin(),	temp.end(),	cmp_textures_lexN);

			// merge lists
			lst.insert					(lst.end(),temp.begin(),temp.end());
		}
	}
	else 
	{
		textures.getANY_P			(lst);
		if (2==amount)				std::sort	(lst.begin(),	lst.end(),	cmp_textures_lex2);
		else if (3==amount)			std::sort	(lst.begin(),	lst.end(),	cmp_textures_lex3);
		else						std::sort	(lst.begin(),	lst.end(),	cmp_textures_lexN);
	}
}

void	CRender::Render		()
{
	Device.Statistic.RenderDUMP.Begin();

	// Target.set_gray					(.5f+sinf(Device.fTimeGlobal)/2.f);
	Target.Begin					();

	if (psDeviceFlags&rsWarmHZB)	HOM.Render_ZB	();

	// HUD render
	{
		extern float psHUD_FOV;
		// Change projection
		Fmatrix Pold				= Device.mProject;
		Fmatrix FTold				= Device.mFullTransform;
		Device.mProject.build_projection(
			deg2rad(psHUD_FOV*Device.fFOV*Device.fASPECT), 
			Device.fASPECT, VIEWPORT_NEAR, 
			pCreator->Environment.Current.Far);
		Device.mFullTransform.mul	(Device.mProject, Device.mView);
		Device.set_xform_project	(Device.mProject);

		// Rendering
		rmNear						();
		mapHUD.traverseLR			(sorted_L1);
		mapHUD.clear				();
		rmNormal					();

		// Restore projection
		Device.mProject				= Pold;
		Device.mFullTransform		= FTold;
		Device.set_xform_project	(Device.mProject);
	}

	// Environment render
	
	// NORMAL			*** mostly the main level
	// Perform sorting based on ScreenSpaceArea
	Device.set_xform_world			(Fidentity);

	// Sorting by SSA and changes minimizations
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

				sort_tlist				(lstTextures, lstTexturesTemp, textures, sort); 
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
				lstTextures.clear		();
				lstTexturesTemp.clear	();
				textures.clear			();
				textures.ssa			= 0;
			}
			lstCodes.clear		();
			codes.clear			();
		}

		if (1==pr)			{
			Device.set_xform_world	(Fidentity);
			Details.Render			(Device.vCameraPosition);

			pCreator->Environment.RenderFirst	();

			// NORMAL-matrix		*** actors and dyn. objects
			mapMatrix.traverseANY	(matrix_L1);
			mapMatrix.clear			();

			Device.set_xform_world	(Fidentity);
			Wallmarks.Render		();		// Wallmarks has priority as normal geometry

			Device.set_xform_world	(Fidentity);
			L_Dynamic.Render		();		// L_DB has priority the same as normal geom

			Device.set_xform_world	(Fidentity);
			L_Shadows.render		();
		}
	}

	// LODs
	flush_LODs				();
	
	// Sorted (back to front)
	mapSorted.traverseRL	(sorted_L1);
	mapSorted.clear			();

	// Glows
	Glows.Render			();

	// HUD
	Device.Statistic.RenderDUMP_HUD.Begin	();
	pCreator->pHUD->Render_Affected			();
	Device.Statistic.RenderDUMP_HUD.End		();
	HOM.Debug				();

	// Patches
	if (vecPatches.size())  {
		flush_Patches	();
	}

	pCreator->Environment.RenderLast		();
	// L_Projector.render					();
	
	// Postprocess
	Target.End				();
	// HOM.Debug			();
	L_Projector.finalize	();
	
	// HUD
	Device.Statistic.RenderDUMP_HUD.Begin	();
	pCreator->pHUD->Render_Direct			();
	Device.Statistic.RenderDUMP_HUD.End		();

	Device.Statistic.RenderDUMP.End();
}

// Device events
void CRender::OnDeviceCreate	()
{
	REQ_CREATE					();
	Target.OnDeviceCreate		();
	L_Shadows.OnDeviceCreate	();
	L_Projector.OnDeviceCreate	();

	PSystems.xrStartUp			();
	PSystems.OnDeviceCreate		();
	level_Load					();
	L_Dynamic.Initialize		();
	Glows.OnDeviceCreate		();
	Wallmarks.OnDeviceCreate	();

	gm_Nearer					= FALSE;
	rmNormal					();

	matDetailTexturing			= Device.Shader._CreateMatrix("$user$detail");
	matFogPass					= Device.Shader._CreateMatrix("$user$fog");
}

void CRender::OnDeviceDestroy	()
{
	Device.Shader._DeleteMatrix	(matFogPass);
	Device.Shader._DeleteMatrix	(matDetailTexturing);

	Wallmarks.OnDeviceDestroy	();
	Glows.OnDeviceDestroy		();
	L_Dynamic.Destroy			();
	level_Unload				();
	PSystems.OnDeviceDestroy	();
	PSystems.xrShutDown			();

	L_Projector.OnDeviceDestroy	();
	L_Shadows.OnDeviceDestroy	();
	Target.OnDeviceDestroy		();

	Models.Destroy				();
}
