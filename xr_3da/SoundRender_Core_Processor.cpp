#include "stdafx.h"
#include "SoundRender_Core.h"
#include "SoundRender_Emitter.h"
#include "SoundRender_Target.h"

CSoundRender_Emitter*	CSoundRender_Core::i_play(sound* S, BOOL _loop )
{
	CSoundRender_Emitter*	E	= xr_new<CSoundRender_Emitter>();
	E->start				(S,S->handle,_loop);
}

void CSoundRender_Core::OnFrame	()
{
	u32 it;

	// Update emmitters
	for (it=0; it<s_emitters.size(); it++)
	{
		CSoundRender_Emitter*	pEmitter = s_emitters[it];
		pEmitter->update		();
		if (!pEmitter->isPlaying())		
		{
			// Stopped
			xr_delete		(pEmitter);
			s_emitters.erase(s_emitters.begin()+it);
			it--;
		}
	}

	// Get currently rendering emitters
	s_targets_defer.clear	();
	for (it=0; it<s_targets.size(); it++)
	{
		CSoundRender_Target*	T	= s_targets	[it];
		if (T->get_emitter())
		{
			// Has emmitter, maybe just not started rendering
			if		(T->get_Rendering())	
				T->update	();
			else 	
				s_targets_defer.push_back		(T);
		}
	}

	// Update listener
	Listener.vVelocity.sub				(Device.vCameraPosition, Listener.vPosition );
	Listener.vVelocity.div				(Device.fTimeDelta+EPS_S);
	Listener.vPosition.set				(Device.vCameraPosition);
	Listener.vOrientFront.set			(Device.vCameraDirection);
	Listener.vOrientTop.set				(Device.vCameraTop);
	Listener.fDopplerFactor				= psSoundDoppler;
	Listener.fRolloffFactor				= psSoundRolloff;
	pListener->SetAllParameters			((DS3DLISTENER*)&Listener, DS3D_DEFERRED );
	pListener->CommitDeferredSettings	();

	// Start rendering of pending targets
	for (it=0; it<s_targets_defer.size(); it++)
		s_targets_defer[it]->render	();
}
