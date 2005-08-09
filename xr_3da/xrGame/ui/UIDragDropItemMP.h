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

void WpnDrawIndex(CUIDragDropItem *pDDItem);


class CUIBuyWeaponWnd;

class CUIDragDropItemMP: public CUIDragDropItem
{
	friend class CUIBuyWeaponWnd;
	friend class CUIBag;

	typedef CUIDragDropItem inherited;
	// ����������� �������� ������ �����
	// � ������� ����� ����������� ������ ����
	u32				slotNum;
	// �������� ������ ������ ������
	u32				sectionNum;
	// x������� ����������� ������ ������ � ������
	u32				posInSection;
	u32				posInSubSection;
	// ��� ������ ������
	shared_str		strName;
	// ���������� ����� "�������"
	CUIDragDropList *m_pOwner;
	// ������� ��������� ����
	int				cost;

	bool			m_bIsInfinite;
public:
		CUIDragDropItemMP						();
	// ��� �����
	void SetSlot								(int slot);
	u32	 GetSlot								()								{ return slotNum; }
	// �������� ����� ������ ������ (��� - 0, ������� - 1, et cetera)
	void SetSectionGroupID						(u32 section)			{ sectionNum = section; }
	u32	 GetSectionGroupID						()					{ return sectionNum; }
	// ������� ��� �����������/����������� ����� ������ � .ltx �����, ���� ����
	void SetSectionName							(const char *pData)		{ strName = pData; }
	const char * GetSectionName					() const			{ return strName.c_str(); }
	// ����������/����������� ��������� �� CUIDragDropList �������� ���������� �����������
	// ����
	void SetOwner								(CUIDragDropList *pOwner)		{ R_ASSERT(pOwner); m_pOwner = pOwner; }
	CUIDragDropList * GetOwner					()				{ return m_pOwner; }
	// ����� �������� � ������ ������
	void SetPosInSectionsGroup					(const u32 pos)	{ posInSection = pos; }
	u32 GetPosInSectionsGroup					() const 			{ return posInSection; }
	void SetPosInSubSection						(const u32 pos)		{ posInSubSection = pos; }
	u32 GetPosInSubSection						() const				{ return posInSubSection; }

	// ��������� ���������� � ������
	typedef struct tAddonInfo
	{
		// ��� ������ ��� ������
		xr_string			strAddonName;
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

	AddonIDs			IsOurAddon(CUIDragDropItemMP * pPossibleAddon);
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

	int					m_iRank;

	// ������ �������������� ����� ������� ����� ���� ���������� ��� ������������ ����� �����,
	// �������� ��� ������� ������ ���������� ������ � ����� �������� ��� ��������������
	xr_vector<float>	m_fAdditionalInfo;
};

#endif	//UI_DRAG_DROP_ITEM_MP_H_