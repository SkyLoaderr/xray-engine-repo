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

IVisual*	CModelPool::Instance_Create(u32 type)
{
	IVisual *V = NULL;

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
IVisual*	CModelPool::Instance_Duplicate	(IVisual* V)
{
	R_ASSERT(V);
	IVisual* N = Instance_Create(V->Type);
	N->Copy	(V);
	return N;
}

IVisual*	CModelPool::Instance_Load		(const char* N)
{
	IVisual*			V;
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

IVisual*	CModelPool::Instance_Load(IReader* data)
{
	IVisual	*V;

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
	vector<ModelDef>::iterator	I;
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

IVisual* CModelPool::Create(const char* name)
{
	// 1. Search for already loaded model
	char low_name[64]; R_ASSERT(strlen(name)<64);
	strcpy(low_name,name); strlwr(low_name);

	IVisual*				Model=0;
	vector<ModelDef>::iterator	I;
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

IVisual* CModelPool::Create(IReader* data)
{
	return Instance_Duplicate(Instance_Load(data));
}

void	CModelPool::Delete(IVisual* &V)
{
	if (V) {
		if (V->Type==MT_PARTICLE_SYSTEM) V->Release();
		xr_delete(V);
	}
}

IC bool	_IsRender(IVisual* visual, const Fmatrix& transform, u32 priority, bool strictB2F)
{
	if ((priority==(visual->hShader?visual->hShader->E[0]->Flags.iPriority:1))&&(strictB2F==!!(visual->hShader?visual->hShader->E[0]->Flags.bStrictB2F:false))){
        Fbox bb; bb.xform(visual->vis.box,transform);
        return ::Render->occ_visible(bb);
    }
    return false;
}

void 	CModelPool::Render(IVisual* m_pVisual, const Fmatrix& mTransform, int priority, bool strictB2F, float m_fLOD)
{
    // render visual
    RCache.set_xform_world(mTransform);
    switch (m_pVisual->Type){
    case MT_SKELETON:{
        CKinematics* pV					= (CKinematics*)m_pVisual;
        vector<IVisual*>::iterator I,E;
        I = pV->children.begin			();
        E = pV->children.end			();
        for (; I!=E; I++){
            IVisual* V					= *I;
            // frustum test
			if (_IsRender(V,mTransform,priority,strictB2F)){
		        RCache.set_Shader		(V->hShader?V->hShader:Device.m_WireShader);
    	        V->Render		 		(m_fLOD);
            }
        }
    }break;
    case MT_HIERRARHY:{
        FHierrarhyVisual* pV			= (FHierrarhyVisual*)m_pVisual;
        vector<IVisual*>::iterator 		I,E;
        I = pV->children.begin			();
        E = pV->children.end			();
        for (; I!=E; I++){
            IVisual* V					= *I;
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

