// ModelPool.cpp: implementation of the CModelPool class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#include "ModelPool.h"
#include "fmesh.h"
#include "fvisual.h"
#include "fprogressivefixedvisual.h"
//#include "fprogressive.h"
#include "fhierrarhyvisual.h"
#include "bodyinstance.h"

IRender_Visual*	CModelPool::Instance_Create(u32 type)
{
	IRender_Visual *V = NULL;

	// Check types
	switch (type) {
	case MT_NORMAL:				// our base visual
		V	= xr_new<Fvisual>();
		break;
	case MT_HIERRARHY:
		V	= xr_new<FHierrarhyVisual>();
		break;
	case MT_PROGRESSIVE:		// dynamic-resolution visual
		V	= xr_new<FProgressiveFixedVisual>();
		break;
	case MT_SKELETON:
		V	= xr_new<CKinematics>();
		break;
	case MT_SKELETON_PART:
		V	= xr_new<CSkeletonX_PM>();
		break;
	case MT_SKELETON_PART_STRIPPED:
		V	= xr_new<CSkeletonX_ST>();
		break;
	case MT_PARTICLE_EFFECT:
		V	= xr_new<PS::CParticleEffect> ();
		break;
	case MT_PARTICLE_GROUP:
		V	= xr_new<PS::CParticleGroup> ();
		break;
//	case MT_PROGRESSIVE_STRIPS:
//  		V	= xr_new<FProgressive>();
//		break;
	default:
		R_ASSERT(0=="Unknown visual type");
		break;
	}
	R_ASSERT(V);
	V->Type = type;
	return V;
}
IRender_Visual*	CModelPool::Instance_Duplicate	(IRender_Visual* V)
{
	R_ASSERT(V);
	IRender_Visual* N = Instance_Create(V->Type);
	N->Copy	(V);
	return N;
}

IRender_Visual*	CModelPool::Instance_Load		(const char* N)
{
	IRender_Visual*		V;
	string256			fn;
	string256			name;

	// Add default ext if no ext at all
	if (0==strext(N))	strconcat	(name,N,".ogf");
	else				strcpy		(name,N);

    FS.update_path		(fn,_game_meshes_,name);
    
	// Load data from MESHES or LEVEL
	if (!FS.exist(fn)){
		ELog.DlgMsg		(mtError,"Can't find model file '%s'.",fn);
		return 			0;
	}
	

	// Actual loading
	IReader*			data	= FS.r_open(fn);
	ogf_header			H;
	data->r_chunk_safe	(OGF_HEADER,&H,sizeof(H));
	V = Instance_Create (H.type);
	V->Load				(fn,data,0);
	FS.r_close			(data);

	// Registration
	ModelDef			M;
	strcpy				(M.name,N);
	M.model				= V;
	Models.push_back	(M);

	return V;
}

IRender_Visual*	CModelPool::Instance_Load(IReader* data)
{
	IRender_Visual	*V;

	// Actual loading
	ogf_header			H;
	data->r_chunk_safe	(OGF_HEADER,&H,sizeof(H));
	V = Instance_Create (H.type);
	V->Load				(0,data,0);

	// Registration
	ModelDef			M;
	M.name[0]			= 0;
	M.model				= V;
	Models.push_back	(M);

	return V;
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

void CModelPool::OnDestroy()
{
	xr_vector<ModelDef>::iterator	I;
	for (I=Models.begin(); I!=Models.end(); I++)
	{
		I->model->Release();
		xr_delete(I->model);
	}
	Models.clear();
}
void CModelPool::OnDeviceDestroy()
{
	OnDestroy();
}
CModelPool::CModelPool()
{
}
CModelPool::~CModelPool()
{
}

IRender_Visual* CModelPool::Create(const char* name)
{
	// 1. Search for already loaded model
	char low_name[64]; R_ASSERT(strlen(name)<64);
	strcpy(low_name,name); strlwr(low_name);

	IRender_Visual*				Model=0;
	xr_vector<ModelDef>::iterator	I;
	for (I=Models.begin(); I!=Models.end(); I++)
	{
		if (I->name[0]&&(0==strcmp(I->name,low_name))) {
			Model = I->model;
			break;
		}
	}

	// 2. If found - return reference
	if (0!=Model) return Instance_Duplicate(Model);

	// 3. If not found
	return Instance_Duplicate(Instance_Load(low_name));
}

IRender_Visual* CModelPool::Create(IReader* data)
{
	return Instance_Duplicate(Instance_Load(data));
}

void	CModelPool::Delete(IRender_Visual* &V)
{
	if (V) {
		if (V->Type==MT_PARTICLE_SYSTEM) V->Release();
		xr_delete(V);
	}
}

IC bool	_IsRender(IRender_Visual* visual, const Fmatrix& transform, u32 priority, bool strictB2F)
{
	if ((priority==(visual->hShader?visual->hShader->E[0]->Flags.iPriority:1))&&(strictB2F==!!(visual->hShader?visual->hShader->E[0]->Flags.bStrictB2F:false))){
        Fbox bb; bb.xform(visual->vis.box,transform);
        return ::Render->occ_visible(bb);
    }
    return false;
}

void 	CModelPool::Render(IRender_Visual* m_pVisual, const Fmatrix& mTransform, int priority, bool strictB2F, float m_fLOD)
{
    // render visual
    RCache.set_xform_world(mTransform);
    switch (m_pVisual->Type){
    case MT_PARTICLE_GROUP:
    case MT_SKELETON:
    case MT_HIERRARHY:{
        FHierrarhyVisual* pV			= (FHierrarhyVisual*)m_pVisual;
        xr_vector<IRender_Visual*>::iterator 		I,E;
        I = pV->children.begin			();
        E = pV->children.end			();
        for (; I!=E; I++){
            IRender_Visual* V					= *I;
			if (_IsRender(V,mTransform,priority,strictB2F)){
		        RCache.set_Shader		(V->hShader?V->hShader:Device.m_WireShader);
	            V->Render		 		(m_fLOD);
            }
        }
    }break;
    default:
		if (_IsRender(m_pVisual,mTransform,priority,strictB2F)){
	        RCache.set_Shader			(m_pVisual->hShader?m_pVisual->hShader:Device.m_WireShader);
            m_pVisual->Render		 	(m_fLOD);
        }
        break;
    }
}

void 	CModelPool::RenderSingle(IRender_Visual* m_pVisual, const Fmatrix& mTransform, float m_fLOD)
{
	for (int p=0; p<4; p++){
    	Render(m_pVisual,mTransform,p,false,m_fLOD);
    	Render(m_pVisual,mTransform,p,true,m_fLOD);
    }
}

PS::CParticleEffect* CModelPool::CreatePE	(PS::CPEDef* source)
{
	PS::CParticleEffect* V	= (PS::CParticleEffect*)Instance_Create(MT_PARTICLE_EFFECT);
	V->Compile		(source);
	return V;
}

PS::CParticleGroup* CModelPool::CreatePG	(PS::CPGDef* source)
{
	PS::CParticleGroup* V	= (PS::CParticleGroup*)Instance_Create(MT_PARTICLE_GROUP);
	V->Compile		(source);
	return V;  	
}

