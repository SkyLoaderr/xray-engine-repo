// Explosive.h: интерфейс для взврывающихся объектов
//
//////////////////////////////////////////////////////////////////////

#pragma once

#define SND_RIC_COUNT 5

#include "../Render.h"
#include "../feel_touch.h"
#include "inventory_item.h"
#include "ai_sounds.h"
#include "script_export_space.h"

class IRender_Light;

class CExplosive : public Feel::Touch {

public:
								CExplosive(void);
	virtual						~CExplosive(void);

	virtual void 				Load(LPCSTR section);
	virtual void				Load(CInifile *ini,LPCSTR section);

	virtual void 				net_Destroy		();

	virtual void 				UpdateCL();

	virtual void 				feel_touch_new(CObject* O);

	virtual void 				Explode();
	virtual void 				ExplodeParams(const Fvector& pos, const Fvector& dir);

	static float 				ExplosionEffect(CGameObject* pExpObject,  const Fvector &expl_centre, const float expl_radius, xr_list<s16> &elements, xr_list<Fvector> &bs_positions);


	virtual void 				OnEvent (NET_Packet& P, u16 type) ;//{inherited::OnEvent( P, type);}
	virtual void				OnAfterExplosion();
	virtual void				OnBeforeExplosion();
	virtual void 				SetCurrentParentID	(u16 parent_id) {m_iCurrentParentID = parent_id;}
	IC		u16 				CurrentParentID		() const {return m_iCurrentParentID;}

	virtual void 				GenExplodeEvent (const Fvector& pos, const Fvector& normal);
	virtual void 				FindNormal(Fvector& normal);
	virtual CGameObject			*cast_game_object()=0;
	virtual CExplosive*			cast_explosive(){return this;}
protected:
	//ID персонажа который иницировал действие
	u16 m_iCurrentParentID;
	
	bool	m_bReadyToExplode;
	Fvector m_vExplodePos;
	Fvector m_vExplodeDir;

	//параметры взрыва
	float m_fBlastHit;
	float m_fBlastHitImpulse;
	float m_fBlastRadius;
	
	//параметры и количество осколков
	float m_fFragsRadius; 
	float m_fFragHit;
	float m_fFragHitImpulse;
	int	  m_iFragsNum;

	//типы наносимых хитов
	ALife::EHitType m_eHitTypeBlast;
	ALife::EHitType m_eHitTypeFrag;

	//фактор подпроса предмета вверх взрывной волной 
	float m_fUpThrowFactor;

	//список пораженных объектов
	xr_list<CGameObject*> m_blasted;

	//текущая продолжительность взрыва
	float m_fExplodeDuration;
	//общее время взрыва
	float m_fExplodeDurationMax;
	//флаг состояния взрыва
	bool  m_bExploding;
	bool m_bExplodeEventSent;

	//////////////////////////////////////////////
	//для разлета осколков
	float				tracerHeadSpeed;
	float				tracerMaxLength;

	//звуки
	ref_sound			sndExplode;
	ESoundTypes			m_eSoundExplode;

	//размер отметки на стенах
	float				fWallmarkSize;
	
	//эффекты и подсветка
	shared_str			m_sExplodeParticles;
	
	//подсветка взрыва
	ref_light			m_pLight;
	Fcolor				m_LightColor;
	float				m_fLightRange;
	float				m_fLightTime;
	
	virtual	void		StartLight	();
	virtual	void		StopLight	();

	// эффектор
	struct {
		float time;
		float amplitude;	
		float period_number;
	} effector;
	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CExplosive)
#undef script_type_list
#define script_type_list save_type_list(CExplosive)