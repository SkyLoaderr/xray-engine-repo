#include "stdafx.h"

void	CConstant::Load	(CStream* fs)
{
	dwMode		= modeWaveForm;
	fs->Read	(&_R,sizeof(WaveForm));
	fs->Read	(&_G,sizeof(WaveForm));
	fs->Read	(&_B,sizeof(WaveForm));
	fs->Read	(&_A,sizeof(WaveForm));
}
