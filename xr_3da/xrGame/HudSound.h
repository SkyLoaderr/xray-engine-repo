//////////////////////////////////////////////////////////////////////
// HudSound.h:		��������� ��� ������ �� ������� ������������ � 
//					HUD-�������� (������� �����, �� � ���. �����������)
//////////////////////////////////////////////////////////////////////

#pragma once


struct HUD_SOUND
{
	HUD_SOUND() {enable = false;}


	////////////////////////////////////
	// ������ �� �������
	/////////////////////////////////////
	static void		LoadSound	(LPCSTR section, LPCSTR line,
		ref_sound& hud_snd, BOOL _3D,
		int type = st_SourceType,
		float* volume = NULL,
		float* delay = NULL);

	static void		LoadSound	(LPCSTR section, LPCSTR line,
		HUD_SOUND& hud_snd, BOOL _3D,
		int type = st_SourceType);

	static void		DestroySound (HUD_SOUND& hud_snd);

	static void		PlaySound	(HUD_SOUND& snd,
								const Fvector& position,
								const CObject* parent,
								bool hud_mode,
								bool looped = false);

	static void		StopSound	(HUD_SOUND& snd);


	void set_position(const Fvector& pos);

	struct SSnd{
		ref_sound	snd;
		float		delay;	//�������� ����� �������������
		float		volume;	//���������
	};
	xr_vector<SSnd> sounds;
	
	bool  enable;			//������������ ������� �� ����
};

