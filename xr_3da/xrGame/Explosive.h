// Explosive.h: интерфейс для взврывающихся объектов
//
//////////////////////////////////////////////////////////////////////

#pragma once

#define SND_RIC_COUNT 5
#define EXPLODE_TIME_MAX 5000


#include "../feel_touch.h"
#include "inventory_item.h"

class CExplosive:
	virtual public CInventoryItem,
    public Feel::Touch
{
private:
	typedef CInventoryItem inherited;
public:
	CExplosive(void);
	virtual ~CExplosive(void);

	virtual void Load(LPCSTR section);
	
	virtual BOOL net_Spawn			(LPVOID DC);
	virtual void net_Destroy		();
	virtual void net_Export			(NET_Packet& P) = 0;	// export to server
	virtual void net_Import			(NET_Packet& P) = 0;	// import from server


	virtual void UpdateCL();

	virtual void feel_touch_new(CObject* O);

	virtual void Explode();

	virtual void OnH_B_Chield		() {inherited::OnH_B_Chield();}
	virtual void OnH_B_Independent	() {inherited::OnH_B_Independent();}
	virtual void OnEvent (NET_Packet& P, u16 type) ;//{inherited::OnEvent( P, type);}
	virtual	void Hit	(float P, Fvector &dir,	CObject* who, s16 element,
						 Fvector position_in_object_space, float impulse, 
						 ALife::EHitType hit_type = ALife::eHitTypeWound)	{inherited::Hit(P, dir, who, element, position_in_object_space,impulse,hit_type);}
	
	virtual void renderable_Render	();



protected:
	//ID персонажа который иницировал действие
	u16			m_iCurrentParentID;

	//параметры взрыва
	float m_fBlastHit;
	float m_fBlastRadius;
	
	//параметры и количество осколков
	float m_fFragsRadius; 
	float m_fFragHit;
	int	  m_iFragsNum;

	//типы наносимых хитов
	ALife::EHitType m_eHitTypeBlast;
	ALife::EHitType m_eHitTypeFrag;

	//коэффицент, сколько из нанесенного взрывом хита
	//пойдет на физический импульс
	float m_fBlastImpulseFactor;


	//фактор подпроса предмета вверх взрывной волной 
	float m_fUpThrowFactor;

	//список пораженных объектов
	xr_list<CGameObject*> m_blasted;

	//продолжительность взрыва
	u32	m_dwExplodeDuration;
	//время взрыва
	u32 m_dwExplodeDurationMax;

	//////////////////////////////////////////////
	//для разлета осколков
	float					tracerHeadSpeed;
	float					tracerMaxLength;

	//звуки
	ref_sound	sndExplode;
	ESoundTypes m_eSoundExplode;

	//размер отметки на стенах
	float		fWallmarkSize;
	
	//эффекты и подсветка
	ref_str		m_sExplodeParticles;
	
	//подсветка взрыва
	IRender_Light*		m_pLight;
	Fcolor				m_LightColor;
	float				m_fLightRange;
	u32					m_dwLightTime;

	// эффектор
	struct {
		float time;
		float amplitude;	
		float period_number;
	} effector;

public:	 

	virtual void			make_Interpolation	();
	virtual void			PH_B_CrPr			(); // actions & operations before physic correction-prediction steps
	virtual void			PH_I_CrPr			(); // actions & operations after correction before prediction steps
	virtual void			PH_A_CrPr			(); // actions & operations after phisic correction-prediction steps
	virtual void			OnH_A_Chield		();
	virtual	void			reinit				();
	virtual	void			reload				(LPCSTR section);
	virtual void			create_physic_shell	();
	virtual void			activate_physic_shell();
	virtual void			setup_physic_shell	();
};
