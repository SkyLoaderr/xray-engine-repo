#include "stdafx.h"
#pragma hdrstop

#include "game_base.h"
#include "clsid_game.h"
#include "xrServer_Entities.h"

#ifdef _EDITOR
	#include "ui_main.h"
#endif

xrSE_Weapon::xrSE_Weapon(LPCSTR caSection) : CALifeItem(caSection)
{
	a_current			= 90;
	a_elapsed			= 0;
	state				= 0;
	if (pSettings->section_exist(caSection) && pSettings->line_exist(caSection,"visual"))
        set_visual		(pSettings->r_string(caSection,"visual"));
}

void xrSE_Weapon::UPDATE_Read		(NET_Packet& P)
{
	inherited::UPDATE_Read(P);
	P.r_u32				(timestamp);
	P.r_u8				(flags);

	P.r_u16				(a_current);
	P.r_u16				(a_elapsed);

	P.r_vec3			(o_Position	);
	P.r_angle8			(o_Angle.x	);
	P.r_angle8			(o_Angle.y	);
	P.r_angle8			(o_Angle.z	);
}
void	xrSE_Weapon::UPDATE_Write	(NET_Packet& P)
{
	inherited::UPDATE_Write(P);
	P.w_u32				(timestamp);
	P.w_u8				(flags);

	P.w_u16				(a_current);
	P.w_u16				(a_elapsed);

	P.w_vec3			(o_Position	);
	P.w_angle8			(o_Angle.x	);
	P.w_angle8			(o_Angle.y	);
	P.w_angle8			(o_Angle.z	);
}
void	xrSE_Weapon::STATE_Read		(NET_Packet& P, u16 size)
{
	inherited::STATE_Read(P, size);
	P.r_u16				(a_current);
	P.r_u16				(a_elapsed);
	P.r_u8				(state);
}
void	xrSE_Weapon::STATE_Write	(NET_Packet& P)
{
	inherited::STATE_Write(P);
	P.w_u16				(a_current);
	P.w_u16				(a_elapsed);
	P.w_u8				(state);
}
void	xrSE_Weapon::OnEvent		(NET_Packet& P, u16 type, u32 time, u32 sender )
{
	inherited::OnEvent	(P,type,time,sender);
	switch(type) 
	{
	case GE_WPN_STATE_CHANGE:
		P.r_u8	(state);
		break;
	}
}

u8		xrSE_Weapon::get_slot		()
{
	return (u8) pSettings->r_u8		(s_name,"slot");
}
u16		xrSE_Weapon::get_ammo_limit	()
{
	return (u16) pSettings->r_u16	(s_name,"ammo_limit");
}
u16		xrSE_Weapon::get_ammo_total	()
{
	return (u16) a_current;
}
u16		xrSE_Weapon::get_ammo_elapsed()
{
	return (u16) a_elapsed;
}
u16		xrSE_Weapon::get_ammo_magsize()
{
	if(pSettings->line_exist(s_name,"ammo_mag_size")) return pSettings->r_u16(s_name,"ammo_mag_size");
	else return 0;
}

#ifdef _EDITOR
void	xrSE_Weapon::FillProp		(LPCSTR pref, PropItemVec& items)
    {
    	inherited::FillProp(pref, items);
        PHelper.CreateU16(items,PHelper.PrepareKey(pref,s_name,"Ammo: total"),			&a_current,0,1000,1);
        PHelper.CreateU16(items,PHelper.PrepareKey(pref,s_name,"Ammo: in magazine"),	&a_elapsed,0,30,1);
    }
#endif

//***** WeaponAmmo
xrSE_WeaponAmmo::xrSE_WeaponAmmo(LPCSTR caSection) : CALifeItem(caSection)
{
	a_elapsed = m_boxSize = pSettings->r_s32(caSection, "box_size");
	if (pSettings->section_exist(caSection) && pSettings->line_exist(caSection,"visual"))
        set_visual		(pSettings->r_string(caSection,"visual"));
}

void xrSE_WeaponAmmo::STATE_Read(NET_Packet& P, u16 size)
{
	inherited::STATE_Read(P,size);
	P.r_u16(a_elapsed);
}

void xrSE_WeaponAmmo::STATE_Write(NET_Packet& P)
{
	inherited::STATE_Write(P);
	P.w_u16(a_elapsed);
}

void xrSE_WeaponAmmo::UPDATE_Read(NET_Packet& P)
{
	inherited::UPDATE_Read(P);
	P.r_u16(a_elapsed);
}

void xrSE_WeaponAmmo::UPDATE_Write(NET_Packet& P)
{
	inherited::UPDATE_Write(P);
	P.w_u16(a_elapsed);
}

#ifdef _EDITOR
void xrSE_WeaponAmmo::FillProp(LPCSTR pref, PropItemVec& values) {
  	inherited::FillProp(pref,values);
	PHelper.CreateU16(values, PHelper.PrepareKey(pref, s_name, "Ammo: left"), &a_elapsed, 0, m_boxSize, m_boxSize);
}
#endif

//
xrSE_Teamed::xrSE_Teamed(LPCSTR caSection) : CALifeDynamicObject(caSection)
{
	s_team = s_squad = s_group = 0;
	fHealth						= 100;
}

void	xrSE_Teamed::STATE_Read			(NET_Packet& P, u16 size)
{
	inherited::STATE_Read(P, size);
	P.r_u8				(s_team	);
	P.r_u8				(s_squad);
	P.r_u8				(s_group);
	if (m_wVersion > 18)
		P.r_float(fHealth);
}

void	xrSE_Teamed::STATE_Write		(NET_Packet& P)
{
	inherited::STATE_Write(P);
	P.w_u8				(s_team	);
	P.w_u8				(s_squad);
	P.w_u8				(s_group);
	P.w_float			(fHealth);
}

void	xrSE_Teamed::UPDATE_Read		(NET_Packet& P)
{
	inherited::UPDATE_Read(P);
	P.r_float_q16		(fHealth,	-1000,1000);
};

void	xrSE_Teamed::UPDATE_Write		(NET_Packet& P)
{
	inherited::UPDATE_Write(P);
	P.w_float_q16		(fHealth,	-1000,1000);
};

#ifdef _EDITOR
void	xrSE_Teamed::FillProp			(LPCSTR pref, PropItemVec& items)
{
  	inherited::FillProp(pref,items);
    PHelper.CreateU8(items,PHelper.PrepareKey(pref,s_name, "Team"),		&s_team, 	0,64,1);
    PHelper.CreateU8(items,PHelper.PrepareKey(pref,s_name, "Squad"),	&s_squad, 	0,64,1);
    PHelper.CreateU8(items,PHelper.PrepareKey(pref,s_name, "Group"),	&s_group, 	0,64,1);
   	PHelper.CreateFloat(		items, PHelper.PrepareKey(pref,s_name,"Personal",	"Health" 				),&fHealth,							0,200,5);
}
#endif

//***** Dummy
xrSE_Dummy::xrSE_Dummy(LPCSTR caSection) : xrServerEntity(caSection)
{
	s_Animation	= 0;
	s_Model		= 0;
	s_Particles	= 0;
	s_Sound		= 0;
}
xrSE_Dummy::~xrSE_Dummy()
{
	xr_free				(s_Animation	);
	xr_free				(s_Model		);
	xr_free				(s_Particles	);
	xr_free				(s_Sound		);
}
void xrSE_Dummy::STATE_Read			(NET_Packet& P, u16 size)
{
	P.r_u8				(s_style);

	if (s_style&esAnimated)		{
		// Load animator
		string256				fn;
		P.r_string				(fn);
		s_Animation				= xr_strdup(fn);
	}
	if (s_style&esModel)			{
		// Load model
		string256				fn;
		P.r_string				(fn);
		s_Model					= xr_strdup(fn);
	}
	if (s_style&esParticles)		{
		// Load model
		string256				fn;
		P.r_string				(fn);
		s_Particles				= xr_strdup(fn);
	}
	if (s_style&esSound)			{
		// Load model
		string256				fn;
		P.r_string				(fn);
		s_Sound					= xr_strdup(fn);
	}
}
void xrSE_Dummy::STATE_Write		(NET_Packet& P)
{
	P.w_u8				(s_style		);
	if (s_style&esAnimated)		P.w_string			(s_Animation	);
	if (s_style&esModel)		P.w_string			(s_Model		);
	if (s_style&esParticles)	P.w_string			(s_Particles	);
	if (s_style&esSound)		P.w_string			(s_Sound		);
}
void xrSE_Dummy::UPDATE_Read		(NET_Packet& P)	{};
void xrSE_Dummy::UPDATE_Write		(NET_Packet& P)	{};
#ifdef _EDITOR
void	xrSE_Dummy::FillProp			(LPCSTR pref, PropItemVec& values)
{
  	inherited::FillProp(pref,values);
}
#endif

