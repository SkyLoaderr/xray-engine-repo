////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_script_sound.h
//	Created 	: 06.02.2004
//  Modified 	: 06.02.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay Script sound class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ai_script_space.h"
#include "ai_script_bind.h"

class CLuaGameObject;

class CLuaSound {
	ref_sound				*m_tpSound;
	ref_str					m_caSoundToPlay;

	friend class CSoundAction;
public:

								CLuaSound		(LPCSTR caSoundName, ESoundTypes sound_type = SOUND_TYPE_NO_SOUND)
	{
		m_caSoundToPlay		= caSoundName;
		string256			l_caFileName;
		if (FS.exist(l_caFileName,"$game_sounds$",caSoundName,".ogg"))
			::Sound->create	(*(m_tpSound = xr_new<ref_sound>()),TRUE,caSoundName,sound_type);
		else {
			LuaOut			(Lua::eLuaMessageTypeError,"File not found \"%s\"!",l_caFileName);
			m_tpSound		= 0;
		}
	}

	virtual						~CLuaSound		()
	{
		xr_delete			(m_tpSound);
	}

			u32					Length			()
	{
		VERIFY				(m_tpSound);
		VERIFY				(m_tpSound->handle);
		return				(m_tpSound->handle->length_ms());
	}

			void				Play			(CLuaGameObject *object, float delay = 0.f);
			void				PlayUnlimited	(CLuaGameObject *object, float delay = 0.f);
			void				PlayAtPos		(CLuaGameObject *object, const Fvector &position, float delay = 0.f);
			void				PlayAtPosUnlimited(CLuaGameObject *object, const Fvector &position, float delay = 0.f);

	BIND_FUNCTION00	(m_tpSound,			Stop,				ref_sound,		stop);
	BIND_FUNCTION01	(m_tpSound,			SetPosition,		ref_sound,		set_position,		const Fvector &,		const Fvector &);
	BIND_FUNCTION01	(m_tpSound,			SetFrequency,		ref_sound,		set_frequency,		const float,			const float);
	BIND_FUNCTION01	(m_tpSound,			SetVolume,			ref_sound,		set_volume,			const float,			const float);
	BIND_FUNCTION10	(m_tpSound,			GetParams,			ref_sound,		get_params,			const CSound_params *,	0);
	BIND_FUNCTION01	(m_tpSound,			SetParams,			ref_sound,		set_params,			CSound_params *,		CSound_params *);

			 void				SetMinDistance	(const float fMinDistance)
	{
		VERIFY				(m_tpSound);
		m_tpSound->set_range(fMinDistance,GetMaxDistance());
	}

			 void				SetMaxDistance	(const float fMaxDistance)
	{
		VERIFY				(m_tpSound);
		m_tpSound->set_range(GetMinDistance(),fMaxDistance);
	}

			 const Fvector		&GetPosition	() const
	{
		VERIFY				(m_tpSound);
		const CSound_params	*l_tpSoundParams = m_tpSound->get_params();
		if (l_tpSoundParams)
			return			(l_tpSoundParams->position);
		else {
			LuaOut			(Lua::eLuaMessageTypeError,"Sound was not launched, can't get position!");
			return			(Fvector().set(0,0,0));
		}
	}

			 const float		GetFrequency	() const
	{
		VERIFY				(m_tpSound);
		return				(m_tpSound->get_params()->freq);
	}

			 const float		GetMinDistance	() const
	{
		VERIFY				(m_tpSound);
		return				(m_tpSound->get_params()->min_distance);
	}

			 const float		GetMaxDistance	() const
	{
		VERIFY				(m_tpSound);
		return				(m_tpSound->get_params()->max_distance);
	}

			 const float		GetVolume		() const
	{
		VERIFY				(m_tpSound);
		return				(m_tpSound->get_params()->volume);
	}

			bool				IsPlaying		() const
	{
		VERIFY				(m_tpSound);
		return				(!!m_tpSound->feedback);
	}
};

