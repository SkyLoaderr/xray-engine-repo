// CRender.cpp: implementation of the CRender class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "..\igame_persistent.h"
#include "..\environment.h"
#include "..\fbasicvisual.h"
#include "..\fcached.h"
#include "..\CustomHUD.h"
#include "..\xr_object.h"
#include "lighttrack.h"

using	namespace SceneGraph;

CRender													RImplementation;

//////////////////////////////////////////////////////////////////////////
void					CRender::create					()
{
	::Device.Resources->SetHLSL_path("R1\\");
	Models						= xr_new<CModelPool>			();
	L_Dynamic					= xr_new<CLightPPA_Manager>		();

	PSLibrary.OnCreate			();
	PSLibrary.OnDeviceCreate	();
}
void					CRender::destroy				()
{
	xr_delete					(L_Dynamic);
	xr_delete					(Models);

	PSLibrary.OnDeviceDestroy	();
	PSLibrary.OnDestroy			();
}

// Implementation
IRender_ObjectSpecific*	CRender::ros_create				(IRenderable* parent)					{ return xr_new<CLightTrack>();			}
void					CRender::ros_destroy			(IRender_ObjectSpecific* &p)			{ xr_delete(p);							}
IRender_Visual*			CRender::model_Create			(LPCSTR name)							{ return Models->Create(name);			}
IRender_Visual*			CRender::model_Create			(LPCSTR name, IReader* data)			{ return Models->Create(name,data);		}
IRender_Visual*			CRender::model_Duplicate		(IRender_Visual* V)						{ return Models->Instance_Duplicate(V);	}
void					CRender::model_Delete			(IRender_Visual* &V, BOOL bDiscard)		{ Models->Delete(V,bDiscard);			}
IRender_DetailModel*	CRender::model_CreateDM			(IReader*	F)
{
	CDetail*	D		= xr_new<CDetail> ();
	D->Load				(F);
	return D;
}
void					CRender::model_Delete			(IRender_DetailModel* & F)
{
	if (F)
	{
		CDetail*	D	= (CDetail*)F;
		D->Unload		();
		xr_delete		(D);
		F				= NULL;
	}
}
IRender_Visual*			CRender::model_CreatePS			(LPCSTR name, PS::SEmitter* E)	
{ 
	PS::SDef*	source		= PSLibrary.FindPS	(name);
	VERIFY					(source);
	return Models->CreatePS	(source,E);
}
IRender_Visual*			CRender::model_CreatePE			(LPCSTR name)	
{ 
	PS::CPEDef*	SE		= PSLibrary.FindPED	(name);		VERIFY(SE);
	return				Models->CreatePE	(SE);
}
IRender_Visual*			CRender::model_CreatePG			(LPCSTR name)	
{ 
	PS::CPGDef*	SG		= PSLibrary.FindPGD	(name);		VERIFY(SG);
	return				Models->CreatePG	(SG);
}

IRender_Visual*			CRender::model_CreateParticles	(LPCSTR name)	
{ 
	PS::CPEDef*	SE		= PSLibrary.FindPED	(name);
	if (SE) return		Models->CreatePE	(SE);
	else{
		PS::CPGDef*	SG	= PSLibrary.FindPGD	(name);		R_ASSERT(SG);
		return			Models->CreatePG	(SG);
	}
}

int						CRender::getVisualsCount		()					{ return Visuals.size();								}
IRender_Portal*			CRender::getPortal				(int id)			{ VERIFY(id<int(Portals.size()));	return Portals[id];	}
IRender_Sector*			CRender::getSector				(int id)			{ VERIFY(id<int(Sectors.size()));	return Sectors[id];	}
IRender_Sector*			CRender::getSectorActive		()					{ return pLastSector;									}
IRender_Visual*			CRender::getVisual				(int id)			{ VERIFY(id<int(Visuals.size()));	return Visuals[id];	}
D3DVERTEXELEMENT9*		CRender::getVB_Format			(int id)			{ VERIFY(id<int(DCL.size()));		return DCL[id].begin();	}
IDirect3DVertexBuffer9*	CRender::getVB					(int id)			{ VERIFY(id<int(VB.size()));		return VB[id];		}
IDirect3DIndexBuffer9*	CRender::getIB					(int id)			{ VERIFY(id<int(IB.size()));		return IB[id];		}
IRender_Target*			CRender::getTarget				()					{ return Target;										}