//***** MercuryBall
xrSE_MercuryBall::xrSE_MercuryBall(LPCSTR caSection) : CALifeItem(caSection)
{
	if (pSettings->section_exist(caSection) && pSettings->line_exist(caSection,"visual"))
        set_visual		(pSettings->r_string(caSection,"visual"));
}
void	xrSE_MercuryBall::UPDATE_Read	(NET_Packet& P)				{inherited::UPDATE_Read(P);}
void	xrSE_MercuryBall::UPDATE_Write	(NET_Packet& P)				{inherited::UPDATE_Write(P);}
void	xrSE_MercuryBall::STATE_Read	(NET_Packet& P, u16 size)	{inherited::STATE_Read(P, size);visual_read(P); }
void	xrSE_MercuryBall::STATE_Write	(NET_Packet& P)				{inherited::STATE_Write(P);visual_write(P); }

#ifdef _EDITOR
void	xrSE_MercuryBall::FillProp	(LPCSTR pref, PropItemVec& items)
{
	xrSE_Visualed::FillProp(pref,items);
}
#endif
//


//***** Car
void xrSE_Car::STATE_Read			(NET_Packet& P, u16 size)	{};
void xrSE_Car::STATE_Write			(NET_Packet& P)				{};
void xrSE_Car::UPDATE_Read			(NET_Packet& P)				{};
void xrSE_Car::UPDATE_Write			(NET_Packet& P)				{};
#ifdef _EDITOR
void xrSE_Car::FillProp				(LPCSTR pref, PropItemVec& values)
{
  	inherited::FillProp(pref,values);
}
#endif

//***** Crow
void xrSE_Crow::STATE_Read			(NET_Packet& P, u16 size)	{};
void xrSE_Crow::STATE_Write			(NET_Packet& P)				{};
void xrSE_Crow::UPDATE_Read			(NET_Packet& P)				{};
void xrSE_Crow::UPDATE_Write		(NET_Packet& P)				{};
#ifdef _EDITOR
void xrSE_Crow::FillProp			(LPCSTR pref, PropItemVec& values)
{
  	inherited::FillProp(pref,values);
}
#endif

//***** Target
void xrSE_Target::STATE_Read		(NET_Packet& P, u16 size)	{};
void xrSE_Target::STATE_Write		(NET_Packet& P)				{};
void xrSE_Target::UPDATE_Read		(NET_Packet& P)				{};
void xrSE_Target::UPDATE_Write		(NET_Packet& P)				{};
#ifdef _EDITOR
void xrSE_Target::FillProp			(LPCSTR pref, PropItemVec& values)
{
	inherited::FillProp(pref,values);
}
#endif

//***** Target Assault
void xrSE_Target_Assault::STATE_Read		(NET_Packet& P, u16 size)	{};
void xrSE_Target_Assault::STATE_Write		(NET_Packet& P)				{};
void xrSE_Target_Assault::UPDATE_Read		(NET_Packet& P)				{};
void xrSE_Target_Assault::UPDATE_Write		(NET_Packet& P)				{};
#ifdef _EDITOR
void xrSE_Target_Assault::FillProp			(LPCSTR pref, PropItemVec& values)
{
	inherited::FillProp(pref,values);
}
#endif

//***** Target CS Base
xrSE_Target_CSBase::xrSE_Target_CSBase(LPCSTR caSection) : xrSE_Target(caSection)
{
	s_team									= 0;
	radius									= 10.f;
    s_gameid								= GAME_CS;    
};

void xrSE_Target_CSBase::STATE_Read			(NET_Packet& P, u16 size)
{
	P.r_float(radius);
	P.r_u8(s_team);
}

void xrSE_Target_CSBase::STATE_Write		(NET_Packet& P)
{
	P.w_float(radius);
	P.w_u8(s_team);
}

void xrSE_Target_CSBase::UPDATE_Read		(NET_Packet& P)
{
}

void xrSE_Target_CSBase::UPDATE_Write		(NET_Packet& P)
{
}

#ifdef _EDITOR
void xrSE_Target_CSBase::FillProp			(LPCSTR pref, PropItemVec& items)
{
	inherited::FillProp	(pref,items);
    PHelper.CreateFloat	(items,PHelper.PrepareKey(pref,s_name,"Radius"),	&radius,1.f,100.f);
    PHelper.CreateU8	(items,PHelper.PrepareKey(pref,s_name,"Team"),		&s_team,0,1);
}
#endif

//***** Target CS Cask
xrSE_Target_CSCask::xrSE_Target_CSCask(LPCSTR caSection) : xrSE_Target(caSection)
{
	s_Model[0]	=	0;
}
void	xrSE_Target_CSCask::UPDATE_Read		(NET_Packet& P)
{
}

void	xrSE_Target_CSCask::UPDATE_Write	(NET_Packet& P)
{
}

void	xrSE_Target_CSCask::STATE_Read		(NET_Packet& P, u16 size)
{
	P.r_string(s_Model);
}

void	xrSE_Target_CSCask::STATE_Write		(NET_Packet& P)
{
	P.w_string(s_Model);
}

#ifdef _EDITOR
void	xrSE_Target_CSCask::FillProp	(LPCSTR pref, PropItemVec& items)
{
	PHelper.CreateGameObject(items, PHelper.PrepareKey(pref,s_name,"Model"),	s_Model,	sizeof(s_Model));
}
#endif
//

//***** Target CS
xrSE_Target_CS::xrSE_Target_CS(LPCSTR caSection) : xrSE_Target(caSection)
{
	s_Model[0]	=	0;
}

void	xrSE_Target_CS::UPDATE_Read	(NET_Packet& P)
{
}

void	xrSE_Target_CS::UPDATE_Write(NET_Packet& P)
{
}

void	xrSE_Target_CS::STATE_Read	(NET_Packet& P, u16 size)
{
	P.r_string(s_Model);
}

void	xrSE_Target_CS::STATE_Write	(NET_Packet& P)
{
	P.w_string(s_Model);
}

#ifdef _EDITOR
void	xrSE_Target_CS::FillProp	(LPCSTR pref, PropItemVec& items)
{
	PHelper.CreateGameObject(items, PHelper.PrepareKey(pref,s_name,"Model"),	s_Model,	sizeof(s_Model));
}
#endif
//

//***** Health
void xrSE_Health::STATE_Read		(NET_Packet& P, u16 size)
{
	inherited::STATE_Read(P, size);
	P.r_u8(amount);
}

void xrSE_Health::STATE_Write		(NET_Packet& P)
{
	inherited::STATE_Write(P);
	P.w_u8(amount);
}

void xrSE_Health::UPDATE_Read		(NET_Packet& P)
{
	inherited::UPDATE_Read(P);
}

void xrSE_Health::UPDATE_Write		(NET_Packet& P)
{
	inherited::UPDATE_Write(P);
}

#ifdef _EDITOR
void xrSE_Health::FillProp			(LPCSTR pref, PropItemVec& items)
{
	inherited::FillProp(pref,items);
    PHelper.CreateU8(items,	PHelper.PrepareKey(pref,s_name,"Health amount"),	&amount,0,255);
}
#endif

//***** Spectator
void xrSE_Spectator::STATE_Read		(NET_Packet& P, u16 size)
{
}

void xrSE_Spectator::STATE_Write	(NET_Packet& P)
{
}

void xrSE_Spectator::UPDATE_Read	(NET_Packet& P)
{
}

void xrSE_Spectator::UPDATE_Write	(NET_Packet& P)
{
}

#ifdef _EDITOR
void xrSE_Spectator::FillProp		(LPCSTR pref, PropItemVec& items)
{
  	inherited::FillProp(pref,items);
}
#endif

//***** Actor
xrSE_Actor::xrSE_Actor				(LPCSTR caSection) : xrSE_Teamed(caSection), CALifeTraderParams(caSection)
{
	set_visual						("actors\\Different_stalkers\\stalker_hood_multiplayer.ogf");
}

void xrSE_Actor::STATE_Read			(NET_Packet& P, u16 size)
{
	inherited::STATE_Read(P,size);
	CALifeTraderParams::STATE_Read(P,size);
	if (m_wVersion >= 3)
		visual_read(P);
};

void xrSE_Actor::STATE_Write		(NET_Packet& P)
{
	inherited::STATE_Write(P);
	CALifeTraderParams::STATE_Write(P);
	visual_write(P);
};

void xrSE_Actor::UPDATE_Read		(NET_Packet& P)
{
	inherited::UPDATE_Read(P);
	CALifeTraderParams::UPDATE_Read(P);
	P.r_u32				(timestamp	);
	P.r_u8				(flags		);
	P.r_vec3			(o_Position	);
	P.r_u16				(mstate		);
	P.r_angle8			(model_yaw	);
	P.r_angle8			(torso.yaw	);
	P.r_angle8			(torso.pitch);
	P.r_sdir			(accel		);
	P.r_sdir			(velocity	);
	P.r_float_q16		(fArmor,	-1000,1000);
	P.r_u8				(weapon		);
};
void xrSE_Actor::UPDATE_Write		(NET_Packet& P)
{
	inherited::UPDATE_Write(P);
	CALifeTraderParams::UPDATE_Write(P);
	P.w_u32				(timestamp	);
	P.w_u8				(flags		);
	P.w_vec3			(o_Position	);
	P.w_u16				(mstate		);
	P.w_angle8			(model_yaw	);
	P.w_angle8			(torso.yaw	);
	P.w_angle8			(torso.pitch);
	P.w_sdir			(accel		);
	P.w_sdir			(velocity	);
	P.w_float_q16		(fArmor,	-1000,1000);
	P.w_u8				(weapon		);
}
#ifdef _EDITOR
void	xrSE_Actor::FillProp			(LPCSTR pref, PropItemVec& items)
{
  	inherited::FillProp					(pref,items);
	CALifeTraderParams::FillProp		(pref,items);
	xrSE_Visualed::FillProp				(pref,items);
}
#endif

