#include "stdafx.h"

void	CConstant::Load	(CStream* fs)
{
	dwMode		= modeWaveForm;
	fs->read	(&_R,sizeof(WaveForm));
	fs->read	(&_G,sizeof(WaveForm));
	fs->read	(&_B,sizeof(WaveForm));
	fs->read	(&_A,sizeof(WaveForm));
}
