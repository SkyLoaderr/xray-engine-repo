// Actor_Network.cpp:	 для работы с сетью
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "actor.h"
#include "Actor_Flags.h"
#include "inventory.h"
#include "xrserver_objects_alife_monsters.h"

#include "CameraLook.h"
#include "CameraFirstEye.h"

#include "ActorEffector.h"


#include "PHWorld.h"

static u32	g_dwStartTime		= 0;
static u32	g_dwLastUpdateTime	;
static u32	g_dwNumUpdates		= 0;
float		g_fNumUpdates		= 0;

//--------------------------------------------------------------------
void CActor::net_Export	(NET_Packet& P)					// export to server
{
	// export 
	//	R_ASSERT			(Local());
	//	Msg			("[%6d] %s",P.r_pos,cName());

	u8					flags = 0;
	P.w_float_q16		(g_Health(),-1000,1000);

	P.w_u32				(Level().timeServer());
	P.w_u8				(flags);
	Fvector				p = Position();
	P.w_vec3			(p);//Position());
	P.w_angle8			(r_model_yaw);
	P.w_angle8			(unaffected_r_torso_yaw	);//(r_torso.yaw);
	P.w_angle8			(unaffected_r_torso_pitch);//(r_torso.pitch);
	//	unaffected_r_torso_yaw	 = r_torso.yaw;
	//	unaffected_r_torso_pitch = r_torso.pitch;

	P.w_float			(inventory().TotalWeight());
	P.w_u32				(0);
	P.w_u32				(0);

	P.w_u16				(u16(mstate_real));
	P.w_sdir			(NET_SavedAccel);
	Fvector				v = m_PhysicMovementControl->GetVelocity();
	P.w_sdir			(v);//m_PhysicMovementControl.GetVelocity());
	P.w_float_q16		(fArmor,-1000,1000);

	P.w_u8				(u8(inventory().GetActiveSlot()));

	/////////////////////////////////////////////////
	u16 NumItems		= PHGetSyncItemsNumber();
	if (H_Parent() || GameID() == 1) NumItems = 0;

	P.w_u16				(NumItems);
	if (!NumItems)		return;

	SPHNetState	State;
	State.enabled = false;
	u32 Time0, Time1;

	if (SMemoryPosStack.empty())
	{
		CPHSynchronize* pSyncObj = NULL;
		pSyncObj = PHGetSyncItem(0);
		if (pSyncObj) pSyncObj->get_State(State);
		Time0 = Time1 = Level().timeServer();
	}
	else
	{
		State = SMemoryPosStack.back().SState;
		Time0 = SMemoryPosStack.back().dwTime0;
		Time1 = SMemoryPosStack.back().dwTime1;
	};

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

	P.w_u32					( Time0 );
	P.w_u32					( Time1 );
}

float g_fMaxDesyncLen		= 1.0f;

void		CActor::net_Import_Base				( NET_Packet& P)
{
	net_update			N;

	u8					flags;
	u16					tmp;

	float health;
	P.r_float_q16 (health,-1000,1000);
	//----------- for E3 -----------------------------
	if (OnClient())fEntityHealth = health;
	//------------------------------------------------

	float				fDummy;
	u32					dwDummy;

	P.r_u32				(N.dwTimeStamp	);
	P.r_u8				(flags			);
	P.r_vec3			(N.p_pos		);
	P.r_angle8			(N.o_model		);
	P.r_angle8			(N.o_torso.yaw	);
	P.r_angle8			(N.o_torso.pitch);
	//----------- for E3 -----------------------------
	if (OnClient())
	//------------------------------------------------
	{
		unaffected_r_torso_yaw	 = N.o_torso.yaw;
		unaffected_r_torso_pitch = N.o_torso.pitch;
	};

	P.r_float			(fDummy);
	P.r_u32				(dwDummy);
	P.r_u32				(dwDummy);

	P.r_u16				(tmp			); N.mstate = u32(tmp);
	P.r_sdir			(N.p_accel		);
	P.r_sdir			(N.p_velocity	);
	float				fRArmor;
	P.r_float_q16		(fRArmor,-1000,1000);
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

	if (pStatGraph) 
	{
		pStatGraph->SetMinMax(0, 100.0f, 300);
		pStatGraph->SetGrid(0, 0.0f, 10, 20.0f, 0xff808080, 0xffffffff);
		pStatGraph->SetStyle(CStatGraph::stBar);
		pStatGraph->SetStyle(CStatGraph::stCurve, 1);
		pStatGraph->SetStyle(CStatGraph::stCurve, 2);

		u32 dTime = 0;
		if (Level().timeServer() < N.dwTimeStamp) dTime = 0;
		else
			dTime = Level().timeServer() - N.dwTimeStamp;

		pStatGraph->AppendItem(float(dTime)/*g_fMaxDesyncLen*/, 0xffff00ff, 0);
	};
	//---------------------------------------------
	SMemoryPos* pMemPos = FindMemoryPos(N.dwTimeStamp);
	if (pMemPos)
	{
		u32 Time = (N.dwTimeStamp < pMemPos->dwTime0) ? pMemPos->dwTime0 : N.dwTimeStamp;
		float factor = float (Time - pMemPos->dwTime0) / float (pMemPos->dwTime1 - pMemPos->dwTime0);
		Fvector rPos;
		rPos.lerp(pMemPos->SState.previous_position, pMemPos->SState.position, factor);
		dDesyncVec.sub(rPos, N.p_pos);

		//		if (pStatGraph)pStatGraph->AppendItem(dDesyncVec.magnitude(), 0xff00ff00);
	}
	else
	{
		dDesyncVec.sub(Position(), N.p_pos);

		//		if (pStatGraph)pStatGraph->AppendItem(dDesyncVec.magnitude(), 0xffff0000);
	};
	//---------------------------------------------
	if (dDesyncVec.magnitude() >= g_fMaxDesyncLen)
	{
		CPHSynchronize* pSyncObj = NULL;
		pSyncObj = PHGetSyncItem(0);
		if (pSyncObj) 
		{
			SPHNetState		State;
			pSyncObj->get_State(State);
			State.position.sub(dDesyncVec);
			pSyncObj->set_State(State);
		};
	};
};

