// Actor_Network.cpp:	 для работы с сетью
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "actor.h"
#include "Actor_Flags.h"
#include "inventory.h"
#include "xrserver_objects_alife_monsters.h"
#include "xrServer.h"

#include "CameraLook.h"
#include "CameraFirstEye.h"

#include "ActorEffector.h"

#include "PHWorld.h"
#include "level.h"
#include "xr_level_controller.h"
#include "game_cl_base.h"
#include "infoportion.h"

//static u32	g_dwStartTime		= 0;
//static u32	g_dwLastUpdateTime	;
//static u32	g_dwNumUpdates		= 0;
//float		g_fNumUpdates		= 0;
int			g_cl_InterpolationType = 0;
u32			g_cl_InterpolationMaxPoints = 0;
int			g_dwInputUpdateDelta	= 20;
BOOL		net_cl_inputguaranteed	= FALSE;

//--------------------------------------------------------------------
void CActor::net_Export	(NET_Packet& P)					// export to server
{
	u8					flags = 0;
	P.w_float_q16		(g_Health(),-500,1000);

	P.w_u32				(Level().timeServer());
	P.w_u8				(flags);
	Fvector				p = Position();
	P.w_vec3			(p);//Position());
	P.w_angle8			(r_model_yaw);
	P.w_angle8			(unaffected_r_torso_yaw	);//(r_torso.yaw);
	P.w_angle8			(unaffected_r_torso_pitch);//(r_torso.pitch);
	P.w_u8				(u8(g_Team()));
	P.w_u8				(u8(g_Squad()));
	P.w_u8				(u8(g_Group()));

	//	unaffected_r_torso_yaw	 = r_torso.yaw;
	//	unaffected_r_torso_pitch = r_torso.pitch;

	P.w_float			(inventory().TotalWeight());

	P.w_u32				(m_dwMoney);
	P.w_u32				(m_tRank);

	u16 ms	= (u16)(mstate_real & 0x0000ffff);
	P.w_u16				(u16(ms));
	P.w_sdir			(NET_SavedAccel);
	Fvector				v = m_PhysicMovementControl->GetVelocity();
	P.w_sdir			(v);//m_PhysicMovementControl.GetVelocity());
	P.w_float_q16		(fArmor,-500,1000);

	P.w_u8				(u8(inventory().GetActiveSlot()));

	/////////////////////////////////////////////////
	u16 NumItems		= PHGetSyncItemsNumber();
	if (H_Parent() || GameID() == 1 || (!g_Alive() && (NumItems<2 || OnClient()))) 
		NumItems = 0;
	if (!g_Alive()) NumItems = 0;
	
	P.w_u16				(NumItems);
	if (!NumItems)		return;

	if (g_Alive())
	{
		SPHNetState	State;

		CPHSynchronize* pSyncObj = NULL;
		pSyncObj = PHGetSyncItem(0);
		pSyncObj->get_State(State);

		P.w_u8					( State.enabled );

		P.w_vec3				( State.angular_vel);
		P.w_vec3				( State.linear_vel);

		P.w_vec3				( State.force);
		P.w_vec3				( State.torque);

		P.w_vec3				( State.position);

		P.w_float				( State.quaternion.x );
		P.w_float				( State.quaternion.y );
		P.w_float				( State.quaternion.z );
		P.w_float				( State.quaternion.w );
	}
	else
	{
		net_ExportDeadBody(P);
	};
};

static void w_vec_q8(NET_Packet& P,const Fvector& vec,const Fvector& min,const Fvector& max)
{
	P.w_float_q8(vec.x,min.x,max.x);
	P.w_float_q8(vec.y,min.y,max.y);
	P.w_float_q8(vec.z,min.z,max.z);
}
static void r_vec_q8(NET_Packet& P,Fvector& vec,const Fvector& min,const Fvector& max)
{
	P.r_float_q8(vec.x,min.x,max.x);
	P.r_float_q8(vec.y,min.y,max.y);
	P.r_float_q8(vec.z,min.z,max.z);

	clamp(vec.x,min.x,max.x);
	clamp(vec.y,min.y,max.y);
	clamp(vec.z,min.z,max.z);
}
static void w_qt_q8(NET_Packet& P,const Fquaternion& q)
{
	//Fvector Q;
	//Q.set(q.x,q.y,q.z);
	//if(q.w<0.f)	Q.invert();
	//P.w_float_q8(Q.x,-1.f,1.f);
	//P.w_float_q8(Q.y,-1.f,1.f);
	//P.w_float_q8(Q.z,-1.f,1.f);
	///////////////////////////////////////////////////
	P.w_float_q8(q.x,-1.f,1.f);
	P.w_float_q8(q.y,-1.f,1.f);
	P.w_float_q8(q.z,-1.f,1.f);
	P.w_float_q8(q.w,-1.f,1.f);

	///////////////////////////////////////////


	//P.w_float_q8(q.x,-1.f,1.f);
	//P.w_float_q8(q.y,-1.f,1.f);
	//P.w_float_q8(q.z,-1.f,1.f);
	//P.w(sign())
}
static void r_qt_q8(NET_Packet& P,Fquaternion& q)
{
	//// x^2 + y^2 + z^2 + w^2 = 1
	//P.r_float_q8(q.x,-1.f,1.f);
	//P.r_float_q8(q.y,-1.f,1.f);
	//P.r_float_q8(q.z,-1.f,1.f);
	//float w2=1.f-q.x*q.x-q.y*q.y-q.z*q.z;
	//w2=w2<0.f ? 0.f : w2;
	//q.w=_sqrt(w2);
	/////////////////////////////////////////////////////
	///////////////////////////////////////////////////
	P.r_float_q8(q.x,-1.f,1.f);
	P.r_float_q8(q.y,-1.f,1.f);
	P.r_float_q8(q.z,-1.f,1.f);
	P.r_float_q8(q.w,-1.f,1.f);

	clamp(q.x,-1.f,1.f);
	clamp(q.y,-1.f,1.f);
	clamp(q.z,-1.f,1.f);
	clamp(q.w,-1.f,1.f);
}

#define F_MAX         3.402823466e+38F

static void	UpdateLimits (Fvector &p, Fvector& min, Fvector& max)
{
	if(p.x<min.x)min.x=p.x;
	if(p.y<min.y)min.y=p.y;
	if(p.z<min.z)min.z=p.z;

	if(p.x>max.x)max.x=p.x;
	if(p.y>max.y)max.y=p.y;
	if(p.z>max.z)max.z=p.z;

	for (int k=0; k<3; k++)
	{
		if (p[k]<min[k] || p[k]>max[k])
		{
			R_ASSERT2(0, "Fuck");
			UpdateLimits(p, min, max);
		}
	}
};