//***** Enemy
void xrSE_Enemy::STATE_Read			(NET_Packet& P, u16 size)	{inherited::STATE_Read(P,size); };
void xrSE_Enemy::STATE_Write		(NET_Packet& P)				{inherited::STATE_Write(P);		};
void xrSE_Enemy::UPDATE_Read		(NET_Packet& P)
{
	inherited::UPDATE_Read(P);
	P.r_u32				(timestamp		);
	P.r_u8				(flags			);
	P.r_vec3			(o_Position		);
	P.r_angle8			(o_model		);
	P.r_angle8			(o_torso.yaw	);
	P.r_angle8			(o_torso.pitch	);
}
void xrSE_Enemy::UPDATE_Write		(NET_Packet& P)
{
	inherited::UPDATE_Write(P);
	P.w_u32				(timestamp		);
	P.w_u8				(flags			);
	P.w_vec3			(o_Position		);
	P.w_angle8			(o_model		);
	P.w_angle8			(o_torso.yaw	);
	P.w_angle8			(o_torso.pitch	);
}
#ifdef _EDITOR
void	xrSE_Enemy::FillProp			(LPCSTR pref, PropItemVec& items)
{
  	inherited::FillProp(pref,items);
}
#endif

////***** CFormed (Base)
//cxr_vectorrSE_CFormed
//{
//public:
//	union shape_data
//	{
//		Fsphere		sphere;
//		Fmatrix		box;
//	};
//	struct shape_def
//	{
//		u8			type;
//		shape_data	data;
//	};
//	xr_vector<shape_def>	shapes;
//public:
//	void					cform_read			(NET_Packet& P)
//	{
//		shapes.clear();
//		u8	count;	P.r_u8(count);
//		while (count) {
//			shape_def	S;
//			P.r_u8	(S.type);
//			switch	(S.type)
//			{
//			case 0:	P.r			(&S.data.sphere,sizeof(S.data.sphere));	break;
//			case 1:	P.r_matrix	(S.data.box);							break;
//			}
//			shapes.push_back	(S);
//			count--;
//		}
//	}
//	void					cform_write			(NET_Packet& P)
//	{
//		P.w_u8	(u8(shapes.size()));
//		for (u32 i=0; i<shapes.size(); i++)
//		{
//			shape_def&	S	= shapes[i];
//			P.w_u8	(S.type);
//			switch (S.type)
//			{
//			case 0:	P.w			(&S.data.sphere,sizeof(S.data.sphere));	break;
//			case 1:	P.w_matrix	(S.data.box);							break;
//			}
//		}
//	}
//};
void					xrSE_Visualed::set_visual(LPCSTR name)
{
	strcpy				(visual_name,name);
#ifdef _EDITOR
	OnChangeVisual		(0);
#endif
}

void					xrSE_Visualed::visual_read(NET_Packet& P)
{
	P.r_string			(visual_name);
#ifdef _EDITOR
	OnChangeVisual		(0);
#endif
}

void					xrSE_Visualed::visual_write(NET_Packet& P)
{
	P.w_string			(visual_name);
}

#ifdef _EDITOR
#include "BodyInstance.h"
void 					xrSE_Visualed::PlayAnimation(LPCSTR name)
{
    // play motion if skeleton
    if (PKinematics(visual)){ 
	    play_animation	= name;
        CMotionDef* M	= PKinematics(visual)->ID_Cycle_Safe(play_animation.c_str());
        if (M) 			PKinematics(visual)->PlayCycle(M); 
        PKinematics(visual)->Calculate();
    }
}
void __fastcall			xrSE_Visualed::OnChangeVisual(PropValue* sender)
{
	Device.Models.Delete(visual);
    if (visual_name[0]){
        visual			= Device.Models.Create(visual_name);
        PlayAnimation	(play_animation.c_str());
    }
	UI.Command			(COMMAND_UPDATE_PROPERTIES);
}
void 					xrSE_Visualed::FillProp(LPCSTR pref, PropItemVec& values)
{
    PropValue* V		= PHelper.CreateGameObject(values, PHelper.PrepareKey(pref,"Model"),visual_name,sizeof(visual_name));
    V->SetEvents		(0,0,OnChangeVisual);
}
#endif

void					xrSE_CFormed::cform_read			(NET_Packet& P)
{
	shapes.clear();
	u8	count;	P.r_u8(count);
	while (count) {
		shape_def	S;
		P.r_u8	(S.type);
		switch	(S.type)
		{
		case 0:	P.r			(&S.data.sphere,sizeof(S.data.sphere));	break;
		case 1:	P.r_matrix	(S.data.box);							break;
		}
		shapes.push_back	(S);
		count--;
	}
}
void					xrSE_CFormed::cform_write			(NET_Packet& P)
{
	P.w_u8	(u8(shapes.size()));
	for (u32 i=0; i<shapes.size(); i++)
	{
		shape_def&	S	= shapes[i];
		P.w_u8	(S.type);
		switch (S.type)
		{
		case 0:	P.w			(&S.data.sphere,sizeof(S.data.sphere));	break;
		case 1:	P.w_matrix	(S.data.box);							break;
		}
	}
}

class xrSE_Event : public xrSE_CFormed, public xrServerEntity
{
	typedef	xrServerEntity		inherited;
public:	// actions
	struct tAction
	{
		u8		type;
		u16		count;
		u64		cls;
		LPSTR	event;
	};
	xr_vector<tAction>			Actions;

	void					Actions_clear()
	{
		for (u32 a=0; a<Actions.size(); a++)
			xr_free(Actions[a].event);
		Actions.clear	();
	}
public:	
							xrSE_Event(LPCSTR caSection) : xrServerEntity(caSection), xrSE_CFormed()
	{
	};
	
	virtual void			UPDATE_Read			(NET_Packet& P)
	{
	}
	virtual void			UPDATE_Write		(NET_Packet& P)
	{
	}
	virtual void			STATE_Read			(NET_Packet& P, u16 size)
	{
		// CForm
		cform_read			(P);

		// Actions
		Actions_clear		();
		u8 count;	P.r_u8	(count);
		while (count)	{
			tAction		A;
			string512	str;
			P.r_u8		(A.type);
			P.r_u16		(A.count);
			P.r_u64		(A.cls);
			P.r_string	(str);
			A.event		= xr_strdup(str);
			Actions.push_back(A);
			count--;
		}
	}
	virtual void			STATE_Write			(NET_Packet& P)
	{
		// CForm
		cform_write			(P);

		// Actions
		P.w_u8				(u8(Actions.size()));
		for (u32 i=0; i<Actions.size(); i++)
		{
			tAction&	A = Actions[i];
			P.w_u8		(A.type	);
			P.w_u16		(A.count);
			P.w_u64		(A.cls	);
			P.w_string	(A.event);
		}
	}
};

// CALifeMonsterAbstract
void CALifeMonsterAbstract::STATE_Write(NET_Packet &tNetPacket)
{
	inherited::STATE_Write		(tNetPacket);
}

void CALifeMonsterAbstract::STATE_Read(NET_Packet &tNetPacket, u16 size)
{
	inherited::STATE_Read		(tNetPacket, size);
}

void CALifeMonsterAbstract::UPDATE_Write(NET_Packet &tNetPacket)
{
	inherited::UPDATE_Write		(tNetPacket);
	tNetPacket.w				(&m_tNextGraphID,			sizeof(m_tNextGraphID));
	tNetPacket.w				(&m_tPrevGraphID,			sizeof(m_tPrevGraphID));
	tNetPacket.w				(&m_fGoingSpeed,			sizeof(m_fGoingSpeed));
	tNetPacket.w				(&m_fCurSpeed,				sizeof(m_fCurSpeed));
	tNetPacket.w				(&m_fDistanceFromPoint,		sizeof(m_fDistanceFromPoint));
	tNetPacket.w				(&m_fDistanceToPoint,		sizeof(m_fDistanceToPoint));
};

void CALifeMonsterAbstract::UPDATE_Read(NET_Packet &tNetPacket)
{
	inherited::UPDATE_Read		(tNetPacket);
	tNetPacket.r				(&m_tNextGraphID,			sizeof(m_tNextGraphID));
	tNetPacket.r				(&m_tPrevGraphID,			sizeof(m_tPrevGraphID));
	tNetPacket.r				(&m_fGoingSpeed,			sizeof(m_fGoingSpeed));
	tNetPacket.r				(&m_fCurSpeed,				sizeof(m_fCurSpeed));
	tNetPacket.r				(&m_fDistanceFromPoint,		sizeof(m_fDistanceFromPoint));
	tNetPacket.r				(&m_fDistanceToPoint,		sizeof(m_fDistanceToPoint));
};

//////////////////////////////////////////////////////////////////////////
// Rat
//////////////////////////////////////////////////////////////////////////

