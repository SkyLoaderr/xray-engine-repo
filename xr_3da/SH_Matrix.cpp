#include "stdafx.h"
#pragma hdrstop

void CMatrix::Load(	CStream* fs )
{
	dwMode		= fs->Rdword	();
	tcm			= fs->Rdword	();
	fs->Read	(&scaleU,sizeof(WaveForm));
	fs->Read	(&scaleV,sizeof(WaveForm));
	fs->Read	(&rotate,sizeof(WaveForm));
	fs->Read	(&scrollU,sizeof(WaveForm));
	fs->Read	(&scrollV,sizeof(WaveForm));
}

void CMatrix::Save(	CFS_Base* fs )
{
	fs->Wdword	(dwMode);
	fs->Wdword	(tcm);
	fs->write	(&scaleU,sizeof(WaveForm));
	fs->write	(&scaleV,sizeof(WaveForm));
	fs->write	(&rotate,sizeof(WaveForm));
	fs->write	(&scrollU,sizeof(WaveForm));
	fs->write	(&scrollV,sizeof(WaveForm));
}
