#include "stdafx.h"

void CShaderManager::xrStartUp()
{
	CCompressedStream		FS("shaders.xr","xrSHADER");
	char					name[256];

	// Load constants
	{
		CStream*	fs		= FS.OpenChunk(0);
		while (!fs->Eof())	{
			fs->RstringZ	(name);
			CConstant*		C = new CConstant;
			C->Load			(fs);
			constants.insert(make_pair(strdup(name),C));
		}
		fs->Close();
	}

	// Load matrices
	{
		CStream*	fs		= FS.OpenChunk(1);
		while (!fs->Eof())	{
			fs->RstringZ	(name);
			CMatrix*		M = new CMatrix;
			M->Load			(fs);
			matrices.insert	(make_pair(strdup(name),M));
		}
		fs->Close();
	}

	// Load blenders
	{
		CStream*	fs		= FS.OpenChunk(2);
		while (!fs->Eof())	{
			CBlender_DESC	desc;
			fs->Read		(&desc,sizeof(desc));
			CBlender*		B = CBlender::Create(desc.CLS);
			fs->Seek		(fs->Tell()-sizeof(desc));
			B->Load			(fs);
			blenders.insert	(make_pair(strdup(desc.cName),B));
		}
		fs->Close();
	}
}

void CShaderManager::xrShutDown()
{
	// Release blenders
	{
		for (map<LPSTR,CBlender*>::iterator I=blenders.begin(); I!=blenders.end(); I++)
		{
			_FREE	(I->first);
			_DELETE (I->second);
		}
		blenders.clear();
	}
	// Release matrices
	{
		for (map<LPSTR,CMatrix*>::iterator I=matrices.begin(); I!=matrices.end(); I++)
		{
			_FREE	(I->first);
			_DELETE (I->second);
		}
		matrices.clear();
	}
	// Release constants
	{
		for (map<LPSTR,CConstant*>::iterator I=constants.begin(); I!=constants.end(); I++)
		{
			_FREE	(I->first);
			_DELETE (I->second);
		}
		constants.clear();
	}
}
