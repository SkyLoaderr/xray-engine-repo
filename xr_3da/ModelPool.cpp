// ModelPool.cpp: implementation of the CModelPool class.
//
//////////////////////////////////////////////////////////////////////
  
#include "stdafx.h"
#pragma hdrstop

#include "ModelPool.h"

#ifndef _EDITOR
	#include "..\IGame_Persistent.h"
    #include "..\fmesh.h"
    #include "..\fvisual.h"
    #include "..\fprogressivefixedvisual.h"
    #include "..\fhierrarhyvisual.h"
    #include "..\SkeletonAnimated.h"
    #include "..\SkeletonRigid.h"
    #include "..\fcached.h"
    #include "..\flod.h"
    #include "..\skeletonx.h"
    #include "ftreevisual.h"
    #include "PSVisual.h"
    #include "ParticleGroup.h"
#else
    #include "fmesh.h"
    #include "fvisual.h"
    #include "fprogressivefixedvisual.h"
    #include "ParticleGroup.h"
    #include "skeletonX.h"
    #include "fhierrarhyvisual.h"
    #include "SkeletonAnimated.h"
    #include "SkeletonRigid.h"
	#include "IGame_Persistent.h"
#endif

IRender_Visual*	CModelPool::Instance_Create(u32 type)
{
	IRender_Visual *V = NULL;

	// Check types
	switch (type) {
	case MT_NORMAL:				// our base visual
		V	= xr_new<Fvisual>	();
		break;
	case MT_HIERRARHY:
		V	= xr_new<FHierrarhyVisual> ();
		break;
	case MT_PROGRESSIVE:		// dynamic-resolution visual
		V	= xr_new<FProgressiveFixedVisual> ();
		break;
	case MT_SKELETON_ANIM:
		V	= xr_new<CSkeletonAnimated> ();
		break;
	case MT_SKELETON_RIGID:
		V	= xr_new<CSkeletonRigid> ();
		break;
	case MT_SKELETON_GEOMDEF_PM:
		V	= xr_new<CSkeletonX_PM> ();
		break;
	case MT_SKELETON_GEOMDEF_ST:
		V	= xr_new<CSkeletonX_ST> ();
		break;
	case MT_PARTICLE_EFFECT:
		V	= xr_new<PS::CParticleEffect> ();
		break;
	case MT_PARTICLE_GROUP:
		V	= xr_new<PS::CParticleGroup> ();
		break;
#ifndef _EDITOR
	case MT_LOD:
		V	= xr_new<FLOD> ();
		break;
	case MT_TREE:
		V	= xr_new<FTreeVisual> ();
		break;
#endif
	default:
		Debug.fatal("Unknown visual type");
		break;
	}
	R_ASSERT	(V);
	V->Type		= type;
	return		V;
}

IRender_Visual*	CModelPool::Instance_Duplicate	(IRender_Visual* V)
{
	R_ASSERT(V);
	IRender_Visual* N		= Instance_Create(V->Type);
	N->Copy			(V);
	N->Spawn		();
    // inc ref counter
	for (xr_vector<ModelDef>::iterator I=Models.begin(); I!=Models.end(); I++) 
		if (I->model==V){ I->refs++; break;}
	return N;
}

IRender_Visual*	CModelPool::Instance_Load		(const char* N)
{
	IRender_Visual	*V;
	string256		fn;
	string256		name;

	// Add default ext if no ext at all
	if (0==strext(N))	strconcat	(name,N,".ogf");
	else				strcpy		(name,N);

	// Load data from MESHES or LEVEL
	if (!FS.exist(N))	{
		if (!FS.exist(fn, "$level$", name))
			if (!FS.exist(fn, "$game_meshes$", name)){
#ifdef _EDITOR
				Msg("!Can't find model file '%s'.",name);
                return 0;
#else            
				Debug.fatal("Can't find model file '%s'.",name);
#endif
			}
	} else {
		strcpy			(fn,N);
	}
	
	// Actual loading
	if (bLogging)		Msg		("! Uncached model loading: %s",fn);
	IReader*			data	= FS.r_open(fn);
	ogf_header			H;
	data->r_chunk_safe	(OGF_HEADER,&H,sizeof(H));
	V = Instance_Create (H.type);
	V->Load				(fn,data,0);
	FS.r_close			(data);
	g_pGamePersistent->RegisterModel(V);

	// Registration
	Instance_Register	(N,V);

	return V;
}

