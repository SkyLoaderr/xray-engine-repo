#pragma once

#include "entity.h"


DEFINE_VECTOR(ref_shader, SHADER_VECTOR, SHADER_VECTOR_IT);
DEFINE_VECTOR(shared_str, STR_VECTOR, STR_VECTOR_IT);


class MONSTER_COMMUNITY;
class CEntityCondition;
class CWound;
class CScriptCallback;


class CEntityAlive : public CEntity {
private:
	typedef	CEntity			inherited;			
public:
	virtual CEntityAlive*				cast_entity_alive		()						{return this;}
public:

	EVENT					m_tpEventSay;
	bool					m_bMobility;
	float					m_fAccuracy;
	float					m_fIntelligence;
	//m_PhysicMovementControl
	CPHMovementControl		*m_PhysicMovementControl;
public:
	// General
							CEntityAlive			();
	virtual					~CEntityAlive			();

	// Core events
	virtual DLL_Pure		*_construct				();
	virtual void			Load					(LPCSTR section);
	virtual void			reinit					();
	virtual void			reload					(LPCSTR section);

	//object serialization
	virtual void			save					(NET_Packet &output_packet);
	virtual void			load					(IReader &input_packet);


	virtual BOOL			net_Spawn				(CSE_Abstract* DC);
	virtual void			net_Destroy				();
	virtual	BOOL			net_SaveRelevant		();

	virtual void			shedule_Update			(u32 dt);

	virtual CPHMovementControl*	movement_control()	{return m_PhysicMovementControl;}
	virtual void			HitImpulse				(float amount, Fvector& vWorldDir, Fvector& vLocalDir);
	virtual	void			Hit						(float P, Fvector &dir,			CObject* who, s16 element,Fvector position_in_object_space, float impulse, ALife::EHitType hit_type = ALife::eHitTypeWound);
	virtual void			Die						(CObject* who);
	virtual void			g_WeaponBones			(int &L, int &R1, int &R2)										= 0;
	
	virtual float GetfHealth() const;
	virtual float SetfHealth(float value);
	PropertyGP(GetfHealth,SetfHealth) float fEntityHealth;

	virtual float			g_Health			()	const;
	virtual float			g_MaxHealth			()	const;

	virtual float			CalcCondition		(float hit);

	// Visibility related
	virtual	float			ffGetFov				()	const			= 0;	
	virtual	float			ffGetRange				()	const			= 0;	
	
//	virtual void			BuyItem					(LPCSTR buf);
	virtual bool			human_being				() const
	{
		return				(false);
	}
public:
IC	CPHMovementControl* PMovement()
	{
		return m_PhysicMovementControl;
	}
///////////////////////////////////////////////////////////////////////
	virtual u16				PHGetSyncItemsNumber();
	virtual CPHSynchronize*	PHGetSyncItem		(u16 item);
	virtual void			PHUnFreeze			();
	virtual void			PHFreeze			();
///////////////////////////////////////////////////////////////////////

protected:
	//информация о партиклах крови, огня или дыма,
	//прицепляемых на местах ран
	DEFINE_VECTOR(CWound*, WOUND_VECTOR, WOUND_VECTOR_IT);
	//список ран, на которых отыгрываются партиклы
	WOUND_VECTOR m_ParticleWounds;


	virtual void StartFireParticles(CWound* pWound);
	virtual void UpdateFireParticles();
    	
	virtual void LoadFireParticles(LPCSTR section);
public:	
	static  void UnloadFireParticles	();
protected:
	//имя партиклов огня, которым может гореть EntityAlive
	static STR_VECTOR* m_pFireParticlesVector;
	//минимальное время горения
	static u32	 m_dwMinBurnTime;
	//размер раны, чтоб запустить партиклы
	static float m_fStartBurnWoundSize;
	//размер раны, чтоб остановить партиклы
	static float m_fStopBurnWoundSize;


	virtual void			BloodyWallmarks			(float P, const Fvector &dir, s16 element, const Fvector& position_in_object_space);
	static  void			LoadBloodyWallmarks		(LPCSTR section);
public:	
	static  void			UnloadBloodyWallmarks	();
protected:
	virtual void			PlaceBloodWallmark		(const Fvector& dir, const Fvector& start_pos, 
														float trace_dist, float wallmark_size,
														SHADER_VECTOR& wallmarks_vector);

	//информация о кровавых отметках на стенах, общая для всех CEntityAlive
	static SHADER_VECTOR* m_pBloodMarksVector;
	static float m_fBloodMarkSizeMax;
	static float m_fBloodMarkSizeMin;
	static float m_fBloodMarkDistance;
	static float m_fNominalHit;

	//текстурки капель крови
	static SHADER_VECTOR* m_pBloodDropsVector;
	//список ран с которых капает кровь
	
	DEFINE_VECTOR(CWound*, WOUND_VECTOR, WOUND_VECTOR_IT);
	WOUND_VECTOR m_BloodWounds;
	//размер раны, чтоб начала капать кровь
	static float m_fStartBloodWoundSize;
	//размер раны, чтоб остановить кровь
	static float m_fStopBloodWoundSize;
	static float m_fBloodDropSize;

	//обновление ран, и рисование отметок от капающей крови
	virtual void			StartBloodDrops			(CWound* pWound);
	virtual void			UpdateBloodDrops		();


	//отношения между существами и персонажами в зоне
public:
	virtual	ALife::ERelationType tfGetRelationType	(const CEntityAlive *tpEntityAlive) const;
protected:	
	MONSTER_COMMUNITY*		monster_community;

private:
	CEntityCondition			*m_entity_condition;

protected:
	virtual	CEntityCondition	*create_entity_condition	();

public:
	IC		CEntityCondition	&conditions					() const;


private:
	CScriptCallback				*m_death_script_callback;

	void						death_callback				(const CObject *who);

public:
	void						set_death_callback			(const luabind::object &lua_object, LPCSTR method);
	void						set_death_callback			(const luabind::functor<void> &lua_function);
	void						clear_death_callback		(bool member_callback);
	
protected:
	u32							m_ef_creature_type;
	u32							m_ef_weapon_type;
	u32							m_ef_detector_type;

public:
	virtual u32					ef_creature_type			() const;
	virtual u32					ef_weapon_type				() const;
	virtual u32					ef_detector_type			() const;
};

#include "entity_alive_inline.h"