void		CActor::net_Import_Physic			( NET_Packet& P)
{
	net_update_A			N_A;

	if (!NET.empty())
		N_A.dwTimeStamp			= NET.back().dwTimeStamp;
	else
		N_A.dwTimeStamp			= Level().timeServer();

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

	P.r_u32					( N_A.dwTime0 );
	P.r_u32					( N_A.dwTime1 );

	N_A.State.previous_position	= N_A.State.position;
	N_A.State.previous_quaternion = N_A.State.quaternion;
	//----------- for E3 -----------------------------
	if (Local() && OnClient()) return;
	//-----------------------------------------------
	if (!NET_A.empty() && N_A.dwTime1 < NET_A.back().dwTime1) return;
	if (!NET_A.empty() && N_A.dwTime1 == NET_A.back().dwTime1)
	{
		NET_A.back() = N_A;
	}
	else
	{
		NET_A.push_back			(N_A);
		if (NET_A.size()>5) NET_A.pop_front();
	}

	if (!NET_A.empty()) m_bInterpolate = true;
	//-----------------------------------------------
	net_Import_Physic_proceed();
	//-----------------------------------------------
};



void	CActor::net_Import_Physic_proceed	( )
{
	net_update N		= NET.back();
	net_update_A N_A	= NET_A.back();

	//----------- for E3 -----------------------------
	if (Remote() || OnServer())
	//------------------------------------------------
	{
		m_bHasUpdate = true;

		Set_Level_CrPr(long(Level().timeServer()) - (NET.back().dwTimeStamp));
	}
	else
	{
		if (NET_InputStack.empty()) return;

		//----------------------------------------------
		u32 dTime = 0;
		if (Level().timeServer() > N.dwTimeStamp) dTime = Level().timeServer() - N.dwTimeStamp;

		xr_deque<net_input>::iterator	B = NET_InputStack.begin();
		xr_deque<net_input>::iterator	E = NET_InputStack.end();
		xr_deque<net_input>::iterator	I = std::lower_bound(B,E,N_A.dwTime0 - dTime);

		float NumRemained = 0;
		net_input N_I = NET_InputStack.back();
		if (I != E) 
		{
			NumRemained = float(E - I);
			NET_InputStack.erase(B, I);
			N_I = *I;
		};

		if (NumRemained)
		{
			m_bHasUpdate = true;

			Set_Level_CrPr(long(Level().timeServer()) - long(N.dwTimeStamp));

			NetInput_Save();
		};
	};

};

void CActor::net_Import		(NET_Packet& P)					// import from server
{
	if (Level().timeServer() != g_dwLastUpdateTime)
	{
		g_dwNumUpdates++;
		g_dwLastUpdateTime = Level().timeServer();
		if (g_dwStartTime == 0)
			g_dwStartTime = g_dwLastUpdateTime;
		else
		{
			u32 CurTime = g_dwLastUpdateTime;
			float CurDTime = float(CurTime - g_dwStartTime) / 1000.0f;
			g_fNumUpdates = float(g_dwNumUpdates)/CurDTime ;
		};
	};

	// import
	//	R_ASSERT			(Remote());
	//-----------------------------------------------
	net_Import_Base(P);
	//-----------------------------------------------
	u16	NumItems = 0;
	P.r_u16					( NumItems);
	if (!NumItems) return;
	//-----------------------------------------------
	net_Import_Physic(P);
	//-----------------------------------------------
}

