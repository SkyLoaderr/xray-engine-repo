////////////////////////////////////////////////////////////////////////////
//	Module 		: script_game_object_script2.cpp
//	Created 	: 17.11.2004
//  Modified 	: 17.11.2004
//	Author		: Dmitriy Iassenev
//	Description : Script game object class script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_game_object.h"
#include "script_game_object_impl.h"
#include "ai_space.h"
#include "script_engine.h"

#include "explosive.h"
#include "script_zone.h"
#include "object_handler.h"
#include "script_hit.h"
#include "../skeletoncustom.h"
#include "pda.h"
#include "InfoPortion.h"
#include "memory_manager.h"
#include "ai_phrasedialogmanager.h"

void CScriptGameObject::explode	(u32 level_time)
{
	CExplosive			*explosive = smart_cast<CExplosive*>(object());
	if (object()->H_Parent())
	{
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CExplosive : cannot explode object wiht parent!");
		return;
	}

	if (!explosive)
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CExplosive : cannot access class member explode!");
	else {
		Fvector normal;
		explosive->FindNormal(normal);
		explosive->SetCurrentParentID(object()->ID());
		explosive->GenExplodeEvent(object()->Position(), normal);
	}
}

bool CScriptGameObject::active_zone_contact		(u16 id)
{
	CScriptZone		*script_zone = smart_cast<CScriptZone*>(object());
	if (!script_zone) {
		ai().script_engine().script_log	(ScriptStorage::eLuaMessageTypeError,"CScriptZone : cannot access class member active_zone_contact!");
		return		(false);
	}
	return			(script_zone->active_contact(id));
}

CScriptGameObject *CScriptGameObject::best_weapon()
{
	CObjectHandler	*object_handler = smart_cast<CObjectHandler*>(object());
	if (!object_handler) {
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CScriptMonster : cannot access class member best_weapon!");
		return			(0);
	}
	else {
		CGameObject		*game_object = object_handler->best_weapon();
		return			(game_object ? game_object->lua_game_object() : 0);
	}
}

void CScriptGameObject::set_item		(MonsterSpace::EObjectAction object_action)
{
	CObjectHandler			*object_handler = smart_cast<CObjectHandler*>(object());
	if (!object_handler)
		ai().script_engine().script_log					(ScriptStorage::eLuaMessageTypeError,"CObjectHandler : cannot access class member set_item!");
	else
		object_handler->set_goal(object_action);
}

void CScriptGameObject::set_item		(MonsterSpace::EObjectAction object_action, CScriptGameObject *lua_game_object)
{
	CObjectHandler			*object_handler = smart_cast<CObjectHandler*>(object());
	if (!object_handler)
		ai().script_engine().script_log					(ScriptStorage::eLuaMessageTypeError,"CObjectHandler : cannot access class member set_item!");
	else
		object_handler->set_goal(object_action,lua_game_object ? lua_game_object->object() : 0);
}

void CScriptGameObject::set_item(MonsterSpace::EObjectAction object_action, CScriptGameObject *lua_game_object, u32 queue_size)
{
	CObjectHandler			*object_handler = smart_cast<CObjectHandler*>(object());
	if (!object_handler)
		ai().script_engine().script_log					(ScriptStorage::eLuaMessageTypeError,"CObjectHandler : cannot access class member set_item!");
	else
		object_handler->set_goal(object_action,lua_game_object ? lua_game_object->object() : 0, queue_size);
}

void CScriptGameObject::set_item(MonsterSpace::EObjectAction object_action, CScriptGameObject *lua_game_object, u32 queue_size, u32 queue_interval)
{
	CObjectHandler			*object_handler = smart_cast<CObjectHandler*>(object());
	if (!object_handler)
		ai().script_engine().script_log					(ScriptStorage::eLuaMessageTypeError,"CObjectHandler : cannot access class member set_item!");
	else
		object_handler->set_goal(object_action,lua_game_object ? lua_game_object->object() : 0, queue_size, queue_interval);
}

