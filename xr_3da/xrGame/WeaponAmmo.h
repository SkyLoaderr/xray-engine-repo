#pragma once
#include "inventory.h"

class CCartridge {
public:
	CCartridge();
	void Load(LPCSTR section);

	LPCSTR m_ammoSect;
	f32 m_kDist, m_kDisp, m_kHit, m_kImpulse, m_kPierce;
	bool m_tracer;
	int	m_buckShot;
	f32 m_impair;
};

class CWeaponAmmo :
	public CInventoryItem
{
	typedef CInventoryItem		inherited;
public:
	CWeaponAmmo(void);
	virtual ~CWeaponAmmo(void);

	virtual void Load(LPCSTR section);
	virtual BOOL net_Spawn(LPVOID DC);
	virtual void net_Destroy();
	virtual void OnH_B_Chield();
	virtual void OnH_B_Independent();
	virtual void UpdateCL();
	virtual void OnVisible();

	virtual const char* Name();
	virtual const char* NameShort();

	virtual bool Useful();
	virtual s32 Sort(PIItem pIItem);
	virtual bool Merge(PIItem pIItem);

	bool Get(CCartridge &cartridge);

	f32 m_kDist, m_kDisp, m_kHit, m_kImpulse, m_kPierce;
	bool m_tracer;
	int	m_buckShot;
	f32 m_impair;

	u16 m_boxSize;				// Размер коробки (Сколько патронов игрок берет за раз)
	u16 m_boxCurr;				// Сколько патронов осталось в коробке (включая текущий магазин)
	char m_tmpName[255];
};