xrSE_Rat::xrSE_Rat(LPCSTR caSection) : CALifeMonsterAbstract(caSection)
{
	set_visual						("monsters\\rat\\rat_1");
	// personal charactersitics
	fEyeFov							= 120;
	fEyeRange						= 10;
	fHealth							= 5;
	fMinSpeed						= .5;
	fMaxSpeed						= 1.5;
	fAttackSpeed					= 4.0;
	fMaxPursuitRadius				= 100;
	fMaxHomeRadius					= 10;
	// morale
	fMoraleSuccessAttackQuant		= 20;
	fMoraleDeathQuant				= -10;
	fMoraleFearQuant				= -20;
	fMoraleRestoreQuant				=  10;
	u16MoraleRestoreTimeInterval	= 3000;
	fMoraleMinValue					= 0;
	fMoraleMaxValue					= 100;
	fMoraleNormalValue				= 66;
	// attack
	fHitPower						= 10.0;
	u16HitInterval					= 1500;
	fAttackDistance					= 0.7f;
	fAttackAngle					= 45;
	fAttackSuccessProbability		= 0.5f;
}

void xrSE_Rat::STATE_Read(NET_Packet& P, u16 size)
{
	// inherited properties
	inherited::STATE_Read(P,size);
	// model
	visual_read(P);
	// personal characteristics
	P.r_float (fEyeFov);
	P.r_float (fEyeRange);
	if (m_wVersion <= 5)
		P.r_float (fHealth);
	P.r_float (fMinSpeed);
	P.r_float (fMaxSpeed);
	P.r_float (fAttackSpeed);
	P.r_float (fMaxPursuitRadius);
	P.r_float (fMaxHomeRadius);
	// morale
	P.r_float (fMoraleSuccessAttackQuant);
	P.r_float (fMoraleDeathQuant);
	P.r_float (fMoraleFearQuant);
	P.r_float (fMoraleRestoreQuant);
	P.r_u16   (u16MoraleRestoreTimeInterval);
	P.r_float (fMoraleMinValue);
	P.r_float (fMoraleMaxValue);
	P.r_float (fMoraleNormalValue);
	// attack
	P.r_float (fHitPower);
	P.r_u16   (u16HitInterval);
	P.r_float (fAttackDistance);
	P.r_float (fAttackAngle);
	P.r_float (fAttackSuccessProbability);
}

void xrSE_Rat::STATE_Write(NET_Packet& P)
{
	// inherited properties
	inherited::STATE_Write(P);
	// model
	visual_write(P);
	// personal characteristics
	P.w_float (fEyeFov);
	P.w_float (fEyeRange);
	P.w_float (fMinSpeed);
	P.w_float (fMaxSpeed);
	P.w_float (fAttackSpeed);
	P.w_float (fMaxPursuitRadius);
	P.w_float (fMaxHomeRadius);
	// morale
	P.w_float (fMoraleSuccessAttackQuant);
	P.w_float (fMoraleDeathQuant);
	P.w_float (fMoraleFearQuant);
	P.w_float (fMoraleRestoreQuant);
	P.w_u16   (u16MoraleRestoreTimeInterval);
	P.w_float (fMoraleMinValue);
	P.w_float (fMoraleMaxValue);
	P.w_float (fMoraleNormalValue);
	// attack
	P.w_float (fHitPower);
	P.w_u16   (u16HitInterval);
	P.w_float (fAttackDistance);
	P.w_float (fAttackAngle);
	P.w_float (fAttackSuccessProbability);
}

void xrSE_Rat::UPDATE_Read(NET_Packet& P)
{
	inherited::UPDATE_Read(P);
}

void xrSE_Rat::UPDATE_Write(NET_Packet& P)
{
	inherited::UPDATE_Write(P);
}

#ifdef _EDITOR
void xrSE_Rat::FillProp(LPCSTR pref, PropItemVec& items)
{
   	inherited::FillProp(pref, items);
	// model
	xrSE_Visualed::FillProp(pref, items);
	// personal characteristics
   	PHelper.CreateFloat(		items, PHelper.PrepareKey(pref,s_name,"Personal",	"Field of view" 		),&fEyeFov,							0,170,10);
   	PHelper.CreateFloat(		items, PHelper.PrepareKey(pref,s_name,"Personal",	"Eye range" 			),&fEyeRange,						0,300,10);
   	PHelper.CreateFloat(		items, PHelper.PrepareKey(pref,s_name,"Personal",	"Minimum speed" 		),&fMinSpeed,						0,10,0.1);
   	PHelper.CreateFloat(		items, PHelper.PrepareKey(pref,s_name,"Personal",	"Maximum speed" 		),&fMaxSpeed,						0,10,0.1);
   	PHelper.CreateFloat(		items, PHelper.PrepareKey(pref,s_name,"Personal",	"Attack speed" 			),&fAttackSpeed,					0,10,0.1);
   	PHelper.CreateFloat(		items, PHelper.PrepareKey(pref,s_name,"Personal",	"Pursuit distance" 		),&fMaxPursuitRadius,				0,300,10);
   	PHelper.CreateFloat(		items, PHelper.PrepareKey(pref,s_name,"Personal",	"Home distance" 		),&fMaxHomeRadius,					0,300,10);
	// morale																		
	PHelper.CreateFloat(		items, PHelper.PrepareKey(pref,s_name,"Morale",		"Success attack quant" 	),&fMoraleSuccessAttackQuant,		-100,100,5);
	PHelper.CreateFloat(		items, PHelper.PrepareKey(pref,s_name,"Morale",		"Death quant" 			),&fMoraleDeathQuant,				-100,100,5);
	PHelper.CreateFloat(		items, PHelper.PrepareKey(pref,s_name,"Morale",		"Fear quant" 			),&fMoraleFearQuant,				-100,100,5);
	PHelper.CreateFloat(		items, PHelper.PrepareKey(pref,s_name,"Morale",		"Restore quant" 		),&fMoraleRestoreQuant,				-100,100,5);
	PHelper.CreateU16  (		items, PHelper.PrepareKey(pref,s_name,"Morale",		"Restore time interval" ),&u16MoraleRestoreTimeInterval,	0,65535,500);
	PHelper.CreateFloat(		items, PHelper.PrepareKey(pref,s_name,"Morale",		"Minimum value" 		),&fMoraleMinValue,					-100,100,5);
	PHelper.CreateFloat(		items, PHelper.PrepareKey(pref,s_name,"Morale",		"Maximum value" 		),&fMoraleMaxValue,					-100,100,5);
	PHelper.CreateFloat(		items, PHelper.PrepareKey(pref,s_name,"Morale",		"Normal value" 			),&fMoraleNormalValue,				-100,100,5);
	// attack																		 	
	PHelper.CreateFloat(		items, PHelper.PrepareKey(pref,s_name,"Attack",		"Hit power" 			),&fHitPower,						0,200,5);
	PHelper.CreateU16  (		items, PHelper.PrepareKey(pref,s_name,"Attack",		"Hit interval" 			),&u16HitInterval,					0,65535,500);
	PHelper.CreateFloat(		items, PHelper.PrepareKey(pref,s_name,"Attack",		"Distance" 				),&fAttackDistance,					0,300,10);
	PHelper.CreateFloat(		items, PHelper.PrepareKey(pref,s_name,"Attack",		"Maximum angle" 		),&fAttackAngle,					0,180,10);
	PHelper.CreateFloat(		items, PHelper.PrepareKey(pref,s_name,"Attack",		"Success probability" 	),&fAttackSuccessProbability,		0,100,1);
}	
#endif

//////////////////////////////////////////////////////////////////////////
// Zombie
//////////////////////////////////////////////////////////////////////////

xrSE_Zombie::xrSE_Zombie(LPCSTR caSection) : CALifeMonsterAbstract(caSection)
{
	set_visual						("monsters\\zombie\\zombie_1");
	// personal charactersitics
	fEyeFov							= 120;
	fEyeRange						= 30;
	fHealth							= 200;
	fMinSpeed						= 1.5;
	fMaxSpeed						= 1.75;
	fAttackSpeed					= 2.0;
	fMaxPursuitRadius				= 100;
	fMaxHomeRadius					= 30;
	// attack
	fHitPower						= 20.0;
	u16HitInterval					= 1000;
	fAttackDistance					= 1.0f;
	fAttackAngle					= 15;
}

void xrSE_Zombie::STATE_Read(NET_Packet& P, u16 size)
{
	// inherited properties
	inherited::STATE_Read(P,size);
	// model
	visual_read(P);
	// personal characteristics
	P.r_float (fEyeFov);
	P.r_float (fEyeRange);
	if (m_wVersion <= 5)
		P.r_float (fHealth);
	P.r_float (fMinSpeed);
	P.r_float (fMaxSpeed);
	P.r_float (fAttackSpeed);
	P.r_float (fMaxPursuitRadius);
	P.r_float (fMaxHomeRadius);
	// attack
	P.r_float (fHitPower);
	P.r_u16   (u16HitInterval);
	P.r_float (fAttackDistance);
	P.r_float (fAttackAngle);
}