void CScriptGameObject::Hit(CScriptHit *tpLuaHit)
{
	CScriptHit		&tLuaHit = *tpLuaHit;
	NET_Packet		P;
	object()->u_EventGen(P,GE_HIT,object()->ID());
	P.w_u16			(u16(tLuaHit.m_tpDraftsman->ID()));
	P.w_u16			(0);
	P.w_dir			(tLuaHit.m_tDirection);
	P.w_float		(tLuaHit.m_fPower);
	CKinematics		*V = smart_cast<CKinematics*>(object()->Visual());
	VERIFY			(V);
	if (xr_strlen	(tLuaHit.m_caBoneName))
		P.w_s16		(V->LL_BoneID(tLuaHit.m_caBoneName));
	else
		P.w_s16		(s16(0));
	P.w_vec3		(Fvector().set(0,0,0));
	P.w_float		(tLuaHit.m_fImpulse);
	P.w_u16			(u16(tLuaHit.m_tHitType));
	object()->u_EventSend(P);
}

#pragma todo("Dima to Dima : find out why user defined conversion operators work incorrect")

CScriptGameObject::operator CObject*()
{
	return			(object());
}

void		CScriptGameObject::set_character_pda_info	(LPCSTR info_id)
{
	CInventoryOwner	 *pInventoryOwner = smart_cast<CInventoryOwner*>(object()); VERIFY(pInventoryOwner);
	CPda* pda = pInventoryOwner->GetPDA(); VERIFY(pda);
	pda->SetInfoPortion(CInfoPortion::IdToIndex(info_id));
}

LPCSTR		CScriptGameObject::get_character_pda_info	()
{
	CInventoryOwner	 *pInventoryOwner = smart_cast<CInventoryOwner*>(object()); VERIFY(pInventoryOwner);
	CPda* pda = pInventoryOwner->GetPDA(); VERIFY(pda);
	return *CInfoPortion::IndexToId(pda->GetInfoPortion());
}

void		CScriptGameObject::set_pda_info				(LPCSTR info_id)
{
	CPda* pda = smart_cast<CPda*>(object()); VERIFY(pda);
	pda->SetInfoPortion(CInfoPortion::IdToIndex(info_id));
}

LPCSTR		CScriptGameObject::get_pda_info				()
{
	CPda* pda = smart_cast<CPda*>(object()); VERIFY(pda);
	return *CInfoPortion::IndexToId(pda->GetInfoPortion());
}

const CHitObject *CScriptGameObject::GetBestHit	() const
{
	const CHitMemoryManager	*hit_memory_manager = smart_cast<const CHitMemoryManager*>(object());
	if (!hit_memory_manager)
		return				(0);
	return					(hit_memory_manager->hit());
}

const CSoundObject *CScriptGameObject::GetBestSound	() const
{
	const CSoundMemoryManager	*sound_memory_manager = smart_cast<const CSoundMemoryManager*>(object());
	if (!sound_memory_manager)
		return				(0);
	return					(sound_memory_manager->sound());
}

CScriptGameObject *CScriptGameObject::GetBestEnemy()
{
	const CEnemyManager		*enemy_manager = smart_cast<const CEnemyManager*>(object());
	if (!enemy_manager || !enemy_manager->selected())
		return				(0);
	return					(smart_cast<const CGameObject*>(enemy_manager->selected())->lua_game_object());
}

CScriptGameObject *CScriptGameObject::GetBestItem()
{
	const CItemManager		*item_manager = smart_cast<const CItemManager*>(object());
	if (!item_manager || !item_manager->selected())
		return				(0);
	return					(smart_cast<const CGameObject*>(item_manager->selected())->lua_game_object());
}

MemorySpace::CMemoryInfo *CScriptGameObject::memory(const CScriptGameObject &lua_game_object)
{
	CMemoryManager	*memory_manager = smart_cast<CMemoryManager*>(object());
	if (!memory_manager) {
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CScriptMonster : cannot access class member memory!");
		return			(0);
	}
	else
		return			(xr_new<MemorySpace::CMemoryInfo>(memory_manager->memory(lua_game_object.object())));
}