void		CActor::net_ExportDeadBody		(NET_Packet &P)
{
	/////////////////////////////
	Fvector min,max;

	min.set(F_MAX,F_MAX,F_MAX);
	max.set(-F_MAX,-F_MAX,-F_MAX);
	/////////////////////////////////////
	u16 bones_number		= PHGetSyncItemsNumber();
	for(u16 i=0;i<bones_number;i++)
	{
		SPHNetState state;
		PHGetSyncItem(i)->get_State(state);

		Fvector& p=state.position;
		UpdateLimits (p, min, max);

		Fvector px =state.linear_vel;
		px.div(10.0f);
		px.add(state.position);
		UpdateLimits (px, min, max);
	};

	P.w_u8(10);
	P.w_vec3(min);
	P.w_vec3(max);

	for(u16 i=0;i<bones_number;i++)
	{
		SPHNetState state;
		PHGetSyncItem(i)->get_State(state);
//		state.net_Save(P,min,max);
		w_vec_q8(P,state.position,min,max);
		w_qt_q8(P,state.quaternion);

		//---------------------------------
		Fvector px =state.linear_vel;
		px.div(10.0f);
		px.add(state.position);
		w_vec_q8(P,px,min,max);
	};	
};

void CActor::net_Import		(NET_Packet& P)					// import from server
{
	//-----------------------------------------------
	net_Import_Base(P);
	//-----------------------------------------------

	m_u16NumBones = P.r_u16();
	if (m_u16NumBones == 0) return;
	//-----------------------------------------------
	net_Import_Physic(P);
	//-----------------------------------------------
};

void		CActor::net_Import_Base				( NET_Packet& P)
{
	net_update			N;

	u8					flags;
	u16					tmp;

	float health;
	P.r_float_q16 (health,-500,1000);
	//----------- for E3 -----------------------------
	if (OnClient())fEntityHealth = health;
	//------------------------------------------------

	float				fDummy;
	//u32					dwDummy;

	P.r_u32				(N.dwTimeStamp	);
	
	//---------------------------------------------
	if (pStatGraph) 
	{
		pStatGraph->SetMinMax(-0.0f, 100.0f, 300);
		pStatGraph->SetGrid(0, 0.0f, 10, 10.0f, 0xff808080, 0xffffffff);
		pStatGraph->SetStyle(CStatGraph::stBar);
		pStatGraph->SetStyle(CStatGraph::stBar, 1);
		pStatGraph->SetStyle(CStatGraph::stBar, 2);

		float dTime = 0;
		if (Level().timeServer() < N.dwTimeStamp)
			dTime =  -float(N.dwTimeStamp - Level().timeServer());
		else
			dTime = float(Level().timeServer() - N.dwTimeStamp);

		if (OnServer())
		{
			
			for (u32 I=0; I<Level().Server->client_Count(); ++I)	
			{
				game_PlayerState*	ps = Level().Server->game->get_it(I);
				if (!ps || ps->GameID != ID()) continue;
				IClient*	C = Level().Server->client_Get(I);
				if (!C) continue;
				pStatGraph->AppendItem(float(C->stats.getPing()), 0xff00ff00, 2);
				pStatGraph->AppendItem(float(Level().timeServer() - P.timeReceive + C->stats.getPing()), 0xffffff00, 0);
			};
		}
		else
		{
			pStatGraph->SetStyle(CStatGraph::stBar);
			pStatGraph->SetStyle(CStatGraph::stBar, 1);
			pStatGraph->SetStyle(CStatGraph::stBar, 2);

			IClientStatistic pStat = Level().GetStatistic();
			pStatGraph->AppendItem(float(pStat.getPing()), 0xff00ff00, 2);
			pStatGraph->AppendItem(float(Level().timeServer() - P.timeReceive + pStat.getPing()), 0xffffff00, 0);
		};
	};
	//---------------------------------------------

	P.r_u8				(flags			);
	P.r_vec3			(N.p_pos		);
	P.r_angle8			(N.o_model		);
	P.r_angle8			(N.o_torso.yaw	);
	P.r_angle8			(N.o_torso.pitch);
	id_Team				= P.r_u8();
	id_Squad			= P.r_u8();
	id_Group			= P.r_u8();
	
	
	//----------- for E3 -----------------------------
	if (OnClient())
	//------------------------------------------------
	{
		unaffected_r_torso_yaw	 = N.o_torso.yaw;
		unaffected_r_torso_pitch = N.o_torso.pitch;
	};

	P.r_float			(fDummy);
	m_dwMoney =			P.r_u32();
	m_tRank	  =			ALife::EStalkerRank(P.r_u32());

	P.r_u16				(tmp			); N.mstate = u32(tmp);
	P.r_sdir			(N.p_accel		);
	P.r_sdir			(N.p_velocity	);
	float				fRArmor;
	P.r_float_q16		(fRArmor,-500,1000);
	//----------- for E3 -----------------------------
	if (OnClient())		fArmor = fRArmor;
	//------------------------------------------------

	u8					ActiveSlot;
	P.r_u8				(ActiveSlot);
	
	//----------- for E3 -----------------------------
	if (OnClient())
	//------------------------------------------------
	{
		if (ActiveSlot == 0xff) inventory().SetActiveSlot(NO_ACTIVE_SLOT);
		else 
		{
			if (inventory().GetActiveSlot() != u32(ActiveSlot))
				inventory().Activate(u32(ActiveSlot));
		};
	}

	//----------- for E3 -----------------------------
	if (Local() && OnClient()) return;
	//-------------------------------------------------
	if (!NET.empty() && N.dwTimeStamp < NET.back().dwTimeStamp) return;

	if (!NET.empty() && N.dwTimeStamp == NET.back().dwTimeStamp)
	{
		NET.back() = N;
	}
	else
	{
		NET.push_back			(N);
		if (NET.size()>5) NET.pop_front();
	}	
	//-----------------------------------------------
	net_Import_Base_proceed	();
	//-----------------------------------------------
};

void	CActor::net_Import_Base_proceed		( )
{
	setVisible					(TRUE);
	setEnabled					(TRUE);
	//---------------------------------------------
		
	if (Remote()) return;

	net_update N		= NET.back();
};