void xrSE_Zombie::STATE_Write(NET_Packet& P)
{
	// inherited properties
	inherited::STATE_Write(P);
	// model
	visual_write(P);
	// personal characteristics
	P.w_float (fEyeFov);
	P.w_float (fEyeRange);
	P.w_float (fMinSpeed);
	P.w_float (fMaxSpeed);
	P.w_float (fAttackSpeed);
	P.w_float (fMaxPursuitRadius);
	P.w_float (fMaxHomeRadius);
	// attack
	P.w_float (fHitPower);
	P.w_u16   (u16HitInterval);
	P.w_float (fAttackDistance);
	P.w_float (fAttackAngle);
}

void xrSE_Zombie::UPDATE_Read(NET_Packet& P)
{
	inherited::UPDATE_Read(P);
}

void xrSE_Zombie::UPDATE_Write(NET_Packet& P)
{
	inherited::UPDATE_Write(P);
}

#ifdef _EDITOR
void xrSE_Zombie::FillProp(LPCSTR pref, PropItemVec& items)
{
   	inherited::FillProp(pref, items);
	// model
	xrSE_Visualed::FillProp(pref, items);
	// personal characteristics
	PHelper.CreateFloat(		items, PHelper.PrepareKey(pref,s_name,"Personal",	"Field of view" 		),&fEyeFov,							0,170,10);
   	PHelper.CreateFloat(		items, PHelper.PrepareKey(pref,s_name,"Personal",	"Eye range" 			),&fEyeRange,						0,300,10);
   	PHelper.CreateFloat(		items, PHelper.PrepareKey(pref,s_name,"Personal",	"Minimum speed" 		),&fMinSpeed,						0,10,0.1);
   	PHelper.CreateFloat(		items, PHelper.PrepareKey(pref,s_name,"Personal",	"Maximum speed" 		),&fMaxSpeed,						0,10,0.1);
   	PHelper.CreateFloat(		items, PHelper.PrepareKey(pref,s_name,"Personal",	"Attack speed" 			),&fAttackSpeed,					0,10,0.1);
   	PHelper.CreateFloat(		items, PHelper.PrepareKey(pref,s_name,"Personal",	"Pursuit distance" 		),&fMaxPursuitRadius,				0,300,10);
   	PHelper.CreateFloat(		items, PHelper.PrepareKey(pref,s_name,"Personal",	"Home distance" 		),&fMaxHomeRadius,					0,300,10);
	// attack																		 	
	PHelper.CreateFloat(		items, PHelper.PrepareKey(pref,s_name,"Attack",		"Hit power" 			),&fHitPower,						0,200,5);
	PHelper.CreateU16  (		items, PHelper.PrepareKey(pref,s_name,"Attack",		"Hit interval" 			),&u16HitInterval,					0,65535,500);
	PHelper.CreateFloat(		items, PHelper.PrepareKey(pref,s_name,"Attack",		"Distance" 				),&fAttackDistance,					0,300,10);
	PHelper.CreateFloat(		items, PHelper.PrepareKey(pref,s_name,"Attack",		"Maximum angle" 		),&fAttackAngle,					0,100,1);
}
#endif

//////////////////////////////////////////////////////////////////////////
// Dog
//////////////////////////////////////////////////////////////////////////

xrSE_Dog::xrSE_Dog(LPCSTR caSection) : CALifeMonsterAbstract(caSection)
{
	set_visual						("monsters\\dog\\dog_1");
	// personal charactersitics
	fEyeFov							= 120;
	fEyeRange						= 10;
	fHealth							= 5;
	fMinSpeed						= .5;
	fMaxSpeed						= 1.5;
	fAttackSpeed					= 4.0;
	fMaxPursuitRadius				= 100;
	fMaxHomeRadius					= 10;
	// morale
	fMoraleSuccessAttackQuant		= 20;
	fMoraleDeathQuant				= -10;
	fMoraleFearQuant				= -20;
	fMoraleRestoreQuant				=  10;
	u16MoraleRestoreTimeInterval	= 3000;
	fMoraleMinValue					= 0;
	fMoraleMaxValue					= 100;
	fMoraleNormalValue				= 66;
	// attack
	fHitPower						= 10.0;
	u16HitInterval					= 1500;
	fAttackDistance					= 0.7f;
	fAttackAngle					= 45;
	fAttackSuccessProbability		= 0.5f;
}

void xrSE_Dog::STATE_Read(NET_Packet& P, u16 size)
{
	// inherited properties
	inherited::STATE_Read(P,size);
	// model
	visual_read(P);
	// personal characteristics
	P.r_float (fEyeFov);
	P.r_float (fEyeRange);
	if (m_wVersion <= 5)
		P.r_float (fHealth);
	P.r_float (fMinSpeed);
	P.r_float (fMaxSpeed);
	P.r_float (fAttackSpeed);
	P.r_float (fMaxPursuitRadius);
	P.r_float (fMaxHomeRadius);
	// morale
	P.r_float (fMoraleSuccessAttackQuant);
	P.r_float (fMoraleDeathQuant);
	P.r_float (fMoraleFearQuant);
	P.r_float (fMoraleRestoreQuant);
	P.r_u16   (u16MoraleRestoreTimeInterval);
	P.r_float (fMoraleMinValue);
	P.r_float (fMoraleMaxValue);
	P.r_float (fMoraleNormalValue);
	// attack
	P.r_float (fHitPower);
	P.r_u16   (u16HitInterval);
	P.r_float (fAttackDistance);
	P.r_float (fAttackAngle);
	P.r_float (fAttackSuccessProbability);
}

void xrSE_Dog::STATE_Write(NET_Packet& P)
{
	// inherited properties
	inherited::STATE_Write(P);
	// model
	visual_write(P);
	// personal characteristics
	P.w_float (fEyeFov);
	P.w_float (fEyeRange);
	P.w_float (fMinSpeed);
	P.w_float (fMaxSpeed);
	P.w_float (fAttackSpeed);
	P.w_float (fMaxPursuitRadius);
	P.w_float (fMaxHomeRadius);
	// morale
	P.w_float (fMoraleSuccessAttackQuant);
	P.w_float (fMoraleDeathQuant);
	P.w_float (fMoraleFearQuant);
	P.w_float (fMoraleRestoreQuant);
	P.w_u16   (u16MoraleRestoreTimeInterval);
	P.w_float (fMoraleMinValue);
	P.w_float (fMoraleMaxValue);
	P.w_float (fMoraleNormalValue);
	// attack
	P.w_float (fHitPower);
	P.w_u16   (u16HitInterval);
	P.w_float (fAttackDistance);
	P.w_float (fAttackAngle);
	P.w_float (fAttackSuccessProbability);
}

void xrSE_Dog::UPDATE_Read(NET_Packet& P)
{
	inherited::UPDATE_Read(P);
}

void xrSE_Dog::UPDATE_Write(NET_Packet& P)
{
	inherited::UPDATE_Write(P);
}

#ifdef _EDITOR
void xrSE_Dog::FillProp(LPCSTR pref, PropItemVec& items)
{
   	inherited::FillProp(pref, items);
	// model
	xrSE_Visualed::FillProp(pref, items);
	// personal characteristics
   	PHelper.CreateFloat(		items, PHelper.PrepareKey(pref,s_name,"Personal",	"Field of view" 		),&fEyeFov,							0,170,10);
   	PHelper.CreateFloat(		items, PHelper.PrepareKey(pref,s_name,"Personal",	"Eye range" 			),&fEyeRange,						0,300,10);
   	PHelper.CreateFloat(		items, PHelper.PrepareKey(pref,s_name,"Personal",	"Minimum speed" 		),&fMinSpeed,						0,10,0.1);
   	PHelper.CreateFloat(		items, PHelper.PrepareKey(pref,s_name,"Personal",	"Maximum speed" 		),&fMaxSpeed,						0,10,0.1);
   	PHelper.CreateFloat(		items, PHelper.PrepareKey(pref,s_name,"Personal",	"Attack speed" 			),&fAttackSpeed,					0,10,0.1);
   	PHelper.CreateFloat(		items, PHelper.PrepareKey(pref,s_name,"Personal",	"Pursuit distance" 		),&fMaxPursuitRadius,				0,300,10);
   	PHelper.CreateFloat(		items, PHelper.PrepareKey(pref,s_name,"Personal",	"Home distance" 		),&fMaxHomeRadius,					0,300,10);
	// morale																		
	PHelper.CreateFloat(		items, PHelper.PrepareKey(pref,s_name,"Morale",		"Success attack quant" 	),&fMoraleSuccessAttackQuant,		-100,100,5);
	PHelper.CreateFloat(		items, PHelper.PrepareKey(pref,s_name,"Morale",		"Death quant" 			),&fMoraleDeathQuant,				-100,100,5);
	PHelper.CreateFloat(		items, PHelper.PrepareKey(pref,s_name,"Morale",		"Fear quant" 			),&fMoraleFearQuant,				-100,100,5);
	PHelper.CreateFloat(		items, PHelper.PrepareKey(pref,s_name,"Morale",		"Restore quant" 		),&fMoraleRestoreQuant,				-100,100,5);
	PHelper.CreateU16  (		items, PHelper.PrepareKey(pref,s_name,"Morale",		"Restore time interval" ),&u16MoraleRestoreTimeInterval,	0,65535,500);
	PHelper.CreateFloat(		items, PHelper.PrepareKey(pref,s_name,"Morale",		"Minimum value" 		),&fMoraleMinValue,					-100,100,5);
	PHelper.CreateFloat(		items, PHelper.PrepareKey(pref,s_name,"Morale",		"Maximum value" 		),&fMoraleMaxValue,					-100,100,5);
	PHelper.CreateFloat(		items, PHelper.PrepareKey(pref,s_name,"Morale",		"Normal value" 			),&fMoraleNormalValue,				-100,100,5);
	// attack																		 	
	PHelper.CreateFloat(		items, PHelper.PrepareKey(pref,s_name,"Attack",		"Hit power" 			),&fHitPower,						0,200,5);
	PHelper.CreateU16  (		items, PHelper.PrepareKey(pref,s_name,"Attack",		"Hit interval" 			),&u16HitInterval,					0,65535,500);
	PHelper.CreateFloat(		items, PHelper.PrepareKey(pref,s_name,"Attack",		"Distance" 				),&fAttackDistance,					0,300,10);
	PHelper.CreateFloat(		items, PHelper.PrepareKey(pref,s_name,"Attack",		"Maximum angle" 		),&fAttackAngle,					0,180,10);
	PHelper.CreateFloat(		items, PHelper.PrepareKey(pref,s_name,"Attack",		"Success probability" 	),&fAttackSuccessProbability,		0,100,1);
}
#endif