IRender_Light*			CRender::light_create			()					{ return L_Dynamic->Create();							}
void					CRender::light_destroy			(IRender_Light* &L)	{ if (L) { L_Dynamic->Destroy((CLightPPA*)L); L=0; }		}

void					CRender::flush					()					{ flush_Models();									}

BOOL					CRender::occ_visible			(vis_data& P)		{ return HOM.visible(P);							}
BOOL					CRender::occ_visible			(sPoly& P)			{ return HOM.visible(P);							}
BOOL					CRender::occ_visible			(Fbox& P)			{ return HOM.visible(P);							}

void					CRender::add_Visual				(IRender_Visual*		V )	{ add_leafs_Dynamic(V);								}
void					CRender::add_Geometry			(IRender_Visual*		V )	{ add_Static(V,View->getMask());					}
void					CRender::add_Patch				(ref_shader& S, const Fvector& P1, float s, float a, BOOL bNearer)
{
	vecPatches.push_back(_PatchItem());
	_PatchItem& P = vecPatches.back();
	P.S		= S->E[0]._get();
	P.P		= P1;
	P.size	= s;
	P.angle = a;
	P.nearer= bNearer;
}
void		CRender::add_Wallmark		(ref_shader& S, const Fvector& P, float s, CDB::TRI* T)
{
	Wallmarks->AddWallmark	(T,P,&*S,s);
}
void		CRender::set_Object			(IRenderable*		O )	
{
	val_pObject				= O;		// NULL is OK, trust me :)
	L_Shadows->set_object	(O);
	L_Projector->set_object	(O);
	L_DB->Track				(O);
	if (O)					VERIFY	(O->renderable.ROS);
}

// Misc
_FpsController			QualityControl;
static	float			g_fGLOD, g_fFarSq, g_fPOWER;
float					g_fSCREEN;
float					g_fLOD,g_fLOD_scale=1.f;
static	Fmaterial		gm_Data;
static	int				gm_Level	= 0;
static	u32				gm_Ambient	= 0;
static	BOOL			gm_Nearer	= 0;
static	IRenderable*	gm_Object	= 0;
static	int				gm_Lcount	= 0;

IC		void		gm_SetLevel			(int iLevel)
{
	if (_abs(s32(gm_Level)-s32(iLevel))>2) {
		gm_Level	= iLevel;
		float c		= 0.1f+float(gm_Level)/255.f;
		gm_Data.diffuse.set				(c,c,c,c);
		CHK_DX(HW.pDevice->SetMaterial	((D3DMATERIAL9*)&gm_Data));
	}
}

IC		void		gm_SetAmbient		(u32 C)
{
	if (C!=gm_Ambient)	{
		gm_Ambient	= C;
		CHK_DX(HW.pDevice->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(C,C,C)));
	}
}

