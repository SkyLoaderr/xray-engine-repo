#include "stdafx.h"

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