void		CActor::net_Import_Physic			( NET_Packet& P)
{
	m_States.clear();
	if (m_u16NumBones != 1)
	{
		Fvector min, max;

		P.r_u8();
		P.r_vec3(min);
		P.r_vec3(max);
		
		for (u16 i=0; i<m_u16NumBones; i++)
		{
			SPHNetState state, stateL;
			PHGetSyncItem(i)->get_State(state);
//			stateL.net_Load(P, min, max);
			r_vec_q8(P, stateL.position, min, max);
			r_qt_q8(P, stateL.quaternion);
			//---------------------------------------
			r_vec_q8(P, stateL.linear_vel, min, max);
			stateL.linear_vel.sub(stateL.position);
			stateL.linear_vel.mul(10.0f);
			//---------------------------------------
			state.position				= stateL.position;
			state.previous_position		= stateL.position;
			state.quaternion			= stateL.quaternion;
			state.previous_quaternion	= stateL.quaternion;
			state.linear_vel			= stateL.linear_vel;
			//---------------------------------------
			m_States.push_back(state);
		};
	}
	else
	{
		net_update_A			N_A;

		P.r_u8					( *((u8*)&(N_A.State.enabled)) );

		P.r_vec3				( N_A.State.angular_vel);
		P.r_vec3				( N_A.State.linear_vel);

		P.r_vec3				( N_A.State.force);
		P.r_vec3				( N_A.State.torque);

		P.r_vec3				( N_A.State.position);

		P.r_float				( N_A.State.quaternion.x );
		P.r_float				( N_A.State.quaternion.y );
		P.r_float				( N_A.State.quaternion.z );
		P.r_float				( N_A.State.quaternion.w );

		if (!NET.empty())
			N_A.dwTimeStamp			= NET.back().dwTimeStamp;
		else
			N_A.dwTimeStamp			= Level().timeServer();

		N_A.State.previous_position	= N_A.State.position;
		N_A.State.previous_quaternion = N_A.State.quaternion;
		//----------- for E3 -----------------------------
		if (Local() && OnClient() || !g_Alive()) return;
//		if (g_Alive() && (Remote() || OnServer()))
		{
			//-----------------------------------------------
			if (!NET_A.empty() && N_A.dwTimeStamp < NET_A.back().dwTimeStamp) return;
			if (!NET_A.empty() && N_A.dwTimeStamp == NET_A.back().dwTimeStamp)
			{
				NET_A.back() = N_A;
			}
			else
			{
				NET_A.push_back			(N_A);
				if (NET_A.size()>5) NET_A.pop_front();
			};

			if (!NET_A.empty()) m_bInterpolate = true;
		};
	}
	//-----------------------------------------------
	net_Import_Physic_proceed();
	//-----------------------------------------------
};

void	CActor::net_Import_Physic_proceed	( )
{
	Level().AddObject_To_Objects4CrPr(this);
	CrPr_SetActivated(false);
	CrPr_SetActivationStep(0);
};

void CActor::net_ImportInput	(NET_Packet &P)
{
	net_input			NI;

	P.r_float			(NI.cam_yaw);
	P.r_float			(NI.cam_pitch);
	//-----------------------------------
	NetInput_Apply(&NI);
};

void CActor::NetInput_Save()
{
	net_input	NI;

	//----------- for E3 -----------------------------
//	if (Local() && OnClient()) return;
	//-------------------------------------------------

//	if (getSVU() || !g_Alive()) return; //don't need to store/send input on server
	if (OnServer()) return;

	//Store Input
	NI.m_dwTimeStamp		= Level().timeServer();
	NI.mstate_wishful		= mstate_wishful;

	NI.cam_mode				= u8(cam_active);
	NI.cam_yaw				= cam_Active()->yaw;
	NI.cam_pitch			= cam_Active()->pitch;

	if (!NET_InputStack.empty() && NET_InputStack.back().m_dwTimeStamp == NI.m_dwTimeStamp)
		NET_InputStack.pop_back();

	if (NET_InputStack.size()>5)
		NET_InputStack.pop_back();

	NET_InputStack.push_back(NI);
}


static	u32	dwLastUpdateTime = 0;
void	CActor::NetInput_Send()
{
	//----------- for E3 -----------------------------
//	if (Local() && OnClient()) return;
	//-------------------------------------------------

	if (OnServer()) return; //don't need to store/send input on server

	//Send Input
	NET_Packet		NP;
	net_input		NI = NET_InputStack.back();

	NP.w_begin		(M_CL_INPUT);

	NP.w_u16		(u16(ID()));
//	NP.w_u32		(NI.m_dwTimeStamp);
//	NP.w_u32		(NI.mstate_wishful);

//	NP.w_u8			(NI.cam_mode	);
	NP.w_float		(NI.cam_yaw		);
	NP.w_float		(NI.cam_pitch	);

//	if (Level().net_HasBandwidth()) 
	u32 DeviceTime = Device.dwTimeGlobal;
	if (dwLastUpdateTime+g_dwInputUpdateDelta < DeviceTime)
	{ 
		Level().Send	(NP,net_flags(net_cl_inputguaranteed,TRUE));
		dwLastUpdateTime = DeviceTime;
	};
};

void CActor::NetInput_Apply			(net_input* pNI)
{
	cam_Active()->yaw = pNI->cam_yaw;
	cam_Active()->pitch = pNI->cam_pitch;

	unaffected_r_torso_yaw = -pNI->cam_yaw;
	unaffected_r_torso_pitch = pNI->cam_pitch;
	//-----------------------------------
	if (OnClient())
	{
		float Jump = 0;
		float dt = 0;
		g_cl_CheckControls		(mstate_wishful,NET_SavedAccel,Jump,dt);
		g_cl_Orientate			(mstate_real,dt);
		g_Orientate				(mstate_real,dt);

		g_Physics				(NET_SavedAccel,Jump,dt);
		g_cl_ValidateMState		(dt,mstate_wishful);
		g_SetAnimation			(mstate_real);
	}
};

static u32 g_dwTime0;
static u32 g_dwTime1;

void CActor::NetInput_Update	(u32 Time)
{
	g_dwTime0 = Time;
	g_dwTime1 = g_dwTime0 + u32(fixed_step*1000);

	xr_deque<net_input>::iterator I = NET_InputStack.begin();
	while (I != NET_InputStack.end())
	{
		if (I->m_dwTimeStamp >= g_dwTime0 && I->m_dwTimeStamp < g_dwTime1)
			NetInput_Apply(&(*I));
		I++;
	};
};

