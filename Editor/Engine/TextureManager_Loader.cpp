#include "stdafx.h"
#pragma hdrstop

#include "fs.h"
#include "blenders\blender.h"

void CShaderManager::xrStartUp()
{
#ifdef M_BORLAND
	CCompressedStream		FS("game\\shaders.xr","shENGINE");
#else
	CCompressedStream		FS("shaders.xr","shENGINE");
#endif

	// Load blenders
	{
		CStream*	fs		= FS.OpenChunk(2);
		while (!fs->Eof())	{
			CBlender_DESC	desc;
			fs->Read		(&desc,sizeof(desc));
			CBlender*		B = CBlender::Create(desc.CLS);
			fs->Seek		(fs->Tell()-sizeof(desc));
			B->Load			(*fs);
			blenders.insert	(make_pair(strdup(desc.cName),B));
		}
		fs->Close();
	}
}

void CShaderManager::xrShutDown()
{
	// Release blenders
	{
		for (map<LPSTR,CBlender*,str_pred>::iterator I=blenders.begin(); I!=blenders.end(); I++)
		{
			free	(I->first);
			delete	I->second;
		}
		blenders.clear();
	}
}
