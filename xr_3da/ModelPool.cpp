// ModelPool.cpp: implementation of the CModelPool class.
//
//////////////////////////////////////////////////////////////////////
  
#include "stdafx.h"
#include "ModelPool.h"
#include "fmesh.h"
#include "fvisual.h"
#include "fprogressivefixedvisual.h"
#include "fhierrarhyvisual.h"
#include "bodyinstance.h"
#include "PSVisual.h"
#include "fcached.h"
#include "flod.h"

#include "x_ray.h"

CVisual*	CModelPool::Instance_Create(u32 type)
{
	CVisual *V = NULL;

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
	case MT_SKELETON_PART:
		V	= xr_new<CSkeletonX_PM> ();
		break;
	case MT_SKELETON_PART_STRIPPED:
		V	= xr_new<CSkeletonX_ST> ();
		break;
	case MT_PARTICLE_SYSTEM:
		V	= xr_new<CPSVisual> ();
		break;
	case MT_CACHED:
		V	= xr_new<FCached> ();
		break;
	case MT_LOD:
		V	= xr_new<FLOD> ();
		break;
	default:
		R_ASSERT2(0,"Unknown visual type");
		break;
	}
	R_ASSERT	(V);
	V->Type		= type;
	return		V;
}

CVisual*	CModelPool::Instance_Duplicate	(CVisual* V)
{
	R_ASSERT(V);
	CVisual* N = Instance_Create(V->Type);
	N->Copy	(V);
	return N;
}

CVisual*	CModelPool::Instance_Load		(const char* N)
{
	CVisual	*V;
	FILE_NAME		fn;
	FILE_NAME		name;

	// Add default ext if no ext at all
	if (0==strext(N))	strconcat	(name,N,".ogf");
	else				strcpy		(name,N);

	// Load data from MESHES or LEVEL
	if (!Engine.FS.Exist(N))	{
		if (!Engine.FS.Exist(fn, Path.Current, name))
			if (!Engine.FS.Exist(fn, Path.Meshes, name)){
				Msg("Can't find model file '%s'.",name);
				THROW;
			}
	} else {
		strcpy			(fn,N);
	}
	
	// Actual loading
	CStream*			data	= Engine.FS.Open(fn);
	ogf_header			H;
	data->ReadChunkSafe	(OGF_HEADER,&H,sizeof(H));
	V = Instance_Create (H.type);
	V->Load				(fn,data,0);
	Engine.FS.Close		(data);

	// Registration
	Instance_Register	(N,V);

	return V;
}

CVisual*	CModelPool::Instance_Load(LPCSTR name, CStream* data)
{
	CVisual	*V;
	
	// Actual loading
	ogf_header			H;
	data->ReadChunkSafe	(OGF_HEADER,&H,sizeof(H));
	V = Instance_Create (H.type);
	V->Load				(name,data,0);

	// Registration
	Instance_Register	(name,V);
	return V;
}

void		CModelPool::Instance_Register(LPCSTR N, CVisual* V)
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
	Destroy();
}

CModelPool::CModelPool()
{
	Device.seqDevDestroy.Add	(this,REG_PRIORITY_LOW);
}

CModelPool::~CModelPool()
{
	Device.seqDevDestroy.Remove	(this);
}

CVisual* CModelPool::Instance_Find(LPCSTR N)
{
	CVisual*				Model=0;
	vector<ModelDef>::iterator	I;
	for (I=Models.begin(); I!=Models.end(); I++)
	{
		if (I->name[0]&&(0==strcmp(I->name,N))) {
			Model = I->model;
			break;
		}
	}
	return Model;
}

CVisual* CModelPool::Create(const char* name)
{
	// Msg					("-CREATE %s",name);
	string128 low_name;		R_ASSERT(strlen(name)<128);
	strcpy(low_name,name);	strlwr(low_name);

	// 1. Search for already loaded model
	CVisual* Model	= Instance_Find(low_name);

	// 2. If found - return reference
	if (0!=Model) return Instance_Duplicate(Model);

	// 3. If not found
	return Instance_Duplicate(Instance_Load(low_name));
}

CVisual* CModelPool::Create(LPCSTR name, CStream* data)
{
	// Msg					("-CREATE_STREAM- %s",name);
	string128 low_name;		R_ASSERT(strlen(name)<128);
	strcpy(low_name,name);	strlwr(low_name);

	// 1. Search for already loaded model
	CVisual* Model	= Instance_Find(low_name);

	// 2. If found - return reference
	if (0!=Model) return Instance_Duplicate(Model);

	// 3. If not found
	return Instance_Duplicate(Instance_Load(name,data));
}

void	CModelPool::Delete(CVisual* &V)
{
	xr_delete			(V);
}

CVisual* CModelPool::CreatePS	(PS::SDef_RT* source, PS::SEmitter* E)
{
	CPSVisual* V	= (CPSVisual*)Instance_Create(MT_PARTICLE_SYSTEM);
	V->Compile		(source,E);
	return V;
}