BOOL CActor::net_Spawn		(LPVOID DC)
{
	if (m_pPhysicsShell)
	{
		m_pPhysicsShell->Deactivate();
		xr_delete(m_pPhysicsShell);
	};
	//force actor to be local on server client
	CSE_Abstract			*e	= (CSE_Abstract*)(DC);
	CSE_ALifeCreatureActor	*E	= smart_cast<CSE_ALifeCreatureActor*>(e);	
	if (OnServer())
	{
		E->s_flags.set(M_SPAWN_OBJECT_LOCAL, TRUE);
	};

	//-------------------------------------
	// инициализация реестров, используемых актером
	contacts_registry.init(ID());
	encyclopedia_registry.init(ID());
	game_task_registry.init(ID());

	if (!CInventoryOwner::net_Spawn(DC)) return FALSE;
	if (!inherited::net_Spawn(DC))	return FALSE;

	m_PhysicMovementControl->SetPosition	(Position());
	m_PhysicMovementControl->SetVelocity	(0,0,0);
	m_PhysicMovementControl->ActivateBox	(0);

	if (GameID() != GAME_SINGLE)
	{
		E->o_model = E->o_Angle.y;
		E->o_torso.yaw = E->o_Angle.y;
		E->o_torso.pitch = -E->o_Angle.x;
	}
/*	else
	{
		E->o_Angle.y = E->o_model;
		E->o_Angle.y = E->o_torso.yaw;
		E->o_Angle.x = -E->o_torso.pitch;
	}*/

	r_model_yaw				= E->o_model;
	r_torso.yaw				= E->o_torso.yaw;
	r_torso.pitch			= E->o_torso.pitch;

	unaffected_r_torso_yaw	 = r_torso.yaw;
	unaffected_r_torso_pitch = r_torso.pitch;



	cam_Set	(eacFirstEye);
	cam_Active()->Set		(-E->o_torso.yaw,E->o_torso.pitch,0);		// set's camera orientation

	// *** movement state - respawn
	mstate_wishful			= 0;
	mstate_real				= 0;
	m_bJumpKeyPressed		= FALSE;

	NET_SavedAccel.set		(0,0,0);
	NET_WasInterpolating	= TRUE;

	setEnabled				(E->s_flags.is(M_SPAWN_OBJECT_LOCAL));

	Engine.Sheduler.Unregister	(this);
	Engine.Sheduler.Register	(this,TRUE);

	hit_slowmo				= 0.f;
	m_pArtefact				= 0;

	CSE_ALifeTraderAbstract	 *pTA	= smart_cast<CSE_ALifeTraderAbstract*>(e);
	m_dwMoney				= pTA->m_dwMoney;
	m_tRank					= pTA->m_tRank;

	OnChangeVisual();
	//----------------------------------
	m_bAllowDeathRemove = false;

//	m_bHasUpdate = false;
	m_bInInterpolation = false;
	m_bInterpolate = false;

//	if (GameID() != GAME_SINGLE)
	{
		processing_activate();
	}

#ifdef DEBUG
	LastPosS.clear();
	LastPosH.clear();
	LastPosL.clear();
#endif
//*
	
//	if (OnServer())// && E->s_flags.is(M_SPAWN_OBJECT_LOCAL))
	/*
	if (OnClient())
	{
		if (!pStatGraph)
		{
		static g_Y = 100;
		pStatGraph = xr_new<CStatGraph>();
		pStatGraph->SetRect(0, g_Y, 1024, 100, 0xff000000, 0xff000000);
		g_Y += 110;
		if (g_Y > 700) g_Y = 100;
		pStatGraph->SetGrid(0, 0.0f, 10, 1.0f, 0xff808080, 0xffffffff);
		pStatGraph->SetMinMax(-PI, PI, 300);

		}
	}
	*/
	if (m_DefaultVisualOutfit == NULL)
		m_DefaultVisualOutfit = cNameVisual();

	VERIFY(m_pActorEffector == NULL);
	m_pActorEffector = xr_new<CActorEffector>();
	smart_cast<CKinematics*>(Visual())->CalculateBones();


	//--------------------------------------------------------------
	//добавить отметки на карте, которые актер помнит в info_portions
	if(known_info_registry.objects_ptr())
	{
		Level().RemoveMapLocations();
		const KNOWN_INFO_VECTOR& know_info = *known_info_registry.objects_ptr();
		for(KNOWN_INFO_VECTOR::const_iterator it = know_info.begin();
					know_info.end() != it; it++)
		{
			CInfoPortion info_portion;
			info_portion.Load((*it).id);
			AddMapLocationsFromInfo(&info_portion);
		}
	}


	//-------------------------------------
	m_States.empty();

	//-------------------------------------
	if (!g_Alive())
	{
		mstate_wishful	&=		~mcAnyMove;
		mstate_real		&=		~mcAnyMove;
		CSkeletonAnimated* K= smart_cast<CSkeletonAnimated*>(Visual());
		K->PlayCycle("death_init");
		create_Skeleton();
		
		//остановить звук тяжелого дыхания
		m_HeavyBreathSnd.stop();
		m_bHeavyBreathSndPlaying = false;
	}
	return					TRUE;
}

void CActor::net_Destroy	()
{
	inherited::net_Destroy	();

#pragma todo("Dima to MadMax : do not comment inventory owner net_Destroy!!!")
	CInventoryOwner::net_Destroy();

	u32 it;
	for (it=0; it<SND_HIT_COUNT; ++it)	::Sound->destroy	(sndHit[it]);
	for (it=0; it<SND_DIE_COUNT; ++it)	::Sound->destroy	(sndDie[it]);
	m_PhysicMovementControl->DestroyCharacter();
	if(m_pPhysicsShell)			{
		m_pPhysicsShell->Deactivate();
		xr_delete<CPhysicsShell>(m_pPhysicsShell);
	};

	xr_delete	(pStatGraph);
	xr_delete	(m_pActorEffector);
	pCamBobbing = NULL;

#ifdef DEBUG	
	LastPosS.clear();
	LastPosH.clear();
	LastPosL.clear();
#endif

	processing_deactivate();
}

void CActor::net_Relcase	(CObject* O)
{
	inherited::net_Relcase	(O);
}

