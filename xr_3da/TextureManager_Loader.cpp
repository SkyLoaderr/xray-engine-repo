#include "stdafx.h"
#pragma hdrstop

#include "fs.h"
#include "blenders\blender.h"

void CShaderManager::xrStartUp()
{
#ifdef _EDITOR
	AnsiString sh = "shaders.xr";
    FS.m_GameRoot.Update(sh);
	if (!FS.Exist(sh.c_str())) return;
	CCompressedStream		FS(sh.c_str(),"shENGINE");
#else
	CCompressedStream		FS("shaders.xr","shENGINE");
#endif

	// Load blenders
	{
		CStream*	fs		= FS.OpenChunk	(2);
		CStream*	chunk	= NULL;
		int			chunk_id= 0;

		while ((chunk=fs->OpenChunk(chunk_id))!=NULL)
		{
			CBlender_DESC	desc;
			chunk->Read		(&desc,sizeof(desc));
			CBlender*		B = CBlender::Create(desc.CLS);
#ifdef _EDITOR
			if (B->getDescription().version!=desc.version){
                _DELETE		(B);
                ELog.DlgMsg	(mtError,"Can't load blender '%s'. Unsupported version.",desc.cName);
                chunk->Close	();
                chunk_id++;
                continue;
            }
#else
			R_ASSERT		(B->getDescription().version == desc.version);
#endif
            chunk->Seek		(0);
            B->Load			(*chunk);
			blenders.insert	(make_pair(strdup(desc.cName),B));
			chunk->Close	();
			chunk_id		+= 1;
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
