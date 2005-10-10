/*
======================================================================
LW_import_object.cpp

Load an object in XRay .object format.

Alexander Maximchuk  13 Jun 02
====================================================================== */
#include "stdafx.h"

#include <lwserver.h>
#include <lwobjimp.h>
#include <lwsurf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <lwhost.h>

#include "EditObject.h"

//======================================================================
//LW_ObjectImport()
//
//Activation function for the loader.
//======================================================================
extern "C" XCALL_( int ) LW_ObjectImport( long version, GlobalFunc *global, LWObjectImport *lwi, void *serverData )
{
	Engine.FS.OnCreate();	
	
	if (!Engine.FS.Exist(lwi->filename)){
		lwi->result = LWOBJIM_BADFILE;
		return AFUNC_OK;
	}

	CStream* F = Engine.FS.Open(lwi->filename);
	if (!F){
		lwi->result = LWOBJIM_BADFILE;
		return AFUNC_OK;
	}

	// see whether this is a  XRay Object file if not, let someone else try to load it
    CStream* OBJ = F->OpenChunk(EOBJ_CHUNK_OBJECT_BODY);
	if (!OBJ){
		Engine.FS.Close(F);
		lwi->result = LWOBJIM_NOREC;
		return AFUNC_OK;
	}
	
	CEditableObject* OBJECT=new CEditableObject(lwi->filename);
	if (!OBJECT->Load(*OBJ)||!OBJECT->Export_LW(lwi)){// !OBJECT->ExportLWO("x:\\import\\temp.lwo")){
		OBJ->Close();
		Engine.FS.Close(F);
		lwi->result = LWOBJIM_FAILED;
		_DELETE(OBJECT);
		return AFUNC_OK;
	}

	// finalize
	_DELETE(OBJECT);
	OBJ->Close();
	Engine.FS.Close(F);

	return AFUNC_OK;
}