BOOL	CActor::net_Relevant		()				// relevant for export to server
{ 
	if (OnServer())
	{
		return getSVU() | getLocal(); 
	}
	else
	{
		return Local() & g_Alive();
	};
};
void	CActor::OnChangeVisual()
{
	// take index spine bone
	CSkeletonAnimated* V= smart_cast<CSkeletonAnimated*>(Visual());
	VERIFY				(V);
	int spine_bone		= V->LL_BoneID("bip01_spine1");
	int shoulder_bone	= V->LL_BoneID("bip01_spine2");
	int head_bone		= V->LL_BoneID("bip01_head");
	V->LL_GetBoneInstance(u16(spine_bone)).set_callback		(SpinCallback,this);
	V->LL_GetBoneInstance(u16(shoulder_bone)).set_callback	(ShoulderCallback,this);
	V->LL_GetBoneInstance(u16(head_bone)).set_callback		(HeadCallback,this);

	m_anims.Create			(V);
	//-------------------------------------------------------------------------------
	m_r_hand				= smart_cast<CKinematics*>(Visual())->LL_BoneID("bip01_r_hand");
	m_l_finger1				= smart_cast<CKinematics*>(Visual())->LL_BoneID("bip01_l_finger1");
	m_r_finger2				= smart_cast<CKinematics*>(Visual())->LL_BoneID("bip01_r_finger2");
	//-------------------------------------------------------------------------------
	reattach_items();
	//-------------------------------------------------------------------------------
};

void	CActor::ChangeVisual			( ref_str NewVisual )
{
	if (NewVisual == NULL || !(*NewVisual)) return;
	if (cNameVisual() != NULL && (*cNameVisual()))
	{
		if (cNameVisual() == NewVisual) return;
	}

	cNameVisual_set(NewVisual);

	OnChangeVisual();
};

void ACTOR_DEFS::net_update::lerp(ACTOR_DEFS::net_update& A, ACTOR_DEFS::net_update& B, float f)
{
	float invf		= 1.f-f;
	// 
	o_model			= angle_lerp	(A.o_model,B.o_model,		f);
	o_torso.yaw		= angle_lerp	(A.o_torso.yaw,B.o_torso.yaw,f);
	o_torso.pitch	= angle_lerp	(A.o_torso.pitch,B.o_torso.pitch,f);
	p_pos.lerp		(A.p_pos,B.p_pos,f);
	p_accel			= (f<0.5f)?A.p_accel:B.p_accel;
	p_velocity.lerp	(A.p_velocity,B.p_velocity,f);
	mstate			= (f<0.5f)?A.mstate:B.mstate;
	weapon			= (f<0.5f)?A.weapon:B.weapon;
	fHealth			= invf*A.fHealth+f*B.fHealth;
	fArmor			= invf*A.fArmor+f*B.fArmor;
	weapon			= (f<0.5f)?A.weapon:B.weapon;
}

void CActor::PH_B_CrPr		()	// actions & operations before physic correction-prediction steps
{
	//just set last update data for now
//	if (!m_bHasUpdate) return;
	if (CrPr_IsActivated()) return;
	if (CrPr_GetActivationStep() > ph_world->m_steps_num) return;

	if (g_Alive())
	{
		CrPr_SetActivated(true);
		///////////////////////////////////////////////
		IStart.Pos				= Position();
		IStart.o_model			= r_model_yaw;
		IStart.o_torso.yaw		= unaffected_r_torso_yaw;
		IStart.o_torso.pitch	= unaffected_r_torso_pitch;
		///////////////////////////////////////////////
		CPHSynchronize* pSyncObj = NULL;
		pSyncObj = PHGetSyncItem(0);
		if (!pSyncObj) return;
		pSyncObj->get_State(LastState);
		///////////////////////////////////////////////

		//----------- for E3 -----------------------------
		if (Local() && OnClient())
			//------------------------------------------------
		{
			PHUnFreeze();

			pSyncObj->set_State(NET_A.back().State);

			NetInput_Update(NET_A.back().dwTimeStamp);
		}
		else
		{
			net_update_A N_A = NET_A.back();
			net_update N = NET.back();

			NET_Last.o_model = N.o_model;
			NET_Last.o_torso = N.o_torso;
			NET_Last.mstate = N.mstate;
			NET_Last.p_accel = N.p_accel;
			///////////////////////////////////////////////
			if (!N_A.State.enabled) 
			{
				pSyncObj->set_State(N_A.State);
			}
			else
			{
				PHUnFreeze();

				pSyncObj->set_State(N_A.State);
			};
		};
	}
	else
	{
		if (PHGetSyncItemsNumber() != m_u16NumBones || m_States.empty()) return;
		CrPr_SetActivated(true);

		PHUnFreeze();

		for (u16 i=0; i<m_u16NumBones; i++)
		{
			SPHNetState state, stateL;
			PHGetSyncItem(i)->get_State(state);
			stateL = m_States[i];
			//---------------------------------------
			state.position				= stateL.position;
			state.previous_position		= stateL.previous_position;
			state.quaternion			= stateL.quaternion;
			state.previous_quaternion	= stateL.previous_quaternion;
			state.linear_vel			= stateL.linear_vel;
			state.enabled				= true;
			//---------------------------------------
			PHGetSyncItem(i)->set_State(state);
		};
	};
};	


void CActor::PH_I_CrPr		()		// actions & operations between two phisic prediction steps
{
	//store recalculated data, then we able to restore it after small future prediction
//	if (!m_bHasUpdate) return;
	if (!CrPr_IsActivated()) return;
	if (g_Alive())
	{
		if (OnClient())
			cam_Active()->Set		(-unaffected_r_torso_yaw,unaffected_r_torso_pitch,0);		// set's camera orientation
		////////////////////////////////////
		CPHSynchronize* pSyncObj = NULL;
		pSyncObj = PHGetSyncItem(0);
		if (!pSyncObj) return;
		////////////////////////////////////
		pSyncObj->get_State(RecalculatedState);
		////////////////////////////////////
		//----------- for E3 -----------------------------
		if (Local() && OnClient())
		//------------------------------------------------
		{
			NetInput_Update(g_dwTime1);
		};
	}; 
};

void CActor::PH_A_CrPr		()
{
	//restore recalculated data and get data for interpolation	
//	if (!m_bHasUpdate) return;
//	m_bHasUpdate = false;
	if (!CrPr_IsActivated()) return;
	if (!g_Alive()) return;
	////////////////////////////////////
	CPHSynchronize* pSyncObj = NULL;
	pSyncObj = PHGetSyncItem(0);
	if (!pSyncObj) return;
	////////////////////////////////////
	pSyncObj->get_State(PredictedState);
	//----------- for E3 -----------------------------
	if (Local() && OnClient())
	//------------------------------------------------
	{
		xr_deque<net_input>::iterator	B = NET_InputStack.begin();
		xr_deque<net_input>::iterator	E = NET_InputStack.end();
		xr_deque<net_input>::iterator I = std::lower_bound(B,E,g_dwTime1-1);
		while (I!= E)
		{
			NetInput_Apply(&(*I));
			I++;
		};		
	};
	////////////////////////////////////
	pSyncObj->set_State(RecalculatedState);
	////////////////////////////////////
	if (!m_bInterpolate) return;

	IEnd.Pos				= PredictedState.position;
	IEnd.o_model			= NET_Last.o_model;
	IEnd.o_torso			= NET_Last.o_torso;
	////////////////////////////////////
	CalculateInterpolationParams();
};

