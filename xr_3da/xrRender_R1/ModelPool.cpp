// ModelPool.cpp: implementation of the CModelPool class.
//
//////////////////////////////////////////////////////////////////////
  
#include "stdafx.h"
#include "ModelPool.h"

#include "..\fmesh.h"
#include "..\fvisual.h"
#include "..\fprogressivefixedvisual.h"
#include "..\fhierrarhyvisual.h"
#include "..\bodyinstance.h"
#include "..\fcached.h"
#include "..\flod.h"
#include "..\skeletonx.h"
#include "ftreevisual.h"
#include "PSVisual.h"
#include "ParticleGroup.h"

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
	case MT_SKELETON:
		V	= xr_new<CKinematics> ();
		break;
	case MT_SKELETON_GEOMDEF_PM:
		V	= xr_new<CSkeletonX_PM> ();
		break;
	case MT_SKELETON_GEOMDEF_ST:
		V	= xr_new<CSkeletonX_ST> ();
		break;
	case MT_PARTICLE_SYSTEM:
		V	= xr_new<CPSVisual> ();
		break;
	case MT_PARTICLE_EFFECT:
		V	= xr_new<PS::CParticleEffect> ();
		break;
	case MT_PARTICLE_GROUP:
		V	= xr_new<PS::CParticleGroup> ();
		break;
	case MT_CACHED:
		V	= xr_new<FCached> ();
		break;
	case MT_LOD:
		V	= xr_new<FLOD> ();
		break;
	case MT_TREE:
		V	= xr_new<FTreeVisual> ();
		break;
	default:
		R_ASSERT2(0,"Unknown visual type");
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
				Debug.fatal("Can't find model file '%s'.",name);
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
	FS.r_close		(data);

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
	strcpy				(M.name,N);
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
		xr_delete	(it->first);
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
		if (I->name[0]&&(0==strcmp(I->name,N))) {
			Model = I->model;
			break;
		}
	}
	return Model;
}

IRender_Visual* CModelPool::Create(const char* name)
{
	// Msg					("-CREATE %s",name);
	string128 low_name;		R_ASSERT(strlen(name)<128);
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
			Model			= Instance_Duplicate(Instance_Load(low_name));
			Registry.insert	(mk_pair(Model,xr_strdup(low_name)));
		}
	}

	return	Model;
}

IRender_Visual* CModelPool::Create(LPCSTR name, IReader* data)
{
	string128 low_name;		R_ASSERT(strlen(name)<128);
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
			if (I->name[0] && (0==strcmp(I->name,name))) 
			{
				xr_delete		(I->model);	
				Models.erase	(I);
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

IRender_Visual* CModelPool::CreatePS	(PS::SDef* source, PS::SEmitter* E)
{
	CPSVisual* V	= (CPSVisual*)Instance_Create(MT_PARTICLE_SYSTEM);
	V->Compile		(source,E);
	return V;
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
