// Rain.cpp: implementation of the CEffect_Rain class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Rain.h"

const float snd_fade		= 0.1f;
const int	desired_items	= 1000;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEffect_Rain::CEffect_Rain()
{
	state				= stIdle;
	control_start		= Engine.Event.Handler_Attach	("level.weather.rain.start",this);
	control_stop		= Engine.Event.Handler_Attach	("level.weather.rain.stop",this);
	
	snd_Ambient			= pSounds->Create2D	("amb_rain");
	snd_Ambient_control	= 0;
	snd_Ambient_volume	= 0;
}

CEffect_Rain::~CEffect_Rain()
{
	Engine.Event.Handler_Detach		(control_stop,this);
	Engine.Event.Handler_Detach		(control_start,this);

	pSounds->Delete2D				(snd_Ambient);
}

void	CEffect_Rain::OnEvent	(EVENT E, DWORD P1, DWORD P2)
{
	if ((E==control_start) && (state!=stWorking))	{
		state				= stStarting;
		snd_Ambient_control	= pSounds->Play2D(snd_Ambient,TRUE);
		snd_Ambient_control->SetVolume	(snd_Ambient_volume);
	} else if ((E==control_stop) && (state!=stIdle))	{
		state				= stStopping;
	}
}

void	CEffect_Rain::Render	()
{
	// Parse states
	BOOL	bBornNewItems;
	switch (state)
	{
	case stIdle:		return;
	case stStarting:	
		snd_Ambient_volume	+= snd_fade*Device.fTimeDelta;
		snd_Ambient_control->SetVolume	(snd_Ambient_volume);
		if (snd_Ambient_volume > 1)	state=stWorking;
		bBornNewItems	= TRUE;
		break;
	case stWorking:		
		bBornNewItems	= TRUE;
		break;
	case stStopping:
		snd_Ambient_volume	-= snd_fade*Device.fTimeDelta;
		snd_Ambient_control->SetVolume	(snd_Ambient_volume);
		if (snd_Ambient_volume < 0)	{
			snd_Ambient_control->Stop	();
			state=stIdle;
		}
		bBornNewItems	= FALSE;
		break;
	}

	// Born new if needed
	float	b_radius		= 30.f;	// Need to ask from heightmap
	float	b_height		= 50.f;
	if (bBornNewItems && (items.size()<desired_items))	{
		items.reserve	(desired_items);
		while (items.size()<desired_items)	{
			Item			one;
			Born			(one,b_radius,b_height);
			items.push_back	(one);
		}
	}

	// Perform update
	for (DWORD I=0; I<items.size(); I++)
	{
		
	}
}
