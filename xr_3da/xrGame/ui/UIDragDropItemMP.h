//=============================================================================
//  Filename:   UIDragDropItemMP.h
//	Created by Roman E. Marchenko, vortex@gsc-game.kiev.ua
//	Copyright 2004. GSC Game World
//	---------------------------------------------------------------------------
//  ���������� ������������� ���� � ���� ������� ������ � �������������.
//	�� �������������� �� �������� ��������� �������������� �������
//=============================================================================

#ifndef UI_DRAG_DROP_ITEM_MP_H_
#define UI_DRAG_DROP_ITEM_MP_H_

#pragma once

#include "UIDragDropItem.h"
#include "UIDragDropList.h"

class CUIBuyWeaponWnd;

class CUIDragDropItemMP: public CUIDragDropItem
{
	friend class CUIBuyWeaponWnd;

	typedef CUIDragDropItem inherited;
	// ����������� �������� ������ �����
	// � ������� ����� ����������� ������ ����
	u32				slotNum;
	// �������� ������ ������ ������
	u32				sectionNum;
	// x������� ����������� ������ ������ � ������
	u32				posInSection;
	// ��� ������ ������
	shared_str		strName;
	// ���������� ����� "�������"
	CUIDragDropList *m_pOwner;
	// ������� ��������� ����
	int				cost;
public:
	CUIDragDropItemMP():	slotNum						(0),
							sectionNum					(0),
							bAddonsAvailable			(false),
							cost						(0),
							m_bAlreadyPaid				(false),
							m_bHasRealRepresentation	(false)
	{
		std::strcpy(m_strAddonTypeNames[0], "Silencer");
		std::strcpy(m_strAddonTypeNames[1], "Grenade Launcher");
		std::strcpy(m_strAddonTypeNames[2], "Scope");

		for (u8 i = 0; i < NUM_OF_ADDONS; ++i)
			m_pAddon[i] = NULL;
	}
	// ��� �����
	void SetSlot(int slot);
	u32	 GetSlot()								{ return slotNum; }
	// �������� ����� ������ ������ (��� - 0, ������� - 1, et cetera)
	void SetSectionGroupID(u32 section)			{ sectionNum = section; }
	u32	 GetSectionGroupID()					{ return sectionNum; }
	// ������� ��� �����������/����������� ����� ������ � .ltx �����, ���� ����
	void SetSectionName(const char *pData)		{ strName = pData; }
	const char * GetSectionName() const			{ return strName.c_str(); }
	// ����������/����������� ��������� �� CUIDragDropList �������� ���������� �����������
	// ����
	void SetOwner(CUIDragDropList *pOwner)		{ R_ASSERT(pOwner); m_pOwner = pOwner; }
	CUIDragDropList * GetOwner()				{ return m_pOwner; }
	// ����� �������� � ������ ������
	void SetPosInSectionsGroup(const u32 pos)	{ posInSection = pos; }
	u32 GetPosInSectionsGroup() const 			{ return posInSection; }

	//-----------------------------------------------------------------------------/
	//  ������ � ��������. ���������� ��������������� CWeapon ������ ����������������
	//	�������� �� �������
	//-----------------------------------------------------------------------------/

	// ��������� ���������� � ������
	typedef struct tAddonInfo
	{
		// ��� ������ ��� ������
		std::string			strAddonName;
		// -1 - ������ ������ �����������, 0 - �� ����������, 1 - ����������
		int					iAttachStatus;
		// ���������� �������� ������������ ������ ������
		int					x, y;
		// Constructor
		tAddonInfo():		iAttachStatus(-1), x(-1), y(-1) {}
	} AddonInfo;

	// � ������� ������ �������� 3 ������. ����� �������, ��� � ������� ��� ���� � ����� ������:
	// Silencer, Grenade Launcher, Scope.
	static const u8		NUM_OF_ADDONS = 3;
	enum  AddonIDs		{ ID_NONE = -1, ID_SILENCER = 0, ID_GRENADE_LAUNCHER, ID_SCOPE };
	AddonInfo			m_AddonInfo[NUM_OF_ADDONS];
	// ���� � ���� ������ ��� �������, �� ��������� ���� ������, ��� ��������� ��������
	bool				bAddonsAvailable;
	// ������ �������� ����� �������
	char				m_strAddonTypeNames[NUM_OF_ADDONS][25];

	// �������/������� ������
	void				AttachDetachAddon(AddonIDs iAddonIndex, bool bAttach, bool bRealRepresentationSet = false);
	void				AttachDetachAddon(CUIDragDropItemMP *pPossibleAddon, bool bAttach, bool bRealRepresentationSet = false);
	void				AttachDetachAllAddons(bool bAttach);
	bool				IsAddonAttached(int iAddonIndex) { return m_AddonInfo[iAddonIndex].iAttachStatus == 1; }
	// ��������� � ��� �� ��� �����?
	AddonIDs			IsOurAddon(CUIDragDropItemMP * pPossibleAddon);
	// �������������� ��������� �������, ������� ��� ����� ��� ��������� ��������� ������ � ��������
	virtual void		ClipperOn();
	virtual void		ClipperOff();
	virtual void		Draw();
	// ����� �� ������ ��������� �� ����-������
	CUIDragDropItemMP	*m_pAddon[NUM_OF_ADDONS];

	// ������ � ��������
	void				SetCost(const int c)	{ cost = c; }
	int					GetCost()	const		{ return cost; }
	// ��������� ����, ���� ��� ���������. ����� ��� ����������� �������� ����� ��� 
	// ���������� ����� ������� � ������
	bool				m_bAlreadyPaid;
	// ���� ��� �����������, ��� ������ drag drop ���� ��� ����� �������� ������� �������������
	bool				m_bHasRealRepresentation;

	// ������ �������������� ����� ������� ����� ���� ���������� ��� ������������ ����� �����,
	// �������� ��� ������� ������ ���������� ������ � ����� �������� ��� ��������������
	xr_vector<float>	m_fAdditionalInfo;
};

#endif	//UI_DRAG_DROP_ITEM_MP_H_