//////////////////////////////////////////////////////////////////////////
// Biting
//////////////////////////////////////////////////////////////////////////

xrSE_Biting::xrSE_Biting(LPCSTR caSection) : CALifeMonsterAbstract(caSection)
{
    set_visual						(pSettings->r_string(caSection,"visual"));
}

void xrSE_Biting::STATE_Read(NET_Packet& P, u16 size)
{
	// inherited properties
	inherited::STATE_Read(P,size);
	// model
	visual_read(P);
}

void xrSE_Biting::STATE_Write(NET_Packet& P)
{
	// inherited properties
	inherited::STATE_Write(P);
	// model
	visual_write(P);
}

void xrSE_Biting::UPDATE_Read(NET_Packet& P)
{
	inherited::UPDATE_Read(P);
}

void xrSE_Biting::UPDATE_Write(NET_Packet& P)
{
	inherited::UPDATE_Write(P);
}


#ifdef _EDITOR
void xrSE_Biting::FillProp(LPCSTR pref, PropItemVec& items)
{
   	inherited::FillProp(pref, items);
	// model
	xrSE_Visualed::FillProp(pref, items);
}	
#endif


// Zone
xrSE_Zone::xrSE_Zone(LPCSTR caSection) : CALifeDynamicObject(caSection)
{
	m_maxPower = 100.f;
	m_attn = 1.f;
	m_period = 1000;
}

void xrSE_Zone::STATE_Read		(NET_Packet& P, u16 size)	{
	// CForm
	if (m_wVersion >= 15)
		inherited::STATE_Read(P,size);
	cform_read			(P);

	P.r_float(m_maxPower);
	P.r_float(m_attn);
	P.r_u32(m_period);
};
void xrSE_Zone::STATE_Write		(NET_Packet& P)				{
	inherited::STATE_Write(P);
	// CForm
	cform_write			(P);

	P.w_float(m_maxPower);
	P.w_float(m_attn);
	P.w_u32(m_period);
};
void xrSE_Zone::UPDATE_Read		(NET_Packet& P)				{inherited::UPDATE_Read(P);}
void xrSE_Zone::UPDATE_Write	(NET_Packet& P)				{inherited::UPDATE_Write(P);}
#ifdef _EDITOR
void	xrSE_Zone::FillProp			(LPCSTR pref, PropItemVec& items)
{
	inherited::FillProp	(pref,items);
    PHelper.CreateFloat	(items,PHelper.PrepareKey(pref,s_name,"Power"),	&m_maxPower,0.f,1000.f);
    PHelper.CreateFloat	(items,PHelper.PrepareKey(pref,s_name,"Attenuation"),	&m_attn,0.f,100.f);
    PHelper.CreateU32	(items,PHelper.PrepareKey(pref,s_name,"Period"), &m_period,20,10000);
}
#endif

//***** Detector
void xrSE_Detector::STATE_Read		(NET_Packet& P, u16 size)	{};
void xrSE_Detector::STATE_Write		(NET_Packet& P)				{};
void xrSE_Detector::UPDATE_Read		(NET_Packet& P)				{};
void xrSE_Detector::UPDATE_Write	(NET_Packet& P)				{};
#ifdef _EDITOR
void xrSE_Detector::FillProp		(LPCSTR pref, PropItemVec& items)
{
  	inherited::FillProp(pref,items);
}
#endif


#ifdef _EDITOR
	static TokenValue4::ItemVec locations[4];
	static TokenValue4::ItemVec	level_ids;
#endif

xrGraphPoint::xrGraphPoint(LPCSTR caSection) : xrServerEntity(caSection)
{
	s_gameid					= GAME_DUMMY;
	m_caConnectionPointName[0]	= 0;
	m_tLevelID					= 0;
	m_tLocations[0]				= 0;
	m_tLocations[1]				= 0;
	m_tLocations[2]				= 0;
	m_tLocations[3]				= 0;
}

void xrGraphPoint::STATE_Read		(NET_Packet& P, u16 size)
{
	P.r_string	(m_caConnectionPointName);
	P.r_u32		(m_tLevelID);
	P.r_u8		(m_tLocations[0]);
	P.r_u8		(m_tLocations[1]);
	P.r_u8		(m_tLocations[2]);
	P.r_u8		(m_tLocations[3]);
	if (!P.r_eof()) {
		P.r_pos -= 4;
		u32			dwTemp;
		P.r_u32		(dwTemp);
		m_tLocations[0] = (u8)dwTemp;
		P.r_u32		(dwTemp);
		m_tLocations[1] = (u8)dwTemp;
	}
};

void xrGraphPoint::STATE_Write		(NET_Packet& P)
{
	P.w_string	(m_caConnectionPointName);
	P.w_u32		(m_tLevelID);
	P.w_u8		(m_tLocations[0]);
	P.w_u8		(m_tLocations[1]);
	P.w_u8		(m_tLocations[2]);
	P.w_u8		(m_tLocations[3]);
};
void xrGraphPoint::UPDATE_Read		(NET_Packet& P)				{}
void xrGraphPoint::UPDATE_Write		(NET_Packet& P)				{}

#ifdef _EDITOR              
void xrGraphPoint::FillProp			(LPCSTR pref, PropItemVec& items)
{
    CInifile *Ini 					= 0;
    if(locations[0].empty()||locations[1].empty()||locations[2].empty()||locations[3].empty()||level_ids.empty()){
	    string256 gm_name;
        FS.update_path				(gm_name,_game_data_,"game.ltx");
    	R_ASSERT2(FS.exist(gm_name),"Couldn't find file 'game.ltx'");
		Ini							= xr_new<CInifile>(gm_name);
    }
    for (int i=0; i<LOCATION_TYPE_COUNT; i++)
		if(locations[i].empty()){
			string256					caSection, T;
			strconcat					(caSection,SECTION_HEADER,itoa(i,T,10));
			R_ASSERT					(Ini->section_exist(caSection));
			LPCSTR						N,V;
			for (u32 k = 0; Ini->r_line(caSection,k,&N,&V); k++) {
   				locations[i].push_back(TokenValue4::Item());
				TokenValue4::Item& val	= locations[i].back();
				val.str					= V;
				val.ID					= atoi(N);
			}
		}
	if(level_ids.empty()){
		R_ASSERT					(Ini->section_exist("levels"));
        LPCSTR N,V;
        for (u32 k = 0; Ini->r_line("levels",k,&N,&V); k++) {
			R_ASSERT(Ini->section_exist(N));
   			level_ids.push_back		(TokenValue4::Item());
            TokenValue4::Item& val	= level_ids.back();
			LPCSTR					S = Ini->r_string(N,"caption");
			val.str					= S;
            val.ID					= Ini->r_u32(N,"id");
        }
    }
    if (Ini)	xr_delete(Ini);
	
	PHelper.CreateToken4	(items,	PHelper.PrepareKey(pref,s_name,"Location\\1"),				(u32*)&m_tLocations[0],			&locations[0],					1);
	PHelper.CreateToken4	(items,	PHelper.PrepareKey(pref,s_name,"Location\\2"),				(u32*)&m_tLocations[1],			&locations[1],					1);
	PHelper.CreateToken4	(items,	PHelper.PrepareKey(pref,s_name,"Location\\3"),				(u32*)&m_tLocations[2],			&locations[2],					1);
	PHelper.CreateToken4	(items,	PHelper.PrepareKey(pref,s_name,"Location\\4"),				(u32*)&m_tLocations[3],			&locations[3],					1);
	PHelper.CreateToken4	(items,	PHelper.PrepareKey(pref,s_name,"Connection\\Level name"),	(u32*)&m_tLevelID,				&level_ids,						1);
	PHelper.CreateText		(items,	PHelper.PrepareKey(pref,s_name,"Connection\\Point name"),	m_caConnectionPointName,		sizeof(m_caConnectionPointName));
}
#endif

xrSE_Human::xrSE_Human(LPCSTR caSection) : CALifeMonsterAbstract(caSection), CALifeTraderParams(caSection)
{
	set_visual						("actors\\Different_stalkers\\stalker_no_hood_singleplayer");
	// personal charactersitics
	fHealth							= 100;
}

