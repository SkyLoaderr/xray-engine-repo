#include "stdafx.h"
#pragma hdrstop

void	CConstant::Load	(IReader* fs)
{
	dwMode		= modeWaveForm;
	fs->r		(&_R,sizeof(WaveForm));
	fs->r		(&_G,sizeof(WaveForm));
	fs->r		(&_B,sizeof(WaveForm));
	fs->r		(&_A,sizeof(WaveForm));
}

void	CConstant::Save	(IWriter* fs)
{
	fs->w		(&_R,sizeof(WaveForm));
	fs->w		(&_G,sizeof(WaveForm));
	fs->w		(&_B,sizeof(WaveForm));
	fs->w		(&_A,sizeof(WaveForm));
}

