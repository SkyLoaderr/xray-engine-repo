// exxZERO Time Stamp AddIn. Document modified at : Thursday, March 07, 2002 18:46:47 , by user : Oles , from computer : OLES
#include "stdafx.h"
#include "fs.h"
#include "blender.h"

void CShaderManager::xrStartUp()
{
	CCompressedStream		FS("shaders.xr","xrSHADER");
	char					name[256];

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
