///////////////////////////////////////////////////////////////
// CustomOutfit.h
// CustomOutfit - �������� ������� ��� ����������
///////////////////////////////////////////////////////////////


#pragma once

#include "inventory.h"

class CCustomOutfit: public CInventoryItem
{
private:
    typedef	CInventoryItem inherited;
public:
	CCustomOutfit(void);
	virtual ~CCustomOutfit(void);

	virtual BOOL net_Spawn			(LPVOID DC);
	virtual void Load				(LPCSTR section);
	virtual void net_Destroy		();
	virtual void shedule_Update		(u32 dt);
	virtual void UpdateCL			();
	virtual void renderable_Render	();

	virtual void OnH_A_Chield		();
	virtual void OnH_B_Independent	();


	//������������ �� ������� ����������� ���
	//��� ��������������� ���� �����������
	//���� �� ��������� ����� ������
	float m_fOutfitBurn;
	float m_fOutfitStrike;
	float m_fOutfitShock;
	float m_fOutfitWound;
	float m_fOutfitRadiation;
	float m_fOutfitTelepatic;

};