int		actInterpType = 0;
void CActor::make_Interpolation	()
{
	m_dwILastUpdateTime = Level().timeServer();
		
	if(g_Alive() && m_bInInterpolation) 
	{
		u32 CurTime = m_dwILastUpdateTime;

		if (CurTime >= m_dwIEndTime)
		{
			m_bInInterpolation = false;

			CPHSynchronize* pSyncObj = NULL;
			pSyncObj = PHGetSyncItem(0);
			if (!pSyncObj) return;
			pSyncObj->set_State(PredictedState);//, PredictedState.enabled);
			Position().set(PredictedState.position);
		}
		else
		{
			float factor = float(CurTime - m_dwIStartTime)/(m_dwIEndTime - m_dwIStartTime);

			Position().lerp(IStart.Pos, IEnd.Pos, factor);
			
			for (u32 k=0; k<3; k++)
			{
				IPosL[k] = Position()[k];
				IPosS[k] = factor*(factor*(factor*SCoeff[k][0]+SCoeff[k][1])+SCoeff[k][2])+SCoeff[k][3];
				IPosH[k] = factor*(factor*(factor*HCoeff[k][0]+HCoeff[k][1])+HCoeff[k][2])+HCoeff[k][3];
			};
			
			switch (g_cl_InterpolationType)
			{
			case 0:	Position().set(IPosL); break;
			case 1: Position().set(IPosS); break;
			case 2: Position().set(IPosH); break;
			default:
				{
					R_ASSERT2(0, "Unknown interpolation curve type!");
				}
			}
			
			g_Orientate				(NET_Last.mstate,0			);

		};
	}
	else
	{
		m_bInInterpolation = false;
	};

#ifdef DEBUG
	if (getVisible() && g_Alive() && mstate_real) 
	{
		LastPosS.push_back(IPosS);	while (LastPosS.size()>g_cl_InterpolationMaxPoints) LastPosS.pop_front();
		LastPosH.push_back(IPosH);	while (LastPosH.size()>g_cl_InterpolationMaxPoints) LastPosH.pop_front();
		LastPosL.push_back(IPosL);	while (LastPosL.size()>g_cl_InterpolationMaxPoints) LastPosL.pop_front();
	};
#endif
};
/*
void		CActor::UpdatePosStack	( u32 Time0, u32 Time1 )
{
		//******** Storing Last Position in stack ********
	CPHSynchronize* pSyncObj = NULL;
	pSyncObj = PHGetSyncItem(0);
	if (!pSyncObj) return;

	SPHNetState		State;
	pSyncObj->get_State(State);

	if (!SMemoryPosStack.empty() && SMemoryPosStack.back().u64WorldStep >= ph_world->m_steps_num)
	{
		xr_deque<SMemoryPos>::iterator B = SMemoryPosStack.begin();
		xr_deque<SMemoryPos>::iterator E = SMemoryPosStack.end();
		xr_deque<SMemoryPos>::iterator I = std::lower_bound(B,E,u64(ph_world->m_steps_num-1));
		if (I != E) 
		{
			I->SState = State;
			I->u64WorldStep = ph_world->m_steps_num;
		};
	}
	else		
	{
		SMemoryPosStack.push_back(SMemoryPos(Time0, Time1, ph_world->m_steps_num, State));
		if (SMemoryPosStack.front().dwTime0 < (Level().timeServer() - 2000)) SMemoryPosStack.pop_front();
	};
};

ACTOR_DEFS::SMemoryPos*				CActor::FindMemoryPos (u32 Time)
{
	if (SMemoryPosStack.empty()) return NULL;

	if (Time > SMemoryPosStack.back().dwTime1) return NULL;
	
	xr_deque<SMemoryPos>::iterator B = SMemoryPosStack.begin();
	xr_deque<SMemoryPos>::iterator E = SMemoryPosStack.end();
	xr_deque<SMemoryPos>::iterator I = std::lower_bound(B,E,Time);

	if (I==E) return NULL;

	return &(*I);
};
*/

extern	float		g_cl_lvInterp;

