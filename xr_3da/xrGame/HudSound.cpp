//////////////////////////////////////////////////////////////////////
// HudSound.cpp:	��������� ��� ������ �� ������� ������������ � 
//					HUD-�������� (������� �����, �� � ���. �����������)
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "HudSound.h"

void HUD_SOUND::LoadSound(LPCSTR section, LPCSTR line, 
						 HUD_SOUND& hud_snd, BOOL _3D, 
						 int type)
{
	LoadSound(section, line, hud_snd.snd, 
				_3D, type, &hud_snd.volume, &hud_snd.delay);
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

void HUD_SOUND::DestroySound	(HUD_SOUND& hud_snd)
{
	hud_snd.snd.destroy();
}

void HUD_SOUND::PlaySound(HUD_SOUND&		hud_snd,
						  const Fvector&	position,
						  const CObject*	parent,
						  bool				hud_mode)
{
	hud_snd.snd.set_volume	(hud_snd.volume);
	hud_snd.snd.play_at_pos	(const_cast<CObject*>(parent),
							position,
							hud_mode?sm_2D:0,
							hud_snd.delay);
}



