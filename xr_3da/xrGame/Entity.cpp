// Entity.cpp: implementation of the CEntity class.

//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "hudmanager.h"
#include "Entity.h"
#include "actor.h"
#include "xrserver_objects_alife_monsters.h"

#define MAX_ARMOR		200
#define MAX_HEALTH		100

#define BODY_REMOVE_TIME 100000

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEntity::CEntity()
{
	fAccuracy			= 1.f;
	fMAX_Health			= MAX_HEALTH;
	fMAX_Armor			= MAX_ARMOR;

	m_iTradeIconX = m_iTradeIconY = 0;
	m_iMapIconX = m_iMapIconY = 0;

	m_bBodyRemoved = false;
	
}

CEntity::~CEntity()
{
	m_bBodyRemoved = false;
	m_dwDeathTime = 0;
}

void CEntity::OnEvent		(NET_Packet& P, u16 type)
{
	inherited::OnEvent		(P,type);

	switch (type)
	{

	case GE_DIE:
		{
			u16				id;
			u32				cl;
			P.r_u16			(id);
			P.r_u32			(cl);
			CObject* who	= Level().Objects.net_Find	(id);
			if (this!=who)	if(bDebug) HUD().outMessage	(0xffffffff,cName(),"Killed by '%s'...",who->cName());
			else			if(bDebug) HUD().outMessage	(0xffffffff,cName(),"Crashed...");
			Die				();
		}
		break;
	}
}

void CEntity::Die() 
{
}

//���������� ���������
float CEntity::CalcCondition(float hit)
{
	// Calc amount (correct only on local player)
	float	lost_health,		lost_armor;

	if (fArmor>0)
	{
		lost_health		=	(fMAX_Armor-fArmor)/fMAX_Armor*hit;
		lost_armor		=	(hit*9.f)/10.f;
	} 
	else 
	{
		lost_health		=	hit;
		lost_armor		=	0;
	}

	// If Local() - perform some logic
	if (Local() && g_Alive()) {
		fEntityHealth		-=	lost_health; 
		fEntityHealth		=	fEntityHealth<-1000?-1000:fEntityHealth;
		fArmor				-=	lost_armor;
	}

	return lost_health;
}

void CEntity::Hit			(float perc, Fvector &dir, CObject* who, s16 element,Fvector position_in_object_space, float impulse, ALife::EHitType hit_type) 
{
	inherited::Hit(perc,dir,who,element,position_in_object_space,impulse, hit_type);
	if (bDebug)				Log("Process HIT: ", *cName());

	// *** process hit calculations
	// Calc impulse
	Fvector					vLocalDir;
	float					m = dir.magnitude();
	R_ASSERT				(m>EPS);
	
	// convert impulse into local coordinate system
	Fmatrix					mInvXForm;
	mInvXForm.invert		(XFORM());
	mInvXForm.transform_dir	(vLocalDir,dir);
	vLocalDir.invert		();

	// hit impulse
	if(impulse) HitImpulse				(impulse,dir,vLocalDir); // @@@: WT
	
	// Calc amount (correct only on local player)
	float lost_health = CalcCondition(perc);

	// Signal hit
	if(-1!=element)	HitSignal(lost_health,vLocalDir,who,element);

	// If Local() - perform some logic
	if (Local() && !g_Alive() && !AlreadyDie())
		KillEntity	(who);
}

void CEntity::Load		(LPCSTR section)
{
	inherited::Load		(section);

	setVisible			(FALSE);
	CLS_ID				= CLSID_ENTITY;
	
	m_fMaxHealthValue = fEntityHealth = 100;
	
	//����� �������� ���� � ������
	if(pSettings->line_exist(section,"body_remove_time"))
		m_dwBodyRemoveTime = pSettings->r_u32(section,"body_remove_time");
	else
		m_dwBodyRemoveTime = BODY_REMOVE_TIME;


	// Team params
	id_Team = -1; if (pSettings->line_exist(section,"team"))	id_Team		= pSettings->r_s32	(section,"team");
	id_Squad= -1; if (pSettings->line_exist(section,"squad"))	id_Squad	= pSettings->r_s32	(section,"squad");
	id_Group= -1; if (pSettings->line_exist(section,"group"))	id_Group	= pSettings->r_s32	(section,"group");
	
#pragma todo("Jim to Dima: no specific figures or comments needed")	
	m_fMorale = 66.f;


	//��������� ��������� ������ ��������
	CKinematics* pKinematics=PKinematics(Visual());
	CInifile* ini = NULL;

/*			ref_str	first;
		ref_str	second;
		ref_str	comment;

		Item() : first(0), second(0), comment(0) {};
	};
	typedef xr_vector<Item>			Items;
	typedef Items::iterator			SectIt;
    struct XRCORE_API Sect {
		ref_str			Name;
		Items			Data;

		IC SectIt		begin()		{ return Data.begin();	}
		IC SectIt		end()		{ return Data.end();	}
		IC size_t		size()		{ return Data.size();	}
		IC void			clear()		{ Data.clear();			}
	    BOOL			line_exist	(LPCSTR L, LPCSTR* val=0);
*/
	
	if(pKinematics) ini = pKinematics->LL_UserData();
	if(ini)
	{
		if(ini->section_exist("icon"))
		{
			m_iTradeIconX = ini->r_u32("icon","icon_x");
			m_iTradeIconY = ini->r_u32("icon","icon_y");

			m_iMapIconX = ini->r_u32("icon","map_icon_x");
			m_iMapIconY = ini->r_u32("icon","map_icon_y");
		}
		else
		{
			m_iTradeIconX = m_iTradeIconY = 0;
			m_iMapIconX = 1;
			m_iMapIconY = 4;
		}

		//������� ������ �������� � ���������������
		//�������  ���� ����� ������ ��������
		if(ini->section_exist("particles_bones"))
		{
			m_ParticlesBonesList.clear();

			SBoneInfo bone_info;
			string256 str_buf;

			CInifile::Sect& sect = ini->r_section("particles_bones");

			for(u32 i=0; i<sect.size(); i+=1)
			{
				CInifile::Item& item = *(sect.begin()+i);
				
				if(!xr_strcmp(*item.first,"bone"))
				{
					bone_info.index = pKinematics->LL_BoneID(_GetItem(*item.second,0,str_buf));
					R_ASSERT3(bone_info.index != BI_NONE, "Particles bone not found", _GetItem(*item.second,0,str_buf));

					bone_info.offset.x = (float)atof(_GetItem(*item.second,1,str_buf));
					bone_info.offset.y = (float)atof(_GetItem(*item.second,2,str_buf));
					bone_info.offset.z = (float)atof(_GetItem(*item.second,3,str_buf));

				}
				R_ASSERT2(!xr_strcmp(*item.first,"bone"), "wrong format for 'particles bones section'");
				m_ParticlesBonesList.push_back(bone_info);
			}
		}
	}
}

