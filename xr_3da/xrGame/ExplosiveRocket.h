//////////////////////////////////////////////////////////////////////
// ExplosiveRocket.h:	ракета, которой стреляет RocketLauncher 
//						взрывается при столкновении
//////////////////////////////////////////////////////////////////////

#pragma once

#include "CustomRocket.h"
#include "Explosive.h"



class CExplosiveRocket : public CCustomRocket,
							public CExplosive
{
private:
	typedef CCustomRocket inherited;
	friend CRocketLauncher;
public:
	CExplosiveRocket(void);
	virtual ~CExplosiveRocket(void);
public:
	virtual CInventoryItem*				cast_inventory_item		()						{return this;}
	virtual const CInventoryItem*		cast_inventory_item		() const				{return this;}
public:

	virtual void Load(LPCSTR section);
	virtual BOOL net_Spawn(LPVOID DC);
	virtual void net_Destroy();

	virtual void OnH_A_Independent();
	virtual void OnH_B_Independent();
	virtual void UpdateCL();

	virtual void Contact(const Fvector &pos, const Fvector &normal);

	virtual void OnEvent (NET_Packet& P, u16 type) ;

	virtual	void Hit	(float P, Fvector &dir,	CObject* who, s16 element,
						Fvector position_in_object_space, float impulse, 
						ALife::EHitType hit_type = ALife::eHitTypeWound)
						{inherited::Hit(P, dir, who, element, position_in_object_space,impulse,hit_type);}

public:
	virtual BOOL			UsedAI_Locations	()				{return inherited::UsedAI_Locations();}
	virtual void			net_Import			(NET_Packet& P)	{inherited::net_Import(P);}
	virtual void			net_Export			(NET_Packet& P) {inherited::net_Export(P);}
	
	virtual void			save				(NET_Packet &output_packet) {inherited::save(output_packet);}
	virtual void			load				(IReader &input_packet)		{inherited::load(input_packet);}
	virtual BOOL			net_SaveRelevant	()							{return inherited::net_SaveRelevant();}

	virtual void			OnH_A_Chield		()				{inherited::OnH_A_Chield();}
	virtual void			OnH_B_Chield		()				{inherited::OnH_B_Chield();}
	virtual void			renderable_Render	()				{inherited::renderable_Render();}
	virtual void			make_Interpolation	();
	virtual void			PH_B_CrPr			(); // actions & operations before physic correction-prediction steps
	virtual void			PH_I_CrPr			(); // actions & operations after correction before prediction steps
#ifdef DEBUG
	virtual void			PH_Ch_CrPr			(); // 
	virtual void			OnRender			();
#endif
	virtual void			PH_A_CrPr			(); // actions & operations after phisic correction-prediction steps
	virtual void			reinit				();
	virtual void			reload				(LPCSTR section);
	virtual void			activate_physic_shell	();
	virtual void			setup_physic_shell		();
	virtual void			create_physic_shell		();
};