//----------------------------------------------------
// file: ObjectOptionPack.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "Log.h"
#include "FileSystem.h"
#include "ObjectOptionPack.h"
#include "PropertiesSubObject.h"
//----------------------------------------------------

bool ObjectOptionPack::Load( FSChunkDef *chunk ){
	_ASSERTE( chunk );
	int version = FS.readword( chunk->filehandle );
	if( version == 4 ){
	} else {
		Reset();
	}
	return true;
}


bool ObjectOptionPack::Save( int handle ){
	// version
	FS.writeword( handle, 4 );
	// data
	return true;
}


//----------------------------------------------------
void ObjectOptionPack::Init()
{
}

void ObjectOptionPack::Result()
{
}

bool ObjectOptionPack::RunEditor( ){
    int mr = 0;
    Init();
    while (mr!=mrOk && mr!=mrCancel){
        mr = frmPropertiesSubObject->ShowModal();
        if (mr==mrYes || mr==mrOk) Result();
    }
	return true;
}