BOOL CEntity::net_Spawn		(LPVOID DC)
{
	if (!inherited::net_Spawn(DC))
		return				(FALSE);
		
	CSE_Abstract			*e	= (CSE_Abstract*)(DC);
	CSE_ALifeCreatureAbstract	*E	= dynamic_cast<CSE_ALifeCreatureAbstract*>(e);
	if (!E) {
		// Car or trader only!!!!
		CSE_ALifeItemCar	*C	= dynamic_cast<CSE_ALifeItemCar*>(e);
		CSE_ALifeTrader		*T	= dynamic_cast<CSE_ALifeTrader*>(e);
		R_ASSERT2			(C || T,"Invalid entity (no inheritance from CSE_CreatureAbstract, CSE_ALifeItemCar and CSE_ALifeTrader)!");
		id_Team				= id_Squad = id_Group = 0;
	}
	else {
		id_Team				= E->g_team();
		id_Squad			= E->g_squad();
		id_Group			= E->g_group();
	}

	// Register
	CSquad& S				= Level().get_squad	(id_Team,id_Squad);
	CGroup& G				= Level().get_group	(id_Team,id_Squad,id_Group);

	G.Member_Add			(this);
	if (S.Leader==0)		S.Leader			=this;
	++G.m_dwAliveCount;
	
	CActor	*pA = dynamic_cast<CActor*>(this);
	if (!pA) {
		Level().SquadMan.RegisterMember((u8)id_Squad, this);
		Level().SquadMan.Dump();
	}

	// Initialize variables
	fEntityHealth			= 100;
	fArmor					= 0;
	m_bBodyRemoved			= false;
	m_dwDeathTime			= 0;
	
	Engine.Sheduler.Unregister	(this);
	Engine.Sheduler.Register	(this);

	return					TRUE;
}

void CEntity::net_Destroy	()
{
	CSquad& S			= Level().get_squad	(g_Team(),g_Squad());
	CGroup& G			= Level().get_group	(g_Team(),g_Squad(),g_Group());
	G.Member_Remove		(this);
	if (this==S.Leader)	
	{
		S.Leader		= 0;
		if (!G.Members.empty())	S.Leader	= G.Members.back();
	}

	CActor	*pA = dynamic_cast<CActor*>(this);
	if (!pA) {
		Level().SquadMan.RemoveMember((u8)g_Squad(), this);
		Level().SquadMan.Dump();
	}

	inherited::net_Destroy	();
}

void CEntity::renderable_Render()
{
	inherited::renderable_Render		();
}

void CEntity::shedule_Update	(u32 dt)
{
	//����������
	if(NeedToDestroyEntity())
        DestroyEntity();

	inherited::shedule_Update	(dt);
}
bool CEntity::NeedToDestroyEntity()	
{
	return false;
}
void CEntity::DestroyEntity()			
{
	if(m_bBodyRemoved) return;
	m_bBodyRemoved = true;

	NET_Packet			P;
	u_EventGen			(P,GE_DESTROY,ID());
	Msg					("DestroyEntity: ge_destroy: [%d] - %s",ID(),*cName());
	if (Local()) u_EventSend			(P);
}
ALife::_TIME_ID	 CEntity::TimePassedAfterDeath()	
{
	if(!g_Alive())
		return Level().GetGameTime() - m_dwDeathTime;
	else
		return 0;
}

void CEntity::KillEntity(CObject* who)
{
	NET_Packet		P;
	u_EventGen		(P,GE_DIE,ID());
	P.w_u16			(u16(who->ID()));
	P.w_u32			(0);
	if (OnServer())
		u_EventSend		(P);

	m_dwDeathTime = Level().GetGameTime();
}

void CEntity::reinit			()
{
	inherited::reinit			();
	m_dwDeathTime			= 0;
}
