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
    #include "..\fprogressive.h"
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
    #include "fprogressive.h"
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
		V	= xr_new<FProgressive> ();
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

IRender_Visual*	CModelPool::Instance_Load		(const char* N, BOOL allow_register)
{
	IRender_Visual	*V;
	string512		fn;
	string512		name;

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
	V->Load				(N,data,0);
	FS.r_close			(data);
	g_pGamePersistent->RegisterModel(V);

	// Registration
	if (allow_register) Instance_Register(N,V);

	return V;
}

IRender_Visual*	CModelPool::Instance_Load(LPCSTR name, IReader* data, BOOL allow_register)
{
	IRender_Visual	*V;
	
	// Actual loading
	if (bLogging)		Msg		("! Uncached model loading: %s",name);
	ogf_header			H;
	data->r_chunk_safe	(OGF_HEADER,&H,sizeof(H));
	V = Instance_Create (H.type);
	V->Load				(name,data,0);

	// Registration
	if (allow_register) Instance_Register(name,V);
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
	bLogging				= TRUE;
    bForceDiscard 			= FALSE;
    bAllowChildrenDuplicate	= TRUE; 
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
		if (I->name[0]&&(0==xr_strcmp(*I->name,N))) {
			Model = I->model;
			break;
		}
	}
	return Model;
}

IRender_Visual* CModelPool::Create(const char* name, IReader* data)
{
#ifdef _EDITOR
	if (!name||!name[0])	return 0;
#endif
	string1024 low_name;	VERIFY	(xr_strlen(name)<sizeof(low_name));
	strcpy(low_name,name);	strlwr	(low_name);
	if (strext(low_name))	*strext	(low_name)=0;
//	Msg						("-CREATE %s",low_name);

	// 0. Search POOL
	POOL_IT	it			=	Pool.find	(low_name);
	if (it!=Pool.end())
	{
		// 1. Instance found
        IRender_Visual*		Model	= it->second;
		Model->Spawn		();
		Pool.erase			(it);
		return				Model;
	} else {
		// 1. Search for already loaded model (reference, base model)
		IRender_Visual* Base		= Instance_Find		(low_name);

		if (0==Base){
			// 2. If not found
			bAllowChildrenDuplicate	= FALSE;
			if (data)		Base = Instance_Load(low_name,data,TRUE);
            else			Base = Instance_Load(low_name,TRUE);
			bAllowChildrenDuplicate	= TRUE;
#ifdef _EDITOR
			if (!Base)		return 0;
#endif
		}
        // 3. If found - return (cloned) reference
        IRender_Visual*		Model	= Instance_Duplicate(Base);
        Registry.insert		(mk_pair(Model,xr_strdup(low_name)));
        return				Model;
	}
}

IRender_Visual* CModelPool::CreateChild(LPCSTR name, IReader* data)
{
	string256 low_name;		VERIFY	(xr_strlen(name)<256);
	strcpy(low_name,name);	strlwr	(low_name);
	if (strext(low_name))	*strext	(low_name) = 0;

	// 1. Search for already loaded model
	IRender_Visual* Base	= Instance_Find(low_name);
	if (0==Base) Base	 	= Instance_Load(name,data,FALSE);

    IRender_Visual* Model	= bAllowChildrenDuplicate?Instance_Duplicate(Base):Base;
    return					Model;
}

void	CModelPool::Delete	(IRender_Visual* &V, BOOL bDiscard)
{
	if (bDiscard||bForceDiscard){
    	Discard(V); 
	}else{
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
			if (I->name[0] && (0==xr_strcmp(*I->name,name))) 
			{
            	VERIFY(I->refs>0);
            	I->refs--; 
                if (0==I->refs){
                	bForceDiscard	= TRUE;
	            	I->model->Release();
					xr_delete		(I->model);	
					Models.erase	(I);
                    bForceDiscard	= FALSE;
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
	if ((priority==(visual->hShader?visual->hShader->E[0]->flags.iPriority:1))&&(strictB2F==!!(visual->hShader?visual->hShader->E[0]->flags.bStrictB2F:false))){
        Fbox bb; bb.xform(visual->vis.box,transform);
        return ::Render->occ_visible(bb);
    }
    return false;
}

void 	CModelPool::Render(IRender_Visual* m_pVisual, const Fmatrix& mTransform, int priority, bool strictB2F, float m_fLOD)
{
    // render visual
    xr_vector<IRender_Visual*>::iterator I,E;
    switch (m_pVisual->Type){
    case MT_SKELETON_ANIM:
    case MT_SKELETON_RIGID:
    case MT_HIERRARHY:{
        if (_IsRender(m_pVisual,mTransform,priority,strictB2F)){
            FHierrarhyVisual* pV		= dynamic_cast<FHierrarhyVisual*>(m_pVisual); VERIFY(pV);
            I = pV->children.begin		();
            E = pV->children.end		();
            for (; I!=E; I++){
                RCache.set_Shader		((*I)->hShader?(*I)->hShader:Device.m_WireShader);
                RCache.set_xform_world	(mTransform);
                (*I)->Render		 	(m_fLOD);
            }
        }
    }break;
    case MT_PARTICLE_GROUP:{
        PS::CParticleGroup* pG			= dynamic_cast<PS::CParticleGroup*>(m_pVisual); VERIFY(pG);
        RCache.set_xform_world			(mTransform);
	    for (PS::CParticleGroup::SItemVecIt i_it=pG->items.begin(); i_it!=pG->items.end(); i_it++){
            xr_vector<IRender_Visual*>	visuals;
            i_it->GetVisuals			(visuals);
            for (xr_vector<IRender_Visual*>::iterator it=visuals.begin(); it!=visuals.end(); it++)
	            Render					(*it,Fidentity,priority,strictB2F,m_fLOD);
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
