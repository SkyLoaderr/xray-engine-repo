////////////////////////////////////////////////////////////////////////////
//	Module 		: script_sound.h
//	Created 	: 06.02.2004
//  Modified 	: 06.02.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay Script sound class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "script_space.h"
#include "script_engine.h"
#include "ai_sounds.h"
#include "ai_space.h"

class CLuaGameObject;

class CScriptSound {
	mutable ref_sound		m_sound;
	ref_str					m_caSoundToPlay;

	friend class CScriptSoundAction;
public:

	IC							CScriptSound		(LPCSTR caSoundName, ESoundTypes sound_type = SOUND_TYPE_NO_SOUND);
	virtual						~CScriptSound		();
	IC		u32					Length				();
	IC		void				Play				(CLuaGameObject *object);
	IC		void				Play				(CLuaGameObject *object, float delay);
			void				Play				(CLuaGameObject *object, float delay, int flags);
	IC		void				PlayUnlimited		(CLuaGameObject *object);
	IC		void				PlayUnlimited		(CLuaGameObject *object, float delay);
			void				PlayUnlimited		(CLuaGameObject *object, float delay, int flags);
	IC		void				PlayAtPos			(CLuaGameObject *object, const Fvector &position);
	IC		void				PlayAtPos			(CLuaGameObject *object, const Fvector &position, float delay);
			void				PlayAtPos			(CLuaGameObject *object, const Fvector &position, float delay, int flags);
	IC		void				PlayAtPosUnlimited	(CLuaGameObject *object, const Fvector &position);
	IC		void				PlayAtPosUnlimited	(CLuaGameObject *object, const Fvector &position, float delay);
			void				PlayAtPosUnlimited	(CLuaGameObject *object, const Fvector &position, float delay, int flags);
	IC		void				Stop				();
	IC		void				SetPosition			(const Fvector &position);
	IC		void				SetFrequency		(float frequency);
	IC		void				SetVolume			(float volume);
	IC		const CSound_params	*GetParams			();
	IC		void				SetParams			(CSound_params *sound_params);
			void				SetMinDistance		(const float fMinDistance);
	IC		void				SetMaxDistance		(const float fMaxDistance);
	IC		const Fvector		&GetPosition		() const;
	IC		const float			GetFrequency		() const;
	IC		const float			GetMinDistance		() const;
	IC		const float			GetMaxDistance		() const;
	IC		const float			GetVolume			() const;
	IC		bool				IsPlaying			() const;

	DECLARE_SCRIPT_REGISTER_FUNCTION
};
add_to_type_list(CScriptSound)
#undef script_type_list
#define script_type_list save_type_list(CScriptSound)

#include "script_sound_inline.h"