// FVisualContainer.cpp: implementation of the FVisualContainer class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "fvisual.h"
#include "fvisualcontainer.h"
#include "fhierrarhyvisual.h"
#include "ftimer.h"
#include "ffileops.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

FVisualContainer::FVisualContainer()
{
}

void FVisualContainer::Load()
{
	CTimer		timer;
	FILE_NAME	fn;
	FILE_NAME	fn2;

	Log("Loading static geometry...");
	timer.Start();
	strcpy(fn,MODE.folder); 
	strcat(fn,"meshlist.");

	CStream* data = new CStream(fn);
	int		count = data->ReadINT();

	char	cName[64];
	int		idx;

	for (idx=0; idx<count; idx++) {
		data->ReadStringZ	(cName);
		FBasicVisual* pV = LoadVisual(cName);
		Items.push_back(pV);
	}
	_DELETE(data);
	
	strcpy(fn2,MODE.folder); 
	strcat(fn2,"meshlist.bak");
	rename(fn,fn2);
}

_E(FVisualContainer::~FVisualContainer(void))
	// Geometry
	for (int i=0; i<Items.size(); i++) {
		_DELETE(Items[i]);
	}
	Items.clear();
_end;

_E(void FVisualContainer::PerformLighting())
_end;

_E(void FVisualContainer::PerformOptimize())
	Log("Optimizing geometry...");
	CTimer t;
	t.Start();
	for (int i=0; i<Items.size(); i++)
		if (Items[i])
		Items[i]->PerformOptimize();
	t.Log();
_end;

_EF(FBasicVisual*	CreateVisualByType(DWORD type))
	FBasicVisual *pVisual = NULL;

	// Check types
	switch (type) {
	case MT_NORMAL:				// our base visual
		pVisual = new Fvisual;
		break;
	case MT_HIERRARHY:
		pVisual	= new FHierrarhyVisual;
		break;
	case MT_PROGRESSIVE:		// dynamic-resolution visual
	case MT_PROGRESSIVE_FIXED:	// fixed-set resolution visual
	case MT_FIELD_ARROWS:
	case MT_FIELD_WALLS:
	case MT_SHADOW_FORM:
	case MT_SHOTMARK:
		pVisual = new Fvisual;
		break;
	default:
		VERIFY2(0,"Unknown visual type");
		break;
	}
	return pVisual;
_end;

_EF(FBasicVisual*	LoadVisual(char *name))
	FBasicVisual	*pVisual;
	FILE_NAME		fn;

//	Msg("* Loading visual %s...",name);
	// Load data from MESHES or LEVEL
	strcpy(fn,MODE.folder);
	strcat(fn,name);
	if (!FileExists(fn)) {
		strcpy(fn,MESHES_PATH);
		strcat(fn,name);
	}

	// Actual loading
	CStream *data = new CStream(fn);
	ogf_header h;
	data->ReadChunk(OGF_HEADER,&h);
	pVisual = CreateVisualByType(h.type);
	if (pVisual) {
		strcpy(pVisual->fName,name);
		pVisual->Load(data);
	}
	_DELETE(data);

	return pVisual;
_end;