void CScriptGameObject::enable_memory_object	(CScriptGameObject *game_object, bool enable)
{
	CMemoryManager	*memory_manager = smart_cast<CMemoryManager*>(object());
	if (!memory_manager)
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CGameObject : cannot access class member enable_memory_object!");
	else
		memory_manager->enable				(game_object->object(),enable);
}

const xr_vector<CNotYetVisibleObject> &CScriptGameObject::not_yet_visible_objects() const
{
	CMemoryManager	*manager = smart_cast<CMemoryManager*>(object());
	if (!manager) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CGameObject : cannot access class member not_yet_visible_objects!");
		NODEFAULT;
	}
	return					(manager->not_yet_visible_objects());
}

float CScriptGameObject::visibility_threshold	() const
{
	CMemoryManager	*manager = smart_cast<CMemoryManager*>(object());
	if (!manager) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CGameObject : cannot access class member visibility_threshold!");
		NODEFAULT;
	}
	return					(manager->visibility_threshold());
}

void CScriptGameObject::enable_vision			(bool value)
{
	CVisualMemoryManager					*visual_memory_manager = smart_cast<CVisualMemoryManager*>(object());
	if (!visual_memory_manager) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CVisualMemoryManager : cannot access class member enable_vision!");
		return;
	}
	visual_memory_manager->enable			(value);
}

bool CScriptGameObject::vision_enabled			() const
{
	CVisualMemoryManager					*visual_memory_manager = smart_cast<CVisualMemoryManager*>(object());
	if (!visual_memory_manager) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CVisualMemoryManager : cannot access class member vision_enabled!");
		return								(false);
	}
	return									(visual_memory_manager->enabled());
}

void CScriptGameObject::set_sound_threshold		(float value)
{
	CSoundMemoryManager						*sound_memory_manager = smart_cast<CSoundMemoryManager*>(object());
	if (!sound_memory_manager) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CSoundMemoryManager : cannot access class member set_sound_threshold!");
		return;
	}
	sound_memory_manager->set_threshold		(value);
}

void CScriptGameObject::restore_sound_threshold	()
{
	CSoundMemoryManager						*sound_memory_manager = smart_cast<CSoundMemoryManager*>(object());
	if (!sound_memory_manager) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CSoundMemoryManager : cannot access class member restore_sound_threshold!");
		return;
	}
	sound_memory_manager->restore_threshold	();
}

bool CScriptGameObject::NeedToAnswerPda		()
{
	CAI_PhraseDialogManager* pDialogManager = smart_cast<CAI_PhraseDialogManager*>(object());
	if(!pDialogManager) return false;
	return pDialogManager->NeedAnswerOnPDA();
}
void CScriptGameObject::AnswerPda			()
{
	CAI_PhraseDialogManager* pDialogManager = smart_cast<CAI_PhraseDialogManager*>(object());
	if(!pDialogManager) return;
	pDialogManager->AnswerOnPDA();
}

void CScriptGameObject::SetStartDialog(LPCSTR dialog_id)
{
	CAI_PhraseDialogManager* pDialogManager = smart_cast<CAI_PhraseDialogManager*>(object());
	if(!pDialogManager) return;
	pDialogManager->SetStartDialog(dialog_id);
}

void CScriptGameObject::GetStartDialog		()
{
	CAI_PhraseDialogManager* pDialogManager = smart_cast<CAI_PhraseDialogManager*>(object());
	if(!pDialogManager) return;
	pDialogManager->GetStartDialog();
}
void CScriptGameObject::RestoreDefaultStartDialog()
{
	CAI_PhraseDialogManager* pDialogManager = smart_cast<CAI_PhraseDialogManager*>(object());
	if(!pDialogManager) return;
	pDialogManager->RestoreDefaultStartDialog();
}
