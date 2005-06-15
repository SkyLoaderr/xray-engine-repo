#include "stdafx.h"
#pragma hdrstop

#include "level.h"
#include "level_sounds.h"

void SStaticSound::Load(IReader& F)
{
	R_ASSERT				(F.find_chunk(0));
	xr_string				wav_name;
	F.r_stringZ				(wav_name);
	m_Source.create			(TRUE,wav_name.c_str());
	F.r_fvector3			(m_Position);
	m_Volume				= F.r_float();
	m_Freq					= F.r_float();
	m_ActiveTime.x			= F.r_u32();
	m_ActiveTime.y			= F.r_u32();
	m_PlayTime.x			= F.r_u32();
	m_PlayTime.y			= F.r_u32();
	m_PauseTime.x			= F.r_u32();
	m_PauseTime.y			= F.r_u32();
	m_NextTime				= 0;
	m_StopTime				= 0;
}

void SStaticSound::Update(u32 game_time, u32 global_time)
{
	if ((0==m_ActiveTime.x)&&(0==m_ActiveTime.y)||((int(game_time)>m_ActiveTime.x)&&(int(game_time)<m_ActiveTime.y))){
		if (0==m_Source.feedback){
			if ((0==m_PauseTime.x)&&(0==m_PauseTime.y)){    
				m_Source.play_at_pos	(0,m_Position,sm_Looped);
				m_Source.set_volume		(m_Volume);
				m_Source.set_frequency	(m_Freq);
				m_StopTime				= 0xFFFFFFFF;
			}else{
				if (global_time>=m_NextTime){
					bool bFullPlay		= (0==m_PlayTime.x)&&(0==m_PlayTime.y);
					m_Source.play_at_pos	(0,m_Position,bFullPlay?0:sm_Looped);
					m_Source.set_volume		(m_Volume);
					m_Source.set_frequency	(m_Freq);
					if (bFullPlay){
						m_StopTime		= 0xFFFFFFFF;
						m_NextTime		= global_time+m_Source.handle->length_ms()+Random.randI(m_PauseTime.x,m_PauseTime.y);
					}else{
						m_StopTime		= bFullPlay?0:global_time+Random.randI(m_PlayTime.x,m_PlayTime.y);
						m_NextTime		= m_StopTime+Random.randI(m_PauseTime.x,m_PauseTime.y);
					}
				}
			}
		}else{
			if (Device.dwTimeGlobal>=m_StopTime)
				m_Source.stop_deffered();
		}

	}else{
		if (0!=m_Source.feedback)
			m_Source.stop_deffered();
	}
}

void CStaticSoundManager::Load()
{
 	VERIFY(m_StaticSounds.empty());
	string_path fn;
	if (FS.exist(fn, "$level$", "level.snd_static")) {
		IReader *F		= FS.r_open	(fn);
		u32				chunk = 0;
		for (IReader *OBJ = F->open_chunk_iterator(chunk); OBJ; OBJ = F->open_chunk_iterator(chunk,OBJ)) {
			m_StaticSounds.push_back	(SStaticSound());
			m_StaticSounds.back().Load	(*OBJ);
		}
		FS.r_close				(F);
	}
}

void CStaticSoundManager::Unload()
{
	m_StaticSounds.clear		();
}

void CStaticSoundManager::Update()
{
	u32 game_time				= iFloor(Level().GetGameDayTimeMS());
	u32 global_time				= Device.dwTimeGlobal;
	
	for (int k=0; k<m_StaticSounds.size(); ++k){
		SStaticSound& s			= m_StaticSounds[k];
		s.Update				(game_time,global_time);
	}
}
