////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_script_monster.h
//	Created 	: 06.10.2003
//  Modified 	: 06.10.2003
//	Author		: Dmitriy Iassenev
//	Description : Script behaviour interface for all the monsters
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "../../GameObject.h"
#include "../../visual_memory_manager.h"
#include "../../ai_script_callback.h"

class CEntityAction;

class CScriptMonster : virtual public CGameObject {
public:
	enum EActionType {
		eActionTypeMovement = u32(0),
		eActionTypeWatch,
		eActionTypeAnimation,
		eActionTypeSound,
		eActionTypeParticle,
		eActionTypeObject,
		eActionTypeCount
	};
protected:
	typedef CGameObject inherited;
	xr_deque<CEntityAction*>	m_tpActionQueue;
	bool						m_bScriptControl;
	ref_str						m_caScriptName;
	CMotionDef					*m_tpNextAnimation;
	CEntityAction				*m_tpCurrentEntityAction;
public:
	CMotionDef					*m_tpScriptAnimation;
protected:
	CScriptCallback				m_tpCallbacks[eActionTypeCount];
	CScriptCallback				m_tSoundCallback;
	CScriptCallback				m_tHitCallback;
	ref_sound					*m_current_sound;
public:
								CScriptMonster			();
	virtual						~CScriptMonster			();
			void				Init					();
	virtual	void				reinit					();
	virtual BOOL				net_Spawn				(LPVOID DC);
	virtual void				net_Destroy				();
			const Fmatrix		GetUpdatedMatrix		(ref_str caBoneName, const Fvector &tPositionOffset, const Fvector &tAngleOffset);
			void				vfUpdateParticles		();
			void				vfUpdateSounds			();
			void				vfFinishAction			(CEntityAction		*tpEntityAction);
	virtual	void				SetScriptControl		(const bool			bScriptControl, ref_str	caSciptName);
	virtual	bool				GetScriptControl		() const;
	virtual	LPCSTR				GetScriptControlName	() const;
	
	virtual bool				CheckObjectVisibility	(const CObject		*tpObject);
	virtual bool				CheckTypeVisibility		(const char* section_name);

	virtual bool				CheckIfCompleted		() const										{return false;};
	virtual	CEntityAction		*GetCurrentAction		();
	virtual	void				AddAction				(const CEntityAction*tpEntityAction, bool bHighPriority = false);
	virtual void				UseObject				(const CObject		*tpObject);
	virtual void				ProcessScripts			();
	virtual	void				ResetScriptData			(void				*P = 0);
	virtual	bool				bfAssignMovement		(CEntityAction		*tpEntityAction);
	virtual	bool				bfAssignWatch			(CEntityAction		*tpEntityAction);
	virtual	bool				bfAssignAnimation		(CEntityAction		*tpEntityAction);
	virtual	bool				bfAssignSound			(CEntityAction		*tpEntityAction);
	virtual	bool				bfAssignParticles		(CEntityAction		*tpEntityAction);
	virtual	bool				bfAssignObject			(CEntityAction		*tpEntityAction);
	virtual bool				bfAssignMonsterAction	(CEntityAction		*tpEntityAction);

	virtual void				set_callback			(const luabind::object &lua_object, LPCSTR method, const CScriptMonster::EActionType tActionType);
	virtual void				set_callback			(const luabind::functor<void> &lua_function, const CScriptMonster::EActionType tActionType);
	virtual void				clear_callback			(const CScriptMonster::EActionType tActionType);

	virtual void				set_sound_callback		(const luabind::object &lua_object, LPCSTR method);
	virtual void				set_sound_callback		(const luabind::functor<void> &lua_function);
	virtual void				clear_sound_callback	(bool member_callback);
	virtual void				sound_callback			(const CObject *object, int sound_type, const Fvector &position, float sound_power);

	virtual void				set_hit_callback		(const luabind::object &lua_object, LPCSTR method);
	virtual void				set_hit_callback		(const luabind::functor<void> &lua_function);
	virtual void				clear_hit_callback		(bool member_callback);
	virtual void				hit_callback			(float amount, const Fvector &vLocalDir, const CObject *who, s16 element);

	virtual	void				callback				(const EActionType tActionType);

	virtual LPCSTR				GetPatrolPathName		();
	virtual BOOL				UsedAI_Locations		();
	virtual void				shedule_Update			(u32				DT);
	virtual void				UpdateCL				();
			bool				bfScriptAnimation		();
			u32					GetActionCount			() const;
	const CEntityAction			*GetActionByIndex		(u32 action_index) const;

	virtual CEntity				*GetCurrentEnemy		();
	virtual CEntity				*GetCurrentCorpse		();
	virtual int					get_enemy_strength		();

	virtual void				set_visible				(bool vis);
private:
			void				FreeAll					();
};