void	CActor::CalculateInterpolationParams()
{
//	Fmatrix xformX0, xformX1;
	CPHSynchronize* pSyncObj = NULL;
	pSyncObj = PHGetSyncItem(0);
	///////////////////////////////////////////////
	IStart.Pos				= Position();
	IStart.o_model			= r_model_yaw;
	IStart.o_torso.yaw		= unaffected_r_torso_yaw;
	IStart.o_torso.pitch	= unaffected_r_torso_pitch;
	/////////////////////////////////////////////////////////////////////
	IRec.Pos				= RecalculatedState.position;
	IRec.o_model			= NET_Last.o_model;
	IRec.o_torso			= NET_Last.o_torso;
	/////////////////////////////////////////////////////////////////////
	IEnd.Pos				= PredictedState.position;
	IEnd.o_model			= IRec.o_model			;
	IEnd.o_torso.yaw		= IRec.o_torso.yaw		;
	IEnd.o_torso.pitch		= IRec.o_torso.pitch	;	
	/////////////////////////////////////////////////////////////////////
	Fvector SP0, SP1, SP2, SP3;
	Fvector HP0, HP1, HP2, HP3;

	SP0 = IStart.Pos;
	HP0 = IStart.Pos;

	if (m_bInInterpolation)
	{
		u32 CurTime = Level().timeServer();
		float factor	= float(CurTime - m_dwIStartTime)/(m_dwIEndTime - m_dwIStartTime);
		if (factor > 1.0f) factor = 1.0f;

		float c = factor;
		for (u32 k=0; k<3; k++)
		{
			SP0[k] = c*(c*(c*SCoeff[k][0]+SCoeff[k][1])+SCoeff[k][2])+SCoeff[k][3];
			SP1[k] = (c*c*SCoeff[k][0]*3+c*SCoeff[k][1]*2+SCoeff[k][2])/3; // сокрость из формулы в 3 раза превышает скорость при расчете коэффициентов !!!!

			HP0[k] = c*(c*(c*HCoeff[k][0]+HCoeff[k][1])+HCoeff[k][2])+HCoeff[k][3];
			HP1[k] = (c*c*HCoeff[k][0]*3+c*HCoeff[k][1]*2+HCoeff[k][2]); // сокрость из формулы в 3 раза превышает скорость при расчете коэффициентов !!!!
		};

		SP1.add(SP0);
	}	
	else
	{
		if (LastState.linear_vel.x == 0 && LastState.linear_vel.y == 0 && LastState.linear_vel.z == 0)
		{
			HP1.sub(RecalculatedState.position, RecalculatedState.previous_position);
		}
		else
		{
			HP1.sub(LastState.position, LastState.previous_position);
		};
		HP1.mul(1.0f/fixed_step);
		SP1.add(HP1, SP0);
	}
	HP2.sub(PredictedState.position, PredictedState.previous_position);
	HP2.mul(1.0f/fixed_step);
	SP2.sub(PredictedState.position, HP2);

	SP3.set(PredictedState.position);
	HP3.set(PredictedState.position);
/*
	{
		Fvector d0, d1;
		d0.sub(SP1, SP0);
		d1.sub(SP3, SP0);
		float res = d0.dotproduct(d1);
		if (res < 0)
		{
			Msg ("! %f", res);
		}
		else
			Msg ("%f", res);
	}
*/
	/////////////////////////////////////////////////////////////////////////////
	Fvector TotalPath;
	TotalPath.sub(SP3, SP0);
	float TotalLen = TotalPath.magnitude();

	SPHNetState	State0 = (NET_A.back()).State;
	SPHNetState	State1 = PredictedState;

	float lV0 = State0.linear_vel.magnitude();
	float lV1 = State1.linear_vel.magnitude();

	u32		ConstTime = u32((fixed_step - ph_world->m_frame_time)*1000)+ Level().GetInterpolationSteps()*u32(fixed_step*1000);

	m_dwIStartTime = m_dwILastUpdateTime;

	if (( lV0 + lV1) > 0.000001 && g_cl_lvInterp == 0)
	{
		u32		CulcTime = iCeil(TotalLen*2000/( lV0 + lV1));
		m_dwIEndTime = m_dwIStartTime + min(CulcTime, ConstTime);
	}
	else
		m_dwIEndTime = m_dwIStartTime + ConstTime;
	/////////////////////////////////////////////////////////////////////////////
	Fvector V0, V1;
//	V0.sub(SP1, SP0);
//	V1.sub(SP3, SP2);
	V0.set(HP1);
	V1.set(HP2);
	lV0 = V0.magnitude();
	lV1 = V1.magnitude();

	if (TotalLen != 0)
	{
		if (V0.x != 0 || V0.y != 0 || V0.z != 0)
		{
			if (lV0 > TotalLen/3)
			{
				HP1.normalize();
//				V0.normalize();
//				V0.mul(TotalLen/3);
				HP1.normalize();
				HP1.mul(TotalLen/3);
				SP1.add(HP1, SP0);
			}
		}

		if (V1.x != 0 || V1.y != 0 || V1.z != 0)
		{
			if (lV1 > TotalLen/3)
			{
//				V1.normalize();
//				V1.mul(TotalLen/3);
				HP2.normalize();
				HP2.mul(TotalLen/3);
				SP2.sub(SP3, HP2);
			};
		}
	};
	/////////////////////////////////////////////////////////////////////////////
	for( u32 i =0; i<3; i++)
	{
		SCoeff[i][0] = SP3[i]	- 3*SP2[i] + 3*SP1[i] - SP0[i];
		SCoeff[i][1] = 3*SP2[i]	- 6*SP1[i] + 3*SP0[i];
		SCoeff[i][2] = 3*SP1[i]	- 3*SP0[i];
		SCoeff[i][3] = SP0[i];

		HCoeff[i][0] = 2*HP0[i] - 2*HP3[i] + HP1[i] + HP2[i];
		HCoeff[i][1] = -3*HP0[i]	+ 3*HP3[i] - 2*HP1[i] - HP2[i];
		HCoeff[i][2] = HP1[i];
		HCoeff[i][3] = HP0[i];
	};
	/////////////////////////////////////////////////////////////////////////////
	m_bInInterpolation = true;

	if (m_pPhysicsShell) m_pPhysicsShell->NetInterpolationModeON();
}

void CActor::save(NET_Packet &output_packet)
{
	inherited::save(output_packet);
	CInventoryOwner::save(output_packet);
}
void CActor::load(IReader &input_packet)
{
	inherited::load(input_packet);
	CInventoryOwner::load(input_packet);
}


#ifdef DEBUG

extern	Flags32	dbg_net_Draw_Flags;

