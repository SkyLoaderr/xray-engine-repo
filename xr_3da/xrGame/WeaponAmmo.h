#pragma once
#include "inventory_item.h"

class CCartridge 
{
public:
	CCartridge();
	void Load(LPCSTR section);

	shared_str	m_ammoSect;
	float	m_kDist, m_kDisp, m_kHit, m_kImpulse, m_kPierce;
	bool	m_tracer;
	bool	m_ricochet;
	int		m_buckShot;
	float	m_impair;
	float	fWallmarkSize;
	
	u16		bullet_material_idx;
};

class CWeaponAmmo :	public CInventoryItem
{
	typedef CInventoryItem		inherited;
public:
	CWeaponAmmo(void);
	virtual ~CWeaponAmmo(void);

	virtual void Load(LPCSTR section);
	virtual BOOL net_Spawn(LPVOID DC);
	virtual void net_Destroy();
	virtual void net_Export(NET_Packet& P);	// export to server
	virtual void net_Import(NET_Packet& P);	// import from server
	virtual void OnH_B_Chield();
	virtual void OnH_B_Independent();
	virtual void UpdateCL();
	virtual void renderable_Render();

	virtual bool Useful() const;
	virtual s32 Sort(PIItem pIItem);
	virtual bool Merge(PIItem pIItem);

	bool		Get(CCartridge &cartridge);

	float		m_kDist, m_kDisp, m_kHit, m_kImpulse, m_kPierce;
	bool		m_tracer;
	int			m_buckShot;
	float		m_impair;
	float		fWallmarkSize;

	u16			m_boxSize;			// Размер коробки (Сколько патронов игрок берет за раз)
	u16			m_boxCurr;			// Сколько патронов осталось в коробке (включая текущий магазин)

	bool		m_bCanBeUnlimited;

protected:
	////////////// network //////////////////	

public:
	virtual CInventoryItem *can_make_killing	(const CInventory *inventory) const;
};