IC		void		gm_SetAmbientLevel	(u32 C)
{
	return;

	u32 Camb		= (C*3)/4;
	gm_SetAmbient	(D3DCOLOR_XRGB(Camb,Camb,Camb));
	u32 Clevel		= (C*4)/3;
	gm_SetLevel		(Clevel);
}
IC		void		gm_SetNearer		(BOOL bNearer)
{
	if (bNearer	!= gm_Nearer)
	{
		gm_Nearer	= bNearer;
		if (gm_Nearer)	RImplementation.rmNear	();
		else			RImplementation.rmNormal();
	}
}
IC		void		gm_SetLighting		(IRenderable* O)
{
	if (O != gm_Object)
	{
		gm_Object			= O;
		if (0==gm_Object)	return;
		VERIFY				(O->renderable.ROS);
		CLightTrack& LT		= *((CLightTrack*)O->renderable.ROS);

		// shadowing
		if ((LT.Shadowed_dwFrame==Device.dwFrame) && O->renderable_ShadowReceive())	
		{
			gm_SetAmbient		(0);
			RImplementation.L_Projector->setup	(LT.Shadowed_Slot);
		} else {
			gm_SetAmbient		(iFloor(LT.ambient)/2);
		}

		// ambience
		// gm_SetAmbient		(iFloor(LT.ambient)/2);

		// set up to 8 lights to device
		int			 max	= _min	(int(LT.lights.size()),8);
		for (int L=0; L<max; L++)
			CHK_DX(HW.pDevice->SetLight(L, (D3DLIGHT9*)&LT.lights[L].L) );

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

extern float		r_ssaDISCARD;
extern float		r_ssaDONTSORT;
extern float		r_ssaLOD_A;
extern float		r_ssaLOD_B;
extern float		r_ssaHZBvsTEX;

IC bool				pred_sp_sort	(ISpatial* _1, ISpatial* _2)
{
	float	d1		= _1->spatial.center.distance_to_sqr(Device.vCameraPosition);
	float	d2		= _2->spatial.center.distance_to_sqr(Device.vCameraPosition);
	return	d1<d2;
}

void CRender::Calculate()
{
	Device.Statistic.RenderCALC.Begin();
	/*
	float	aX		= ::Random.randF(PI_MUL_2);
	float	aY		= ::Random.randF(PI_MUL_2);
	float	aZ		= ::Random.randF(PI_MUL_2);

	Fmatrix	mX,mY,mZ,temp,res1,res2;
	mX.rotateX		(aX);
	mY.rotateY		(aY);
	mZ.rotateZ		(aZ);

	temp.mul		(mX,mZ);
	res1.mul		(mY,temp);

	res2.setXYZ		(-aX,-aY,-aZ);
	*/

	// Transfer to global space to avoid deep pointer access
	IRender_Target* T				=	getTarget	();
	g_fFarSq						=	75.f;
	g_fFarSq						*=	g_fFarSq;
	g_fSCREEN						=	float(T->get_width()*T->get_height());
	g_fLOD							=	QualityControl.fGeometryLOD*g_fLOD_scale;
	r_ssaDISCARD					=	_sqr(ps_r1_ssaDISCARD)/g_fSCREEN;
	r_ssaDONTSORT					=	_sqr(ps_r1_ssaDONTSORT)/g_fSCREEN;
	r_ssaLOD_A						=	_sqr(ps_r1_ssaLOD_A)/g_fSCREEN;
	r_ssaLOD_B						=	_sqr(ps_r1_ssaLOD_B)/g_fSCREEN;
	r_ssaHZBvsTEX					=	_sqr(ps_r1_ssaHZBvsTEX)/g_fSCREEN;

	// Frustum & HOM rendering
	ViewBase.CreateFromMatrix		(Device.mFullTransform,FRUSTUM_P_LRTB|FRUSTUM_P_FAR);
	View							= 0;
	HOM.Enable						();
	HOM.Render						(ViewBase);

	// Build L_DB visibility & perform basic initialization
	gm_Data.diffuse.set				(1,1,1,1);
	gm_Data.ambient.set				(1,1,1,1);
	gm_Data.emissive.set			(0,0,0,0);
	gm_Data.specular.set			(1,1,1,1);
	gm_Data.power					= 15.f;
	gm_Level						= 255;
	gm_Ambient						= 0xffffffff;
	CHK_DX(HW.pDevice->SetMaterial	((D3DMATERIAL9*)&gm_Data));
	gm_SetAmbient					(0);
	gm_SetNearer					(FALSE);
	gm_SetLighting					(0);

	// Detect camera-sector
	if (!vLastCameraPos.similar(Device.vCameraPosition,EPS_S)) 
	{
		CSector* pSector		= (CSector*)detectSector(Device.vCameraPosition);
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

	// Main process
	marker	++;
	if (pLastSector)
	{
		// Traverse sector/portal structure
		PortalTraverser.traverse	
			(
			pLastSector,
			ViewBase,
			Device.vCameraPosition,
			Device.mFullTransform,
			CPortalTraverser::VQ_HOM + CPortalTraverser::VQ_SSA
			);

		// Determine visibility for static geometry hierrarhy
		for (u32 s_it=0; s_it<PortalTraverser.r_sectors.size(); s_it++)
		{
			CSector*	sector		= (CSector*)PortalTraverser.r_sectors[s_it];
			IRender_Visual*	root	= sector->root();
			for (u32 v_it=0; v_it<sector->r_frustums.size(); v_it++)
			{
				set_Frustum			(&(sector->r_frustums[v_it]));
				add_Geometry		(root);
			}
		}

		// Traverse object database
		g_SpatialSpace.q_frustum
			(
			ISpatial_DB::O_ORDERED,
			STYPE_RENDERABLE,
			ViewBase
			);

		// Exact sorting order (front-to-back)
		lstRenderables.swap	(g_SpatialSpace.q_result);
		std::sort			(lstRenderables.begin(),lstRenderables.end(),pred_sp_sort);

		// Determine visibility for dynamic part of scene
		set_Object							(0);
		g_pGameLevel->pHUD->Render_First	( );
		for (u32 o_it=0; o_it<lstRenderables.size(); o_it++)
		{
			ISpatial*	spatial		= lstRenderables[o_it];
			CSector*	sector		= (CSector*)spatial->spatial.sector;
			if	(0==sector)										continue;	// disassociated from S/P structure
			if	(PortalTraverser.i_marker != sector->r_marker)	continue;	// inactive (untouched) sector
			for (u32 v_it=0; v_it<sector->r_frustums.size(); v_it++)
			{
				CFrustum&	view	= sector->r_frustums[v_it];
				if (!view.testSphere_dirty(spatial->spatial.center,spatial->spatial.radius))	continue;

				// renderable
				IRenderable*	renderable		= dynamic_cast<IRenderable*>(spatial);
				if (0==renderable)	
				{
					// It may be an glow
					CGlow*		glow				= dynamic_cast<CGlow*>(spatial);
					VERIFY							(glow);
					L_Glows->add					(glow);
				} else {
					// Occlusion
					vis_data&		v_orig			= renderable->renderable.visual->vis;
					vis_data		v_copy			= v_orig;
					v_copy.box.xform				(renderable->renderable.xform);
					BOOL			bVisible		= HOM.visible(v_copy);
					v_orig.frame					= v_copy.frame;
					v_orig.hom_frame				= v_copy.hom_frame;
					v_orig.hom_tested				= v_copy.hom_tested;
					if (!bVisible)					continue;

					// Rendering
					set_Object						(renderable);
					renderable->renderable_Render	();
					set_Object						(0);	//? is it needed at all
				}
				break;	// exit loop on frustums
			}
		}
		g_pGameLevel->pHUD->Render_Last						();	

		// Calculate miscelaneous stuff
		calc_DetailTexturing								();
		L_Shadows->calculate								();
		L_Projector->calculate								();
	}
	else
	{
		set_Object											(0);
		g_pGameLevel->pHUD->Render_First					();	
		g_pGameLevel->pHUD->Render_Last						();	
	}

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
void __fastcall normal_L2(FixedMAP<float,IRender_Visual*>::TNode *N)
{
	IRender_Visual *V = N->val;
	V->Render(calcLOD(N->key,V->vis.sphere.R));
}

extern void __fastcall render_Cached(xr_vector<FCached*>& cache);
void __fastcall mapNormal_Render	(mapNormalItems& N)
{
	// *** DIRECT ***
	{
		// DIRECT:SORTED
		N.sorted.traverseLR		(normal_L2);
		N.sorted.clear			();

		// DIRECT:UNSORTED
		xr_vector<IRender_Visual*>&	L			= N.unsorted;
		IRender_Visual **I=&*L.begin(), **E = &*L.end();
		for (; I!=E; I++)
		{
			IRender_Visual *V = *I;
			V->Render	(0);	// zero lod 'cause it is too small onscreen
		}
		L.clear	();
	}
}

// MATRIX
void __fastcall matrix_L2	(mapMatrixItem::TNode *N)
{
	IRender_Visual *V		= N->val.pVisual;
	RCache.set_xform_world	(N->val.Matrix);
	gm_SetLighting			(N->val.pObject);
	V->Render				(calcLOD(N->key,V->vis.sphere.R));
}

void __fastcall matrix_L1	(mapMatrix_Node *N)
{
	RCache.set_Element		(N->key);
	N->val.traverseLR		(matrix_L2);
	N->val.clear			();
}

// ALPHA
void __fastcall sorted_L1	(mapSorted_Node *N)
{
	if (N->val.pObject)		VERIFY	(N->val.pObject->renderable.ROS);
	IRender_Visual *V	=	N->val.pVisual;
	RCache.set_Shader		(V->hShader);
	RCache.set_xform_world	(N->val.Matrix);
	gm_SetLighting			(N->val.pObject);
	V->Render				(calcLOD(N->key,V->vis.sphere.R));
}

void CRender::flush_Models	()
{
	mapMatrix.traverseANY	(matrix_L1);
	mapMatrix.clear			();
}

void CRender::flush_Patches	()
{
	// *** Fill VB
	u32							vOffset;
	FVF::TL*					V		= (FVF::TL*)RCache.Vertex.Lock	(vecPatches.size()*4,hGeomPatches->vb_stride, vOffset);
	svector<int,max_patches>	groups;
	ShaderElement*				cur_S=vecPatches[0].S;
	int							cur_count=0;
	float						w_2		= float	(getTarget()->get_width())	/ 2;
	float						h_2		= float	(getTarget()->get_height()) / 2;
	float						scale	= float	(getTarget()->get_width());
	for (u32 i=0; i<vecPatches.size(); i++)
	{
		// sort out redundancy
		_PatchItem	&P = vecPatches[i];
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
		float cx        = (TL.p.x+1)*w_2;
		float cy        = (TL.p.y+1)*h_2;

		// Rotation
		float			_sin1,_cos1,_sin2,_cos2;
		float			da	= P.angle;
		_sin1			= _sin(da); _cos1 = _cos(da);
		da				+= PI_DIV_2;
		_sin2			= _sin(da); _cos2 = _cos(da);

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
	RCache.Vertex.Unlock			(vecPatches.size()*4,hGeomPatches->vb_stride);

	// *** Render
	int current=0;
	for (u32 g=0; g<groups.size(); g++)
	{
		int p_count				= groups[g];
		RCache.set_Element		(vecPatches[current].S);
		RCache.set_Geometry		(hGeomPatches);
		RCache.Render			(D3DPT_TRIANGLELIST,vOffset,0,4*p_count,0,2*p_count);
		current					+=	p_count;
		vOffset					+=	4*p_count;
	}

	// *** Cleanup
	vecPatches.clear	();
}

void	CRender::rmNear		()
{
	IRender_Target* T	=	getTarget	();
	D3DVIEWPORT9 VP		=	{0,0,T->get_width(),T->get_height(),0,0.02f };
	CHK_DX				(HW.pDevice->SetViewport(&VP));
}
void	CRender::rmFar		()
{
	IRender_Target* T	=	getTarget	();
	D3DVIEWPORT9 VP		=	{0,0,T->get_width(),T->get_height(),0.99999f,1.f };
	CHK_DX				(HW.pDevice->SetViewport(&VP));
}
void	CRender::rmNormal	()
{
	IRender_Target* T	=	getTarget	();
	D3DVIEWPORT9 VP		= {0,0,T->get_width(),T->get_height(),0,1.f };
	CHK_DX				(HW.pDevice->SetViewport(&VP));
}

IC	bool	cmp_codes			(mapNormalCodes::TNode* N1, mapNormalCodes::TNode* N2)
{	return (N1->val.ssa > N2->val.ssa);		}

IC	bool	cmp_matrices		(mapNormalMatrices::TNode* N1, mapNormalMatrices::TNode* N2)
{	return (N1->val.ssa > N2->val.ssa);		}

IC	bool	cmp_constants		(mapNormalCS::TNode* N1, mapNormalCS::TNode* N2)
{	return (N1->val.ssa > N2->val.ssa);		}

IC	bool	cmp_vs				(mapNormalVS::TNode* N1, mapNormalVS::TNode* N2)	{	return (N1->val.ssa > N2->val.ssa);		}

IC	bool	cmp_ps_nrm			(mapNormalPS::TNode* N1, mapNormalPS::TNode* N2)			{	return (N1->val.ssa > N2->val.ssa);		}

IC	bool	cmp_cs				(mapNormalCS::TNode* N1, mapNormalCS::TNode* N2)
{	return (N1->val.ssa > N2->val.ssa);		}

IC	bool	cmp_vb				(mapNormalVB::TNode* N1, mapNormalVB::TNode* N2)
{	return (N1->val.ssa > N2->val.ssa);		}

IC	bool	cmp_textures_lex2	(mapNormalTextures::TNode* N1, mapNormalTextures::TNode* N2)
{	
	STextureList*	t1			= N1->key;
	STextureList*	t2			= N2->key;
	if ((*t1)[0] < (*t2)[0])	return true;
	if ((*t1)[0] > (*t2)[0])	return false;
	if ((*t1)[1] < (*t2)[1])	return true;
	else						return false;
}
IC	bool	cmp_textures_lex3	(mapNormalTextures::TNode* N1, mapNormalTextures::TNode* N2)
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
IC	bool	cmp_textures_lexN	(mapNormalTextures::TNode* N1, mapNormalTextures::TNode* N2)
{	
	STextureList*	t1			= N1->key;
	STextureList*	t2			= N2->key;
	return std::lexicographical_compare(t1->begin(),t1->end(),t2->begin(),t2->end());
}
IC	bool	cmp_textures_ssa	(mapNormalTextures::TNode* N1, mapNormalTextures::TNode* N2)
{	
	return (N1->val.ssa > N2->val.ssa);		
}

void		sort_tlist			
(
 xr_vector<mapNormalTextures::TNode*>& lst, 
 xr_vector<mapNormalTextures::TNode*>& temp, 
 mapNormalTextures& textures, 
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
			mapNormalTextures::TNode* _it	= textures.begin	();
			mapNormalTextures::TNode* _end	= textures.end		();
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

	// Target.set_gray					(.5f+_sin(Device.fTimeGlobal)/2.f);
	Target->Begin					();

	// if (psDeviceFlags.test(rsWarmHZB))	HOM.Render_ZB	();

	// HUD render
	{
		ENGINE_API extern float		psHUD_FOV;

		// Change projection
		Fmatrix Pold				= Device.mProject;
		Fmatrix FTold				= Device.mFullTransform;
		Device.mProject.build_projection(
			deg2rad(psHUD_FOV*Device.fFOV*Device.fASPECT), 
			Device.fASPECT, VIEWPORT_NEAR, 
			g_pGamePersistent->Environment.Current.far_plane);
		Device.mFullTransform.mul	(Device.mProject, Device.mView);
		RCache.set_xform_project	(Device.mProject);

		// Rendering
		rmNear						();
		mapHUD.traverseLR			(sorted_L1);
		mapHUD.clear				();
		rmNormal					();

		// Restore projection
		Device.mProject				= Pold;
		Device.mFullTransform		= FTold;
		RCache.set_xform_project	(Device.mProject);
	}

	// Environment render

	// NORMAL			*** mostly the main level
	// Perform sorting based on ScreenSpaceArea
	RCache.set_xform_world			(Fidentity);

	// Sorting by SSA and changes minimizations
	for (u32 pr=0; pr<4; pr++)
	{
		if (0==mapNormal[pr][0].size())	continue;

		for (u32 pass_id=0; pass_id<SHADER_PASSES_MAX; pass_id++)	
		{
			mapNormalCodes&		codes		= mapNormal	[pr][pass_id];
			if (0==codes.size())			break;
			BOOL sort						= (pass_id==0);

			codes.getANY_P		(lstCodes);
			if (sort) std::sort	(lstCodes.begin(), lstCodes.end(), cmp_codes);
			for (u32 code_id=0; code_id<lstCodes.size(); code_id++)
			{
				mapNormalCodes::TNode*	Ncode	= lstCodes[code_id];
				RCache.set_States					(Ncode->key);	

				mapNormalVS&	vs				= Ncode->val;	vs.ssa	= 0;
				vs.getANY_P		(lstVS);	if (sort)		std::sort	(lstVS.begin(),lstVS.end(),cmp_vs);
				for (u32 vs_id=0; vs_id<lstVS.size(); vs_id++)
				{
					mapNormalVS::TNode*		Nvs		= lstVS[vs_id];
					RCache.set_VS						(Nvs->key);	

					mapNormalPS&		ps			= Nvs->val;		ps.ssa	= 0;
					ps.getANY_P						(lstPS);
					std::sort						(lstPS.begin(), lstPS.end(), cmp_ps_nrm);

					for (u32 ps_id=0; ps_id<lstPS.size(); ps_id++)
					{
						mapNormalPS::TNode*	Nps			= lstPS[ps_id];
						RCache.set_PS					(Nps->key);	

						mapNormalCS&	cs			= Nps->val;		cs.ssa	= 0;
						cs.getANY_P		(lstCS);	if (sort)		std::sort	(lstCS.begin(),lstCS.end(),cmp_cs);
						for (u32 cs_id=0; cs_id<lstCS.size(); cs_id++)
						{
							mapNormalCS::TNode*	Ncs	= lstCS[cs_id];
							RCache.set_xform_world					(Fidentity);
							RCache.set_Constants					(Ncs->key);

							mapNormalTextures&	tex			= Ncs->val;		tex.ssa	= 0;
							sort_tlist		(lstTextures,lstTexturesTemp,tex,sort);
							for (u32 tex_id=0; tex_id<lstTextures.size(); tex_id++)
							{
								mapNormalTextures::TNode*	Ntex = lstTextures[tex_id];
								RCache.set_Textures						(Ntex->key);	

								mapNormalVB&		vb		= Ntex->val;		vb.ssa	= 0;
								vb.getANY_P		(lstVB);	if (sort)	std::sort	(lstVB.begin(),lstVB.end(),cmp_vb);
								for (u32 vb_id=0; vb_id<lstVB.size(); vb_id++)
								{
									mapNormalVB::TNode*		Nvb = lstVB[vb_id];
									// RCache.set_Vertices					(Nvb->key);	

									mapNormalMatrices&	mat		= Nvb->val;			mat.ssa	= 0;
									mat.getANY_P	(lstMatrices);	if (sort)	std::sort	(lstMatrices.begin(),lstMatrices.end(),cmp_matrices);
									for (u32 mat_id=0; mat_id<lstMatrices.size(); mat_id++)
									{
										mapNormalMatrices::TNode*	Nmat	= lstMatrices[mat_id];
										RCache.set_Matrices						(Nmat->key);	

										mapNormalItems&				items	= Nmat->val;		items.ssa	= 0;
										mapNormal_Render						(items);
									}
									lstMatrices.clear		();
									mat.clear				();
								}
								lstVB.clear				();
								vb.clear				();
							}
							lstTextures.clear		();
							lstTexturesTemp.clear	();
							tex.clear				();
						}
						lstCS.clear				();
						cs.clear				();
					}
					lstPS.clear				();
					ps.clear				();
				}
				lstVS.clear				();
				vs.clear				();
			}
			lstCodes.clear			();
			codes.clear				();
		}

		if (1==pr)			{
			RCache.set_xform_world	(Fidentity);
			Details->Render			(Device.vCameraPosition);

			g_pGamePersistent->Environment.RenderFirst	();

			// NORMAL-matrix		*** actors and dyn. objects
			mapMatrix.traverseANY	(matrix_L1);
			mapMatrix.clear			();

			RCache.set_xform_world	(Fidentity);
			Wallmarks->Render		();		// Wallmarks has priority as normal geometry

			RCache.set_xform_world	(Fidentity);
			L_Dynamic->Render		();		// L_DB has priority the same as normal geom

			RCache.set_xform_world	(Fidentity);
			L_Shadows->render		();
		}
	}

	// LODs
	flush_LODs				();

	// Sorted (back to front)
	mapSorted.traverseRL	(sorted_L1);
	mapSorted.clear			();

	// Glows
	L_Glows->Render			();

	// Patches
	if (vecPatches.size())  {
		flush_Patches	();
	}

	g_pGamePersistent->Environment.RenderLast	();
	// L_Projector.render					();

	// Postprocess
	Target->End				();
	L_Projector->finalize	();

	// HUD
	Device.Statistic.RenderDUMP_HUD.Begin	();
	g_pGameLevel->pHUD->Render_Direct			();
	Device.Statistic.RenderDUMP_HUD.End		();

	Device.Statistic.RenderDUMP.End();
}