void xrSE_Human::STATE_Read(NET_Packet& P, u16 size)
{
	// inherited properties
	inherited::STATE_Read(P,size);
	CALifeTraderParams::STATE_Read(P,size);
	// model
	visual_read(P);
	// personal characteristics
	if (m_wVersion <= 8)
		P.r_float (fHealth);
}

void xrSE_Human::STATE_Write(NET_Packet& P)
{
	// inherited properties
	inherited::STATE_Write(P);
	CALifeTraderParams::STATE_Write(P);
	// model
	visual_write(P);
}

void xrSE_Human::UPDATE_Read(NET_Packet& P)
{
	inherited::UPDATE_Read(P);
	CALifeTraderParams::UPDATE_Read(P);
}

void xrSE_Human::UPDATE_Write(NET_Packet& P)
{
	inherited::UPDATE_Write(P);
	CALifeTraderParams::UPDATE_Write(P);
}

#ifdef _EDITOR
void xrSE_Human::FillProp(LPCSTR pref, PropItemVec& items)
{
   	inherited::FillProp(pref, items);
	// model
	xrSE_Visualed::FillProp(pref, items);
}	
#endif


xrSE_Idol::xrSE_Idol(LPCSTR caSection) : xrSE_Human(caSection)
{
	m_dwAniPlayType		= 0;
	m_caAnimations[0]	= 0;
}

void xrSE_Idol::STATE_Read(NET_Packet& P, u16 size)
{
	inherited::STATE_Read(P,size);
	P.r_string			(m_caAnimations);
	P.r_u32				(m_dwAniPlayType);
}

void xrSE_Idol::STATE_Write(NET_Packet& P)
{
	inherited::STATE_Write(P);
	P.w_string	(m_caAnimations);
	P.w_u32		(m_dwAniPlayType);
}

void xrSE_Idol::UPDATE_Read(NET_Packet& P)
{
}

void xrSE_Idol::UPDATE_Write(NET_Packet& P)
{
}

#ifdef _EDITOR
void xrSE_Idol::FillProp(LPCSTR pref, PropItemVec& items)
{
   	inherited::FillProp		(pref, items);
    PHelper.CreateText		(items, PHelper.PrepareKey(pref,s_name,"Idol", "Animations"),m_caAnimations,sizeof(m_caAnimations));
   	PHelper.CreateU32		(items, PHelper.PrepareKey(pref,s_name,"Idol", "Animation playing type"),&m_dwAniPlayType,0,2,1);
}	
#endif

//***** Lamp
xrSE_HangingLamp::xrSE_HangingLamp(LPCSTR caSection) : xrServerEntity(caSection)
{
	flags.set				(flPhysic,TRUE);
    mass					= 10.f;
    startup_animation[0]	= 0;
	spot_texture[0]			= 0;
	color_animator[0]		= 0;
	spot_bone[0]			= 0;
	spot_range				= 10.f;
	spot_cone_angle			= PI_DIV_3;
	color					= 0xffffffff;
    spot_brightness			= 1.f;
}
xrSE_HangingLamp::~xrSE_HangingLamp()
{
}
void xrSE_HangingLamp::STATE_Read		(NET_Packet& P, u16 size)
{
	visual_read				(P);
	// model
	P.r_u32					(color);
	P.r_string				(color_animator);
	P.r_string				(spot_texture);
	P.r_string				(spot_bone);
	P.r_float				(spot_range);
	P.r_angle8				(spot_cone_angle);
    if (m_wVersion>10)
		P.r_float			(spot_brightness);
    if (m_wVersion>11)
    	P.r_u16				(flags.flags);
    if (m_wVersion>12)
    	P.r_float			(mass);
    if (m_wVersion>17)
		P.r_string			(startup_animation);

#ifdef _EDITOR    
	PlayAnimation			(startup_animation[0]?startup_animation:"$editor");
#endif

	// internal
	strlwr					(spot_bone);
}
void xrSE_HangingLamp::STATE_Write		(NET_Packet& P)
{
	visual_write			(P);
	// model
	P.w_u32					(color);
	P.w_string				(color_animator);
	P.w_string				(spot_texture);
	P.w_string				(spot_bone);
	P.w_float				(spot_range);
	P.w_angle8				(spot_cone_angle);
	P.w_float				(spot_brightness);
   	P.w_u16					(flags.flags);
	P.w_float				(mass);
	P.w_string				(startup_animation);
}
void xrSE_HangingLamp::UPDATE_Read		(NET_Packet& P)	{};
void xrSE_HangingLamp::UPDATE_Write		(NET_Packet& P)	{};
#ifdef _EDITOR
void __fastcall	xrSE_HangingLamp::OnChangeAnim(PropValue* sender)
{
	PlayAnimation			(startup_animation);
}
void	xrSE_HangingLamp::FillProp		(LPCSTR pref, PropItemVec& values)
{
	inherited::FillProp		(pref,values);
	xrSE_Visualed::FillProp	(PHelper.PrepareKey(pref,s_name),values);
	PHelper.CreateColor		(values, PHelper.PrepareKey(pref,s_name,"Color"),			&color);
	PHelper.CreateFlag16	(values, PHelper.PrepareKey(pref,s_name,"Physic"),			&flags,				flPhysic);
	PHelper.CreateLightAnim	(values, PHelper.PrepareKey(pref,s_name,"Color animator"),	color_animator,		sizeof(color_animator));
	PHelper.CreateTexture	(values, PHelper.PrepareKey(pref,s_name,"Texture"),			spot_texture,		sizeof(spot_texture));
	PHelper.CreateFloat		(values, PHelper.PrepareKey(pref,s_name,"Range"),			&spot_range,		0.1f, 1000.f);
	PHelper.CreateAngle		(values, PHelper.PrepareKey(pref,s_name,"Angle"),			&spot_cone_angle,	0, deg2rad(120.f));
    PHelper.CreateFloat		(values, PHelper.PrepareKey(pref,s_name,"Brightness"),		&spot_brightness,	0.1f, 5.f);
    PHelper.CreateFloat		(values, PHelper.PrepareKey(pref,s_name,"Mass"),			&mass,				1.f, 1000.f);

    if (visual&&PKinematics(visual))
    {
    	CKinematics::accel*	ll_bones		= PKinematics(visual)->LL_Bones();
    	CKinematics::accel* ll_motions		= PKinematics(visual)->LL_Motions();
        CKinematics::accel::iterator _I, _E;
        AStringVec vec;
        bool bFound;
        // bones
        _I=ll_motions->begin();
        _E=ll_motions->end();
        bFound = false;
        for (; _I!=_E; _I++){ vec.push_back(_I->first); if (!bFound&&startup_animation[0]&&(vec.back()==startup_animation)) bFound=true; }
        if (!bFound) startup_animation[0]=0;
        PropValue* P = PHelper.CreateList	(values,	PHelper.PrepareKey(pref,s_name,"Startup animation"), startup_animation, &vec);
        P->SetEvents		(0,0,OnChangeAnim);
		// motions
        vec.clear();
        _I=ll_bones->begin();
        _E=ll_bones->end();
        bFound = false;
        for (; _I!=_E; _I++){ vec.push_back(_I->first); if (!bFound&&spot_bone[0]&&(vec.back()==spot_bone)) bFound=true; }
        if (!bFound) spot_bone[0]=0;
		PHelper.CreateList	(values, PHelper.PrepareKey(pref,s_name,"Guide bone"),		spot_bone,			&vec);
    }
}
#endif

xrSE_DeviceTorch::xrSE_DeviceTorch(LPCSTR caSection) : CALifeItem(caSection), xrSE_Visualed("lights\\lights_torch")
{
	strcpy					(spot_texture,"");
	strcpy					(animator,"");
	spot_range				= 10.f;
	spot_cone_angle			= PI_DIV_3;
	color					= 0xffffffff;
    spot_brightness			= 1.f;
}
xrSE_DeviceTorch::~xrSE_DeviceTorch()
{
}
void xrSE_DeviceTorch::STATE_Read		(NET_Packet& P, u16 size)
{
	visual_read				(P);
	// model
	P.r_u32					(color);
	P.r_string				(animator);
	P.r_string				(spot_texture);
	P.r_float				(spot_range);
	P.r_angle8				(spot_cone_angle);
	P.r_float				(spot_brightness);
}
void xrSE_DeviceTorch::STATE_Write		(NET_Packet& P)
{
	visual_write			(P);
	// model
	P.w_u32					(color);
	P.w_string				(animator);
	P.w_string				(spot_texture);
	P.w_float				(spot_range);
	P.w_angle8				(spot_cone_angle);
	P.w_float				(spot_brightness);
}
void xrSE_DeviceTorch::UPDATE_Read		(NET_Packet& P)	{};
void xrSE_DeviceTorch::UPDATE_Write		(NET_Packet& P)	{};
#ifdef _EDITOR
void	xrSE_DeviceTorch::FillProp		(LPCSTR pref, PropItemVec& values)
{
	inherited::FillProp		(pref,values);
	xrSE_Visualed::FillProp	(PHelper.PrepareKey(pref,s_name),values);
	PHelper.CreateColor		(values, PHelper.PrepareKey(pref,s_name,"Color"),			&color);
	PHelper.CreateLightAnim	(values, PHelper.PrepareKey(pref,s_name,"Color animator"),	animator,			sizeof(animator));
	PHelper.CreateTexture	(values, PHelper.PrepareKey(pref,s_name,"Texture"),			spot_texture,		sizeof(spot_texture));
	PHelper.CreateFloat		(values, PHelper.PrepareKey(pref,s_name,"Range"),			&spot_range,		0.1f, 1000.f);
	PHelper.CreateAngle		(values, PHelper.PrepareKey(pref,s_name,"Angle"),			&spot_cone_angle,	0, PI_DIV_2);
    PHelper.CreateFloat		(values, PHelper.PrepareKey(pref,s_name,"Brightness"),		&spot_brightness,	0.1f, 5.f);
}
#endif
//--------------------------------------------------------------------