void CActor::Set_Level_CrPr (long dTime)
{
	if (!m_bHasUpdate) return;

	if (dTime < 0) dTime = 0;
	u32 NumSteps = 0;
	if (dTime < (fixed_step*500))
		NumSteps = 0;
	else
		NumSteps = ph_world->CalcNumSteps(dTime);

	Level().SetNumCrSteps ( NumSteps );
};

void CActor::NetInput_Save()
{
	net_input	NI;

	//----------- for E3 -----------------------------
//	if (Local() && OnClient()) return;
	//-------------------------------------------------

	if (getSVU() || !g_Alive()) return; //don't need to store/send input on server

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

extern	BOOL net_cl_inputguaranteed;
extern	int g_dwInputUpdateDelta;
static	u32	dwLastUpdateTime = 0;
void	CActor::NetInput_Send()
{
	//----------- for E3 -----------------------------
//	if (Local() && OnClient()) return;
	//-------------------------------------------------

	if (getSVU() || !g_Alive()) return; //don't need to store/send input on server

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

void CActor::net_ImportInput	(NET_Packet &P)
{
	net_input			NI;

//	P.r_u32				(NI.m_dwTimeStamp);
//	P.r_u32				(NI.mstate_wishful);

//	P.r_u8				(NI.cam_mode);

	P.r_float			(NI.cam_yaw);
	P.r_float			(NI.cam_pitch);
	//-----------------------------------
	NetInput_Apply(&NI);
};

void CActor::NetInput_Apply			(net_input* pNI)
{
//	mstate_wishful = pNI->mstate_wishful;
//	cam_Set	(EActorCameras(pNI->cam_mode));
	cam_Active()->yaw = pNI->cam_yaw;
	cam_Active()->pitch = pNI->cam_pitch;

	unaffected_r_torso_yaw = -pNI->cam_yaw;
	unaffected_r_torso_pitch = pNI->cam_pitch;
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

BOOL CActor::net_Spawn		(LPVOID DC)
{
	if (m_pPhysicsShell)
	{
		m_pPhysicsShell->Deactivate();
		xr_delete(m_pPhysicsShell);
	};
	//force actor to be local on server client
	CSE_Abstract			*e	= (CSE_Abstract*)(DC);
	CSE_ALifeCreatureActor	*E	= dynamic_cast<CSE_ALifeCreatureActor*>(e);	
	if (OnServer())
	{
		E->s_flags.set(M_SPAWN_OBJECT_LOCAL, TRUE);
	};

	if (!inherited::net_Spawn(DC))	return FALSE;
	//проспавнить PDA у InventoryOwner
	if (!CInventoryOwner::net_Spawn(DC)) return FALSE;

	m_PhysicMovementControl->SetPosition	(Position());
	m_PhysicMovementControl->SetVelocity	(0,0,0);

	E->o_model = E->o_Angle.y;
	E->o_torso.yaw = E->o_Angle.y;
	E->o_torso.pitch = -E->o_Angle.x;

	r_model_yaw				= E->o_model;
	r_torso.yaw				= E->o_torso.yaw;
	r_torso.pitch			= E->o_torso.pitch;

	unaffected_r_torso_yaw	 = r_torso.yaw;
	unaffected_r_torso_pitch = r_torso.pitch;


	cam_Active()->Set		(-E->o_torso.yaw,E->o_torso.pitch,0);		// set's camera orientation
	cam_Set	(eacFirstEye);

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
	hit_factor				= 1.f;

	m_pArtifact				= 0;

	CSE_ALifeTraderAbstract	 *pTA	= dynamic_cast<CSE_ALifeTraderAbstract*>(e);
	m_dwMoney				= pTA->m_dwMoney;
	m_tRank					= pTA->m_tRank;

	// take index spine bone
	CSkeletonAnimated* V= PSkeletonAnimated(Visual());
	R_ASSERT			(V);
	int spine_bone		= V->LL_BoneID("bip01_spine1");
	int shoulder_bone	= V->LL_BoneID("bip01_spine2");
	int head_bone		= V->LL_BoneID("bip01_head");
	V->LL_GetBoneInstance(u16(spine_bone)).set_callback		(SpinCallback,this);
	V->LL_GetBoneInstance(u16(shoulder_bone)).set_callback	(ShoulderCallback,this);
	V->LL_GetBoneInstance(u16(head_bone)).set_callback		(HeadCallback,this);

	m_anims.Create			(V);

	// load damage params
	CDamageManager::Load	(*cNameSect());
	//----------------------------------
	m_bAllowDeathRemove = false;

	m_bHasUpdate = false;
	m_bInInterpolation = false;
	m_bInterpolate = false;

	/*
	if (OnClient() && E->s_flags.is(M_SPAWN_OBJECT_LOCAL))
	{
	if (!pStatGraph)
	{
	pStatGraph = xr_new<CStatGraph>();
	pStatGraph->SetRect(0, 650, 1024, 100, 0xff000000, 0xff000000);
	pStatGraph->SetGrid(0, 0.0f, 10, 1.0f, 0xff808080, 0xffffffff);
	pStatGraph->SetMinMax(-PI, PI, 300);

	pStatGraph->SetStyle(CStatGraph::stBar);
	pStatGraph->AppendSubGraph(CStatGraph::stCurve);
	pStatGraph->AppendSubGraph(CStatGraph::stCurve);
	};
	};
	//*/
	//----------------------------------

	VERIFY(m_pActorEffector == NULL);
	m_pActorEffector = xr_new<CActorEffector>();
	PKinematics(Visual())->Calculate();
	return					TRUE;
}

void CActor::net_Relcase	(CObject* O)
{
	inherited::net_Relcase	(O);
}

void CActor::net_Destroy	()
{
	inherited::net_Destroy	();

#pragma todo("Dima to MadMax : do not comment inventory owner net_Destroy!!!")
	CInventoryOwner::net_Destroy();
	::Sound->destroy			(sndZoneHeart);
	::Sound->destroy			(sndZoneDetector);

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
}


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


void CActor::PH_B_CrPr		()	// actions & operations before physic correction-prediction steps
{
	//just set last update data for now
	if (!m_bHasUpdate) return;
	///////////////////////////////////////////////
//	IStartPos = Position();
	IStart.Pos				= Position();
	IStart.o_model			= r_model_yaw;
	IStart.o_torso.yaw		= unaffected_r_torso_yaw;
	IStart.o_torso.pitch	= unaffected_r_torso_pitch;
	///////////////////////////////////////////////
	CPHSynchronize* pSyncObj = NULL;
	pSyncObj = PHGetSyncItem(0);
	if (!pSyncObj) return;
	///////////////////////////////////////////////

	//----------- for E3 -----------------------------
	if (Local() && OnClient())
	//------------------------------------------------
	{
		PHUnFreeze();

		pSyncObj->set_State(NET_A.back().State);

		NetInput_Update(NET_A.back().dwTime0);
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
		///////////////////////////////////////////////
		if (Level().InterpolationDisabled())
		{
			m_bHasUpdate = false;
		};
		///////////////////////////////////////////////
	};
	m_bInInterpolation = false;
};	


void CActor::PH_I_CrPr		()		// actions & operations between two phisic prediction steps
{
	//store recalculated data, then we able to restore it after small future prediction
	if (!m_bHasUpdate) return;
	////////////////////////////////////
	CPHSynchronize* pSyncObj = NULL;
	pSyncObj = PHGetSyncItem(0);
	if (!pSyncObj) return;
	////////////////////////////////////
	pSyncObj->get_State(RecalculatedState);

	//----------- for E3 -----------------------------
	if (Local() && OnClient())
	//------------------------------------------------
	{
		NetInput_Update(g_dwTime1);
	};
}; 

void CActor::PH_A_CrPr		()
{
	//restore recalculated data and get data for interpolation	
	if (!m_bHasUpdate) return;
	m_bHasUpdate = false;
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

//	IEndPos = PredictedState.position;
	IEnd.Pos				= PredictedState.position;
	IEnd.o_model			= NET_Last.o_model;
	IEnd.o_torso			= NET_Last.o_torso;
	////////////////////////////////////

	m_bInInterpolation = true;
	m_dwIStartTime = m_dwLastUpdateTime;
	m_dwIEndTime = Level().timeServer() + u32((fixed_step - ph_world->m_frame_time)*1000)+ Level().GetInterpolationSteps()*u32(fixed_step*1000);
};

int		actInterpType = 0;
void CActor::make_Interpolation	()
{
	m_dwLastUpdateTime = Level().timeServer();
	if (!m_bInInterpolation) return;

	//	if (Local())
	{
	}
	//	else 
	{
		if(!H_Parent() /*&& getVisible()*/ && g_Alive()) 
		{
			u32 CurTime = m_dwLastUpdateTime;

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

				g_Orientate				(NET_Last.mstate,0			);

				if (OnClient())
					cam_Active()->Set		(-unaffected_r_torso_yaw,unaffected_r_torso_pitch,0);		// set's camera orientation
			};
		};
	};
};

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

