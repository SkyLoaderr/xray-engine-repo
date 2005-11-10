#pragma once

class CUIXml;

class CMMSound {
public:
	CMMSound();
	~CMMSound();
	void Init(CUIXml& xml_doc, LPCSTR path);
	void whell_Play();
	void whell_Stop();
	void whell_UpdateMoving(float frequency);

	void music_Play();
	void music_Stop();
	void music_Update();

protected:
	ref_sound		m_music;
	ref_sound		m_whel;
	bool			m_bRandom;
	xr_vector<xr_string>	m_play_list;
};
