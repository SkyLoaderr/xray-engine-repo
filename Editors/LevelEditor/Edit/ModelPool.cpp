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

CVisual*	CModelPool::Instance_Create(u32 type)
{
	CVisual *V = NULL;

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
	string256		fn;
	string256		name;

	// Add default ext if no ext at all
	if (0==strext(N))	strconcat	(name,N,".ogf");
	else				strcpy		(name,N);

	// Load data from MESHES or LEVEL
	if (!FS.exist(N)){
		Msg("Can't find model file '%s'.",name);
		THROW;
	} else {
		strcpy			(fn,N);
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

CVisual*	CModelPool::Instance_Load(IReader* data)
{
	CVisual	*V;

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

CVisual* CModelPool::Create(const char* name)
{
	// 1. Search for already loaded model
	char low_name[64]; R_ASSERT(strlen(name)<64);
	strcpy(low_name,name); strlwr(low_name);

	CVisual*				Model=0;
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

CVisual* CModelPool::Create(IReader* data)
{
	return Instance_Duplicate(Instance_Load(data));
}

void	CModelPool::Delete(CVisual* &V)
{
	if (V) {
		if (V->Type==MT_PARTICLE_SYSTEM) V->Release();
		xr_delete(V);
	}
}

