//////////////////////////////////////////////////////////////////////
// HudSound.cpp:	структура для работы со звуками применяемыми в 
//					HUD-объектах (обычные звуки, но с доп. параметрами)
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "HudSound.h"

void HUD_SOUND::LoadSound(LPCSTR section, LPCSTR line, 
						 HUD_SOUND& hud_snd, BOOL _3D, 
						 int type)
{
	string128 sound_line;
	strcpy(sound_line,line);
//	LPCSTR str;
	int k=0;
	while( pSettings->line_exist(section, sound_line) ){
	hud_snd.sounds.push_back( SSnd() );
	SSnd& s = hud_snd.sounds.back();

	LoadSound(section, sound_line, s.snd, 
				_3D, type, &s.volume, &s.delay);

		hud_snd.enable = true;

		sprintf(sound_line,"%s%d",line,++k);
	}//while

}

void  HUD_SOUND::LoadSound(LPCSTR section, LPCSTR line, 
						  ref_sound& snd, BOOL _3D, 
						  int type,
						  float* volume, 
						  float* delay)
{

		LPCSTR str = pSettings->r_string(section, line);
		string256 buf_str;

		int	count = _GetItemCount	(str);
		R_ASSERT(count);

		_GetItem(str, 0, buf_str);
		snd.create(_3D, buf_str, type);


		if(volume != NULL)
		{
			*volume = 1.f;
			if(count>1)
			{
				_GetItem (str, 1, buf_str);
				if(xr_strlen(buf_str)>0)
					*volume = (float)atof(buf_str);
			}
		}

		if(delay != NULL)
		{
			*delay = 0;
			if(count>2)
			{
				_GetItem (str, 2, buf_str);
				if(xr_strlen(buf_str)>0)
					*delay = (float)atof(buf_str);
			}
		}
		
}
void HUD_SOUND::set_position(const Fvector& pos) 
{
	xr_vector<SSnd>::iterator it = sounds.begin();
	for(;it!=sounds.end();++it)
		if( (*it).snd.feedback )
			(*it).snd.set_position(pos);
	
}


void HUD_SOUND::DestroySound	(HUD_SOUND& hud_snd)
{
	xr_vector<SSnd>::iterator it = hud_snd.sounds.begin();
	for(;it!=hud_snd.sounds.end();++it)
		(*it).snd.destroy();
}

void HUD_SOUND::PlaySound	(HUD_SOUND&		hud_snd,
							const Fvector&	position,
							const CObject*	parent,
							bool			hud_mode,
							bool			looped)
{
	if(!hud_snd.enable)		return;
	StopSound(hud_snd);

	VERIFY(hud_snd.sounds.size());
//	VERIFY2					(hud_snd.snd.handle,"Trying to play non-existant or destroyed sound");

	u32 flags = hud_mode?sm_2D:0;
	if(looped)
		flags |= sm_Looped;

	SSnd& s = hud_snd.sounds[ Random.randI(hud_snd.sounds.size()) ];

	s.snd.play_at_pos	(const_cast<CObject*>(parent),
							position,
							flags,
							s.delay);
	s.snd.set_volume		(s.volume);
}

void HUD_SOUND::StopSound	(HUD_SOUND& hud_snd)
{
	if(!hud_snd.enable)		return;
	
	xr_vector<SSnd>::iterator it = hud_snd.sounds.begin();
	for(;it!=hud_snd.sounds.end();++it){
		VERIFY2					((*it).snd.handle,"Trying to stop non-existant or destroyed sound");
		(*it).snd.stop();
	}
}