#include "stdafx.h"
#pragma hdrstop

void	CConstant::Load	(CStream* fs)
{
	dwMode		= modeWaveForm;
	fs->Read	(&_R,sizeof(WaveForm));
	fs->Read	(&_G,sizeof(WaveForm));
	fs->Read	(&_B,sizeof(WaveForm));
	fs->Read	(&_A,sizeof(WaveForm));
}

void	CConstant::Save	(CFS_Base* fs)
{
	fs->write	(&_R,sizeof(WaveForm));
	fs->write	(&_G,sizeof(WaveForm));
	fs->write	(&_B,sizeof(WaveForm));
	fs->write	(&_A,sizeof(WaveForm));
}

