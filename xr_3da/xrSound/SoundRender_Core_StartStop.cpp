#include "stdafx.h"
#include "SoundRender_Core.h"
#include "SoundRender_Emitter.h"
#include "SoundRender_Target.h"

void	CSoundRender_Core::i_start		(CSoundRender_Emitter* E)
{
	R_ASSERT	(E);

	// Search available target
	CSoundRender_Target*	T	= 0;
	for (u32 it=0; it<s_targets.size(); it++)
	{
		if (0==s_targets[it]->get_emitter())	
		{
			T = s_targets[it];
			break;
		}
	}
	
	// If not found - create new
	if (0==T)
	{
		T					=	xr_new<CSoundRender_Target>();
		T->_initialize		();
		s_targets.push_back	(T);
	}

	// Associate
	E->target			= T;
	T->start			(E);
}

void	CSoundRender_Core::i_stop		(CSoundRender_Emitter* E)
{
	R_ASSERT			(E);
	R_ASSERT			(E == E->target->get_emitter());
	E->target->stop		();
	E->target			= NULL;
}

void	CSoundRender_Core::i_rewind		(CSoundRender_Emitter* E)
{
	R_ASSERT			(E);
	R_ASSERT			(E == E->target->get_emitter());
	E->target->rewind	();
}
