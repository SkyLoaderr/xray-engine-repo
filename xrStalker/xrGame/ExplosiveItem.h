//////////////////////////////////////////////////////////////////////
// ExplosiveItem.h: класс для вещи которая взрывается под 
//					действием различных хитов (канистры,
//					балоны с газом и т.д.)
//////////////////////////////////////////////////////////////////////

#pragma once

#include "Explosive.h"

class CExplosiveItem: public CExplosive
{
private:
	typedef CExplosive inherited;
public:
	CExplosiveItem(void);
	virtual ~CExplosiveItem(void);

	virtual void				Load				(LPCSTR section);
	virtual BOOL				net_Spawn			(LPVOID DC)			{return CInventoryItem::net_Spawn(DC);}
	virtual void				net_Destroy			();
	virtual void				net_Export			(NET_Packet& P)		{CInventoryItem::net_Export(P);}
	virtual void				net_Import			(NET_Packet& P)		{CInventoryItem::net_Import(P);}


	virtual void OnEvent		(NET_Packet& P, u16 type);
	virtual	void Hit			(float P, Fvector &dir,	CObject* who, s16 element,
										  Fvector position_in_object_space, float impulse, 
										  ALife::EHitType hit_type = ALife::eHitTypeWound);
	
	virtual void UpdateCL();
	virtual void renderable_Render(); 
};