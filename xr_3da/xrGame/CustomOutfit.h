///////////////////////////////////////////////////////////////
// CustomOutfit.h
// CustomOutfit - �������� ������� ��� ����������
///////////////////////////////////////////////////////////////


#pragma once

#include "inventory_item_object.h"

struct SBoneProtections;

class CCustomOutfit: public CInventoryItemObject {
private:
    typedef	CInventoryItemObject inherited;
public:
	CCustomOutfit(void);
	virtual ~CCustomOutfit(void);

	virtual BOOL net_Spawn			(CSE_Abstract* DC);
	virtual void Load				(LPCSTR section);
	virtual void net_Destroy		();
	virtual void shedule_Update		(u32 dt);
	virtual void UpdateCL			();
	virtual void renderable_Render	();

	virtual void OnH_A_Chield		();
	virtual void OnH_B_Independent	();

	virtual	void Hit (float P, Fvector &dir,	
					  CObject* who, s16 element,
					  Fvector position_in_object_space, 
					  float impulse, 
					  ALife::EHitType hit_type = ALife::eHitTypeWound);

	//����������� ������ ����, ��� ������, ����� ������ ����� �� ���������
	virtual void Hit (float P, ALife::EHitType hit_type);

	//������������ �� ������� ����������� ���
	//��� ��������������� ���� �����������
	//���� �� ��������� ����� ������
	float GetHitTypeProtection(ALife::EHitType hit_type, s16 element);
	float GetDefHitTypeProtection(ALife::EHitType hit_type);

	float HitThruArmour(float hit_power, s16 element);
	//����������� �� ������� ����������� ������ ����
	//���� �� ��������� ����� ������
	float GetPowerLoss();

	int GetIconX() {return m_iOutfitIconX;}
	int GetIconY() {return m_iOutfitIconY;}

	virtual void	OnMoveToSlot		();
	virtual void	OnMoveToRuck		();

protected:
	int m_iOutfitIconX;
	int m_iOutfitIconY;

	HitImmunity::HitTypeSVec	m_HitTypeProtection;
	float						m_fPowerLoss;

	shared_str					m_ActorVisual;
	SBoneProtections*			m_boneProtection;
	float						m_fHitFrac;
protected:
	u32							m_ef_equipment_type;

public:
	virtual u32		ef_equipment_type	() const;
};