IRender_Visual*	CModelPool::Instance_Load(LPCSTR name, IReader* data)
{
	IRender_Visual	*V;
	
	// Actual loading
	if (bLogging)		Msg		("! Uncached model loading: %s",name);
	ogf_header			H;
	data->r_chunk_safe	(OGF_HEADER,&H,sizeof(H));
	V = Instance_Create (H.type);
	V->Load				(name,data,0);

	// Registration
	Instance_Register	(name,V);
	return V;
}

void		CModelPool::Instance_Register(LPCSTR N, IRender_Visual* V)
{
	// Registration
	ModelDef			M;
	M.name				= N;
	M.model				= V;
	Models.push_back	(M);
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
void CModelPool::Destroy()
{
	// Pool
	Pool.clear		();

	// Registry
	for (REGISTRY_IT it=Registry.begin(); it!=Registry.end(); it++)
	{
		xr_delete	((IRender_Visual*)it->first);
		xr_free		(it->second);
	}
	Registry.clear();

	// Base/Reference
	xr_vector<ModelDef>::iterator	I;
	for (I=Models.begin(); I!=Models.end(); I++) 
	{
		I->model->Release();
		xr_delete(I->model);
	}
	Models.clear();
}

CModelPool::CModelPool()
{
	bLogging	= TRUE;
}

CModelPool::~CModelPool()
{
	Destroy		();
}

IRender_Visual* CModelPool::Instance_Find(LPCSTR N)
{
	IRender_Visual*				Model=0;
	xr_vector<ModelDef>::iterator	I;
	for (I=Models.begin(); I!=Models.end(); I++)
	{
		if (I->name[0]&&(0==strcmp(*I->name,N))) {
			Model = I->model;
			break;
		}
	}
	return Model;
}

IRender_Visual* CModelPool::Create(const char* name)
{
	// Msg					("-CREATE %s",name);
	string128 low_name;		R_ASSERT(xr_strlen(name)<128);
	strcpy(low_name,name);	strlwr(low_name);
	IRender_Visual* Model			= NULL;

	// 0. Search POOL
	POOL_IT	it			=	Pool.find	(low_name);
	if (it!=Pool.end())
	{
		// 1. Instance found
		Model				=	it->second;
		Model->Spawn		();
		Pool.erase			(it);
	} else {
		// 1. Search for already loaded model (reference, base model)
		IRender_Visual* Base		= Instance_Find		(low_name);

		if (0!=Base) 
		{
			// 2. If found - return (cloned) reference
			Model			= Instance_Duplicate(Base);
			Registry.insert	(mk_pair(Model,xr_strdup(low_name)));
		} else {
			// 3. If not found
            Model			= Instance_Load(low_name);
#ifdef _EDITOR
			if (!Model)		return 0;
#endif
			Model			= Instance_Duplicate(Model);
			Registry.insert	(mk_pair(Model,xr_strdup(low_name)));
		}
	}

	return	Model;
}

IRender_Visual* CModelPool::Create(LPCSTR name, IReader* data)
{
	string128 low_name;		R_ASSERT(xr_strlen(name)<128);
	strcpy(low_name,name);	strlwr(low_name);

	// 1. Search for already loaded model
	IRender_Visual* Model			= Instance_Find(low_name);

	// 2. If found - return reference
	if (0!=Model)			return Instance_Duplicate(Model);

	// 3. If not found
	return					Instance_Duplicate(Instance_Load(name,data));
}

void	CModelPool::Delete	(IRender_Visual* &V, BOOL bDiscard)
{
	if (bDiscard)			Discard(V); 
	else 
	{
		//
		REGISTRY_IT	it		= Registry.find	(V);
		if (it!=Registry.end())
		{
			// Registry entry found - move it to pool
			Pool.insert			(mk_pair(it->second,V));
		} else {
			// Registry entry not-found - just special type of visual / particles / etc.
			xr_delete			(V);
		}
	}
	V	=	NULL;
}

void	CModelPool::Discard	(IRender_Visual* &V)
{
	//
	REGISTRY_IT	it		= Registry.find	(V);
	if (it!=Registry.end())
	{
		// Pool - OK

		// Base
		LPCSTR	name	= it->second;
		for (xr_vector<ModelDef>::iterator I=Models.begin(); I!=Models.end(); I++)
		{
			if (I->name[0] && (0==strcmp(*I->name,name))) 
			{
            	VERIFY(I->refs>0);
            	I->refs--; 
                if (0==I->refs){
	            	I->model->Release();
					xr_delete		(I->model);	
					Models.erase	(I);
                }
				break;
			}
		}

		// Registry
		xr_delete		(V);	
		xr_free			(name);
		Registry.erase	(it);
	} else {
		// Registry entry not-found - just special type of visual / particles / etc.
		xr_delete		(V);
	}
	V	=	NULL;
}

IRender_Visual* CModelPool::CreatePE	(PS::CPEDef* source)
{
	PS::CParticleEffect* V	= (PS::CParticleEffect*)Instance_Create(MT_PARTICLE_EFFECT);
	V->Compile		(source);
	return V;
}

IRender_Visual* CModelPool::CreatePG	(PS::CPGDef* source)
{
	PS::CParticleGroup* V	= (PS::CParticleGroup*)Instance_Create(MT_PARTICLE_GROUP);
	V->Compile		(source);
	return V;
}

#ifdef _EDITOR
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
    switch (m_pVisual->Type){
    case MT_SKELETON_ANIM:
    case MT_SKELETON_RIGID:
    case MT_HIERRARHY:{
        FHierrarhyVisual* pV			= dynamic_cast<FHierrarhyVisual*>(m_pVisual); R_ASSERT(pV);
        xr_vector<IRender_Visual*>::iterator 		I,E;
        I = pV->children.begin			();
        E = pV->children.end			();
        for (; I!=E; I++){
            IRender_Visual* V			= *I;
			if (_IsRender(V,mTransform,priority,strictB2F)){
		        RCache.set_Shader		(V->hShader?V->hShader:Device.m_WireShader);
			    RCache.set_xform_world	(mTransform);
	            V->Render		 		(m_fLOD);
            }
        }
    }break;
    case MT_PARTICLE_GROUP:{
        PS::CParticleGroup* pV			= dynamic_cast<PS::CParticleGroup*>(m_pVisual); R_ASSERT(pV);
        xr_vector<IRender_Visual*>::iterator 		I,E;
        I = pV->children.begin			();
        E = pV->children.end			();
        for (; I!=E; I++){
            RCache.set_xform_world		(mTransform);
            Render						(*I,Fidentity,priority,strictB2F,m_fLOD);
        }
    }break;
    case MT_PARTICLE_EFFECT:{
		if (_IsRender(m_pVisual,Fidentity,priority,strictB2F)){
	        RCache.set_Shader			(m_pVisual->hShader?m_pVisual->hShader:Device.m_WireShader);
            RCache.set_xform_world		(mTransform);
            m_pVisual->Render		 	(m_fLOD);
        }
    }break;
    default:
		if (_IsRender(m_pVisual,mTransform,priority,strictB2F)){
	        RCache.set_Shader			(m_pVisual->hShader?m_pVisual->hShader:Device.m_WireShader);
            RCache.set_xform_world		(mTransform);
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
void CModelPool::OnDeviceDestroy()
{
	Destroy();
}
#endif