//***** Physic Object
xrSE_PhysicObject::xrSE_PhysicObject(LPCSTR caSection) : CALifeDynamicObject(caSection) 
{
	type 		= epotBox;
	mass 		= 10.f;
    fixed_bone[0]=0;
	if (pSettings->section_exist(caSection) && pSettings->line_exist(caSection,"visual"))
    	set_visual			(pSettings->r_string(caSection,"visual"));
}
xrSE_PhysicObject::~xrSE_PhysicObject() 
{
}
void xrSE_PhysicObject::STATE_Read		(NET_Packet& P, u16 size) 
{
	if (m_wVersion >= 14)
		if (m_wVersion >= 16)
			inherited::STATE_Read	(P,size);
		else
			CALifeObject::STATE_Read(P,size);
	visual_read				(P);
	P.r_u32					(type);
	P.r_float				(mass);
    if (m_wVersion>9){
		P.r_string			(fixed_bone);
    }
	// internal
	strlwr					(fixed_bone);
}
void xrSE_PhysicObject::STATE_Write		(NET_Packet& P)
{
	inherited::STATE_Write	(P);
	visual_write			(P);
	P.w_u32					(type);
	P.w_float				(mass);
	P.w_string				(fixed_bone);
}
void xrSE_PhysicObject::UPDATE_Read		(NET_Packet& P)	{inherited::UPDATE_Read(P);};
void xrSE_PhysicObject::UPDATE_Write	(NET_Packet& P)	{inherited::UPDATE_Write(P);};
#ifdef _EDITOR
xr_token po_types[]={
	{ "Box",			epotBox			},
	{ "Fixed chain",	epotFixedChain	},
	{ "Free chain",		epotFreeChain	},
	{ "Skeleton",		epotSkeleton	},
	{ 0,				0				}
};
void	xrSE_PhysicObject::FillProp		(LPCSTR pref, PropItemVec& values) {
	inherited::FillProp(pref,values);
	xrSE_Visualed::FillProp	(PHelper.PrepareKey(pref,s_name),values);
	PHelper.CreateToken		(values, PHelper.PrepareKey(pref,s_name,"Type"), &type,	po_types, 1);
	PHelper.CreateFloat		(values, PHelper.PrepareKey(pref,s_name,"Mass"), &mass, 0.1f, 10000.f);
	PHelper.CreateText		(values, PHelper.PrepareKey(pref,s_name,"Fixed bone"),	fixed_bone,	sizeof(fixed_bone));
}
#endif

// temporary object for WT
xrSE_TempObject::xrSE_TempObject(LPCSTR caSection) : xrServerEntity(caSection) {}
void xrSE_TempObject::STATE_Read	(NET_Packet& P, u16 size) {};
void xrSE_TempObject::STATE_Write	(NET_Packet& P) {};
void xrSE_TempObject::UPDATE_Read	(NET_Packet& P)	{};
void xrSE_TempObject::UPDATE_Write	(NET_Packet& P)	{};
#ifdef _EDITOR
void xrSE_TempObject::FillProp		(LPCSTR pref, PropItemVec& values) {};
#endif

//--------------------------------------------------------------------
xrServerEntity*	F_entity_Create		(LPCSTR caSection)
{
	if (!pSettings->section_exist(caSection)) return 0;
    
	CLASS_ID cls = pSettings->r_clsid(caSection,"class");

	switch (cls){
	case CLSID_OBJECT_ACTOR:		return xr_new<xrSE_Actor>			(caSection);          	// Visualed
	case CLSID_OBJECT_DUMMY:		return xr_new<xrSE_Dummy>			(caSection);
	case CLSID_OBJECT_HLAMP:		return xr_new<xrSE_HangingLamp>		(caSection);            // Visualed
	case CLSID_AI_GRAPH:			return xr_new<xrGraphPoint>			(caSection);			
	case CLSID_AI_CROW:				return xr_new<xrSE_Crow>			(caSection);            // Visualed
	case CLSID_AI_RAT:				return xr_new<xrSE_Rat>				(caSection);			// Visualed
	case CLSID_AI_RAT_GROUP:		return xr_new<CALifeGroupTemplate<xrSE_Rat> >(caSection);	
	case CLSID_AI_FLESH:			return xr_new<xrSE_Biting>			(caSection);
	case CLSID_AI_FLESH_GROUP:		return xr_new<CALifeGroupTemplate<xrSE_Biting> >(caSection);
	case CLSID_AI_ZOMBIE:			return xr_new<xrSE_Zombie>			(caSection);            // Visualed
	case CLSID_AI_DOG:				return xr_new<xrSE_Dog>				(caSection);			// Visualed
	case CLSID_AI_SOLDIER:			return xr_new<xrSE_Enemy>			(caSection);           	// Visualed
	case CLSID_AI_STALKER:			return xr_new<xrSE_Human>			(caSection);			// Visualed
	case CLSID_AI_IDOL:				return xr_new<xrSE_Idol>			(caSection);			// Visualed
	case CLSID_EVENT:				return xr_new<xrSE_Event>			(caSection);
	case CLSID_CAR_NIVA:			return xr_new<xrSE_Car>				(caSection);			// Visualed
	case CLSID_SPECTATOR:			return xr_new<xrSE_Spectator>		(caSection);

	// Artifacts
	case CLSID_AF_MERCURY_BALL:		return xr_new<xrSE_MercuryBall>		(caSection);
	case CLSID_ARTIFACT:			return xr_new<CALifeItem>			(caSection);

	// Zones
	case CLSID_ZONE:				return xr_new<xrSE_Zone>			(caSection);
	case CLSID_Z_MBALD:				return xr_new<xrSE_Zone>			(caSection);
	case CLSID_Z_MINCER:			return xr_new<xrSE_Zone>			(caSection);

	// Detectors
	case CLSID_DETECTOR_SIMPLE:		return xr_new<xrSE_Detector>		(caSection);

	case CLSID_OBJECT_W_M134:					
	case CLSID_OBJECT_W_FN2000:		
	case CLSID_OBJECT_W_AK74:		
	case CLSID_OBJECT_W_LR300:		
	case CLSID_OBJECT_W_HPSA:		
	case CLSID_OBJECT_W_PM:			
	case CLSID_OBJECT_W_VAL:		
	case CLSID_OBJECT_W_VINTOREZ:	
	case CLSID_OBJECT_W_WALTHER:	
	case CLSID_OBJECT_W_USP45:		
	case CLSID_OBJECT_W_GROZA:		
	case CLSID_OBJECT_W_FORT:		
	case CLSID_OBJECT_W_BINOCULAR:	
	case CLSID_OBJECT_W_SHOTGUN:	
	case CLSID_OBJECT_W_SVD:		
	case CLSID_OBJECT_W_SVU:		
	case CLSID_OBJECT_W_RPG7:		return xr_new<xrSE_Weapon>			(caSection);  			// Visualed
	case CLSID_OBJECT_A_PM:			return xr_new<xrSE_WeaponAmmo>		(caSection);  			// Visualed
	case CLSID_OBJECT_G_RPG7:		return xr_new<xrSE_TempObject>		(caSection);
	case CLSID_OBJECT_G_FAKE:		return xr_new<xrSE_TempObject>		(caSection);

	case CLSID_OBJECT_HEALTH:		return xr_new<xrSE_Health>			(caSection);
	case CLSID_TARGET_ASSAULT:		return xr_new<xrSE_Target_Assault>	(caSection);
	case CLSID_TARGET_CS_BASE:		return xr_new<xrSE_Target_CSBase>	(caSection);
	case CLSID_TARGET_CS:			return xr_new<xrSE_Target_CS>		(caSection);
	case CLSID_TARGET_CS_CASK:		return xr_new<xrSE_Target_CSCask>	(caSection);
	case CLSID_IITEM_BOLT:			return xr_new<CALifeDynamicObject>	(caSection);
	case CLSID_GRENADE_F1:			return xr_new<CALifeItem>			(caSection);
	case CLSID_GRENADE_RGD5:		return xr_new<CALifeItem>			(caSection);
	case CLSID_DEVICE_TORCH:		return xr_new<xrSE_DeviceTorch>		(caSection);			// Visualed
    case CLSID_OBJECT_PHYSIC:		return xr_new<xrSE_PhysicObject>	(caSection);            // Visualed
    default: NODEFAULT;
	}
	return 0;
}