void	CActor::OnRender_Network()
{
	if (g_Alive())
	{
		if (!(dbg_net_Draw_Flags.is_any((1<<2)))) return;
		float size = 0.2f;

		Fvector PI;
		PI.set(Position()); PI.y += 1;
		RCache.dbg_DrawAABB			(PI, size, size, size, color_rgba(128, 255, 128, 255));

		RCache.dbg_DrawAABB			(IStart.Pos, size, size, size, color_rgba(255, 0, 0, 255));
		RCache.dbg_DrawAABB			(IRec.Pos, size, size, size, color_rgba(0, 255, 0, 255));
		RCache.dbg_DrawAABB			(IEnd.Pos, size, size, size, color_rgba(0, 0, 255, 255));

		Fmatrix MS, MH, ML, *pM = NULL;
		ML.translate(0, 0.2f, 0);
		MS.translate(0, 0.2f, 0);
		MH.translate(0, 0.2f, 0);

		Fvector point0S, point1S, point0H, point1H, point0L, point1L, *ppoint0 = NULL, *ppoint1 = NULL;
		Fvector tS, tH;
		u32	cColor = 0, sColor = 0;
		VIS_POSITION*	pLastPos = NULL;

		switch (g_cl_InterpolationType)
		{
		case 0: ppoint0 = &point0L; ppoint1 = &point1L; cColor = color_rgba(0, 255, 0, 255); sColor = color_rgba(128, 255, 128, 255); pM = &ML; pLastPos = &LastPosL; break;
		case 1: ppoint0 = &point0S; ppoint1 = &point1S; cColor = color_rgba(0, 0, 255, 255); sColor = color_rgba(128, 128, 255, 255); pM = &MS; pLastPos = &LastPosS; break;
		case 2: ppoint0 = &point0H; ppoint1 = &point1H; cColor = color_rgba(255, 0, 0, 255); sColor = color_rgba(255, 128, 128, 255); pM = &MH; pLastPos = &LastPosH; break;
		}

		float c = 0;
		for (float i=0; i<1.1f; i+= 0.1f)
		{
			c = i;// * 0.1f;
			for (u32 k=0; k<3; k++)
			{
				point1S[k] = c*(c*(c*SCoeff[k][0]+SCoeff[k][1])+SCoeff[k][2])+SCoeff[k][3];
				point1H[k] = c*(c*(c*HCoeff[k][0]+HCoeff[k][1])+HCoeff[k][2])+HCoeff[k][3];
				point1L[k] = IStart.Pos[k] + c*(IEnd.Pos[k]-IStart.Pos[k]);
			};
			if (i!=0)
			{
				RCache.dbg_DrawLINE(*pM, *ppoint0, *ppoint1, cColor);
			};
			point0S.set(point1S);
			point0H.set(point1H);
			point0L.set(point1L);
		};

		for (i=0; i<2; i++)
		{
			c = i;
			for (u32 k=0; k<3; k++)
			{
				point1S[k] = c*(c*(c*SCoeff[k][0]+SCoeff[k][1])+SCoeff[k][2])+SCoeff[k][3];
				point1H[k] = c*(c*(c*HCoeff[k][0]+HCoeff[k][1])+HCoeff[k][2])+HCoeff[k][3];

				tS[k] = (c*c*SCoeff[k][0]*3+c*SCoeff[k][1]*2+SCoeff[k][2])/3; // сокрость из формулы в 3 раза превышает скорость при расчете коэффициентов !!!!
				tH[k] = (c*c*HCoeff[k][0]*3+c*HCoeff[k][1]*2+HCoeff[k][2]); 
			};

			point0S.add(tS, point1S);
			point0H.add(tH, point1H);

			if (g_cl_InterpolationType > 0)
			{
				RCache.dbg_DrawLINE(*pM, *ppoint0, *ppoint1, cColor);
			}
		}

		if (!pLastPos->empty())
		{
			Fvector Pos1, Pos2;
			VIS_POSITION_it It = pLastPos->begin();
			Pos1 = *It;
			for (; It != pLastPos->end(); It++)
			{
				Pos2 = *It;

				RCache.dbg_DrawLINE(*pM, Pos1, Pos2, sColor);
				Pos1 = *It;
			};
		};

		Fvector PH, PS;
		PH.set(IPosH); PH.y += 1;
		PS.set(IPosS); PS.y += 1;
		RCache.dbg_DrawAABB			(PS, size, size, size, color_rgba(128, 128, 255, 255));
		RCache.dbg_DrawAABB			(PH, size, size, size, color_rgba(255, 128, 128, 255));
		/////////////////////////////////////////////////////////////////////////////////
	}
	else
	{
		if (!(dbg_net_Draw_Flags.is_any((1<<1)))) return;
		if (!m_States.empty())
		{
			u32 NumBones = m_States.size();
			for (u32 i=0; i<NumBones; i++)
			{
				SPHNetState state = m_States[i];			

				Fvector half_dim;
				half_dim.x = 0.2f;
				half_dim.y = 0.1f;
				half_dim.z = 0.1f;

				u32 Color = color_rgba(255, 0, 0, 255);

				Fmatrix M;
				
				M = Fidentity;
				M.rotation(state.quaternion);
				M.translate_add(state.position);
				RCache.dbg_DrawOBB				(M, half_dim, Color);

				if (!PHGetSyncItem(u16(i))) continue;
				PHGetSyncItem(u16(i))->get_State(state);

				Color = color_rgba(0, 255, 0, 255);
				M = Fidentity;
				M.rotation(state.quaternion);
				M.translate_add(state.position);
				RCache.dbg_DrawOBB				(M, half_dim, Color);
			};
		}
		else
		{
			if (!g_Alive() && PHGetSyncItemsNumber() > 2)
			{
				u16 NumBones = PHGetSyncItemsNumber();
				for (u16 i=0; i<NumBones; i++)
				{
					SPHNetState state;// = m_States[i];
					PHGetSyncItem(i)->get_State(state);

					Fmatrix M;
					M = Fidentity;
					M.rotation(state.quaternion);
					M.translate_add(state.position);

					Fvector half_dim;
					half_dim.x = 0.2f;
					half_dim.y = 0.1f;
					half_dim.z = 0.1f;

					u32 Color = color_rgba(0, 255, 0, 255);
					RCache.dbg_DrawOBB				(M, half_dim, Color);
				};
				//-----------------------------------------------------------------
				Fvector min,max;

				min.set(F_MAX,F_MAX,F_MAX);
				max.set(-F_MAX,-F_MAX,-F_MAX);
				/////////////////////////////////////
				for(u16 i=0;i<NumBones;i++)
				{
					SPHNetState state;
					PHGetSyncItem(i)->get_State(state);

					Fvector& p=state.position;
					UpdateLimits (p, min, max);

					Fvector px =state.linear_vel;
					px.div(10.0f);
					px.add(state.position);
					UpdateLimits (px, min, max);
				};

				NET_Packet PX;
				for(u16 i=0;i<NumBones;i++)
				{
					SPHNetState state;
					PHGetSyncItem(i)->get_State(state);

					PX.B.count = 0;
					w_vec_q8(PX,state.position,min,max);
					w_qt_q8(PX,state.quaternion);
//					w_vec_q8(PX,state.linear_vel,min,max);

					PX.r_pos = 0;
					r_vec_q8(PX,state.position,min,max);
					r_qt_q8(PX,state.quaternion);
//					r_vec_q8(PX,state.linear_vel,min,max);
					//===============================================
					Fmatrix M;
					M = Fidentity;
					M.rotation(state.quaternion);
					M.translate_add(state.position);

					Fvector half_dim;
					half_dim.x = 0.2f;
					half_dim.y = 0.1f;
					half_dim.z = 0.1f;

					u32 Color = color_rgba(255, 0, 0, 255);
					RCache.dbg_DrawOBB				(M, half_dim, Color);
				};	
				Fvector LC, LS;
				LC.add(min, max); LC.div(2.0f);
				LS.sub(max, min); LS.div(2.0f);

				RCache.dbg_DrawAABB			(LC, LS.x, LS.y, LS.z, color_rgba(255, 128, 128, 255));
				//-----------------------------------------------------------------
			};
		}
	}
};

#endif
