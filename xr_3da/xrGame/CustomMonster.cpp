// CustomMonster.cpp: implementation of the CCustomMonster class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_debug.h"
#include "CustomMonster.h"
#include "hudmanager.h"
#include "ai_space.h"
#include "ai/rat/ai_rat.h"
#include "ai/biting/ai_biting.h"
#include "ai/stalker/ai_stalker.h"
#include "xrserver_objects_alife_monsters.h"
#include "xrserver.h"
#include "seniority_hierarchy_holder.h"
#include "team_hierarchy_holder.h"
#include "squad_hierarchy_holder.h"
#include "group_hierarchy_holder.h"
#include "customzone.h"

#ifdef DEBUG
Flags32		psAI_Flags	= {0};
#else
Flags32		psAI_Flags	= {aiLua};
#endif
 
void CCustomMonster::SAnimState::Create(CSkeletonAnimated* K, LPCSTR base)
{
	char	buf[128];
	fwd		= K->ID_Cycle_Safe(strconcat(buf,base,"_fwd"));
	back	= K->ID_Cycle_Safe(strconcat(buf,base,"_back"));
	ls		= K->ID_Cycle_Safe(strconcat(buf,base,"_ls"));
	rs		= K->ID_Cycle_Safe(strconcat(buf,base,"_rs"));
}

//void __stdcall CCustomMonster::TorsoSpinCallback(CBoneInstance* B)
//{
//	CCustomMonster*		M = static_cast<CCustomMonster*> (B->Callback_Param);
//
//	Fmatrix					spin;
//	spin.setXYZ				(0, M->NET_Last.o_torso.pitch, 0);
//	B->mTransform.mulB_43	(spin);
//}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCustomMonster::CCustomMonster()
{
	init				();
}

CCustomMonster::~CCustomMonster	()
{
}

void CCustomMonster::init()
{
	m_client_update_activated	= false;
}

void CCustomMonster::Load		(LPCSTR section)
{
	inherited::Load				(section);
	CScriptMonster::Load		(section);
	CEventMemoryManager::Load	(section);
	CLocationManager::Load		(section);
	CMaterialManager::Load		(section);
	CMemoryManager::Load		(section);
	CMovementManager::Load		(section);

	ISpatial*		self				= smart_cast<ISpatial*> (this);
	if (self)		{
		self->spatial.type	|= STYPE_VISIBLEFORAI;
		self->spatial.type	|= STYPE_REACTTOSOUND;
	}
//////////////////////////////////////////////////////////////////////////

	///////////
	// m_PhysicMovementControl: General
	m_PhysicMovementControl->SetParent		(this);
	m_PhysicMovementControl->Load			(section);
	//Fbox	bb;

	//// m_PhysicMovementControl: BOX
	//Fvector	vBOX0_center= pSettings->r_fvector3	(section,"ph_box0_center"	);
	//Fvector	vBOX0_size	= pSettings->r_fvector3	(section,"ph_box0_size"		);
	//bb.set	(vBOX0_center,vBOX0_center); bb.grow(vBOX0_size);
	//m_PhysicMovementControl->SetBox		(0,bb);

	//// m_PhysicMovementControl: BOX
	//Fvector	vBOX1_center= pSettings->r_fvector3	(section,"ph_box1_center"	);
	//Fvector	vBOX1_size	= pSettings->r_fvector3	(section,"ph_box1_size"		);
	//bb.set	(vBOX1_center,vBOX1_center); bb.grow(vBOX1_size);
	//m_PhysicMovementControl->SetBox		(1,bb);

	//// m_PhysicMovementControl: Foots
	//Fvector	vFOOT_center= pSettings->r_fvector3	(section,"ph_foot_center"	);
	//Fvector	vFOOT_size	= pSettings->r_fvector3	(section,"ph_foot_size"		);
	//bb.set	(vFOOT_center,vFOOT_center); bb.grow(vFOOT_size);
	//m_PhysicMovementControl->SetFoots	(vFOOT_center,vFOOT_size);

	//// m_PhysicMovementControl: Crash speed and mass
	//float	cs_min		= pSettings->r_float	(section,"ph_crash_speed_min"	);
	//float	cs_max		= pSettings->r_float	(section,"ph_crash_speed_max"	);
	//float	mass		= pSettings->r_float	(section,"ph_mass"				);
	//m_PhysicMovementControl->SetCrashSpeeds	(cs_min,cs_max);
	//m_PhysicMovementControl->SetMass		(mass);
	

	// m_PhysicMovementControl: Frictions
	/*
	float af, gf, wf;
	af					= pSettings->r_float	(section,"ph_friction_air"	);
	gf					= pSettings->r_float	(section,"ph_friction_ground");
	wf					= pSettings->r_float	(section,"ph_friction_wall"	);
	m_PhysicMovementControl->SetFriction	(af,wf,gf);

	// BOX activate
	m_PhysicMovementControl->ActivateBox	(0);
	*/
	////////

	Position().y			+= EPS_L;
	
//	m_current			= 0;

	if (pSettings->line_exist(section,"eye_fov"))
		eye_fov						= pSettings->r_float(section,"eye_fov");
	else
		eye_fov						= pSettings->r_float(section,"EyeFov");
	eye_range						= pSettings->r_float(section,"eye_range");

	// Health & Armor
	fArmor				= 0;
	
	// Sheduler
	shedule.t_min			= 50;
	shedule.t_max			= 500; // 30 * NET_Latency / 4;

	// Msg				("! cmonster size: %d",sizeof(*this));
}

void CCustomMonster::reinit()
{
	CScriptMonster::reinit		();
	CEntityAlive::reinit		();
	CEventMemoryManager::reinit	();
	CLocationManager::reinit	();
	CMaterialManager::reinit	();
	CMemoryManager::reinit		();
	CMovementManager::reinit	();
	CSelectorManager::reinit	();
	CSoundPlayer::reinit		();

	eye_pp_stage				= 0;
	m_dwLastUpdateTime			= 0xffffffff;
	m_tEyeShift.set				(0,0,0);
	m_fEyeShiftYaw				= 0.f;
	NET_WasExtrapolating		= FALSE;
}

void CCustomMonster::reload		(LPCSTR section)
{
	CSoundPlayer::reload		(section);
	CScriptMonster::reload		(section);
	CEntityAlive::reload		(section);
	CEventMemoryManager::reload	(section);
	CLocationManager::reload	(section);
	CMaterialManager::reload	(section);
	CMemoryManager::reload		(section);
	CMovementManager::reload	(section);
//	CSelectorManager::reload	(section);
}

void CCustomMonster::mk_orientation(Fvector &dir, Fmatrix& mR)
{
	// orient only in XZ plane
	dir.y		= 0;
	float		len = dir.magnitude();
	if			(len>EPS_S)
	{
		// normalize
		dir.x /= len;
		dir.z /= len;
		Fvector up;	up.set(0,1,0);
		mR.rotation	(dir,up);
	}
}

void CCustomMonster::net_Export(NET_Packet& P)					// export to server
{
	R_ASSERT				(Local());

	// export last known packet
	R_ASSERT				(!NET.empty());
	net_update& N			= NET.back();
	P.w_float_q16		(fEntityHealth,-500,1000);
	P.w_u32					(N.dwTimeStamp);
	P.w_u8					(0);
	P.w_vec3				(N.p_pos);
	P.w_angle8				(N.o_model);
	P.w_angle8				(N.o_torso.yaw);
	P.w_angle8				(N.o_torso.pitch);
	P.w_u8					(u8(g_Team()));
	P.w_u8					(u8(g_Squad()));
	P.w_u8					(u8(g_Group()));
}

void CCustomMonster::net_Import(NET_Packet& P)
{
	R_ASSERT				(Remote());
	net_update				N;

	u8 flags;
	
	float health;
	P.r_float_q16		(health,-500,1000);
	fEntityHealth = health;

	P.r_u32					(N.dwTimeStamp);
	P.r_u8					(flags);
	P.r_vec3				(N.p_pos);
	P.r_angle8				(N.o_model);
	P.r_angle8				(N.o_torso.yaw);
	P.r_angle8				(N.o_torso.pitch);
	
	id_Team					= P.r_u8();
	id_Squad				= P.r_u8();
	id_Group				= P.r_u8();
	
	if (NET.empty() || (NET.back().dwTimeStamp<N.dwTimeStamp))	{
		NET.push_back			(N);
		NET_WasInterpolating	= TRUE;
	}

	setVisible				(TRUE);
	setEnabled				(TRUE);
}

void CCustomMonster::Exec_Physics( float /**dt/**/)
{
	// 
	Engine.Sheduler.Slice	();
}

void CCustomMonster::shedule_Update	( u32 DT )
{
	// Queue shrink
	VERIFY				(_valid(Position()));
	u32	dwTimeCL		= Level().timeServer()-NET_Latency;
	VERIFY				(!NET.empty());
	while ((NET.size()>2) && (NET[1].dwTimeStamp<dwTimeCL)) NET.pop_front();

	float dt			= float(DT)/1000.f;
	// *** general stuff
	if (g_Alive())
		CMemoryManager::update	(dt);
	inherited::shedule_Update	(DT);

	// Queue setup
	if (dt > 3) return;

	m_dwCurrentTime	= Level().timeServer();

	VERIFY				(_valid(Position()));
	if (Remote())		{
	} else {
		// here is monster AI call
		m_fTimeUpdateDelta				= dt;
		Device.Statistic.AI_Think.Begin	();
		Device.Statistic.TEST1.Begin();
		if (GetScriptControl())
			ProcessScripts();
		else {
			Think						();
		}
		m_dwLastUpdateTime				= Level().timeServer();
		Device.Statistic.TEST1.End();
		Device.Statistic.AI_Think.End	();

		Engine.Sheduler.Slice			();

		// Look and action streams
		if (fEntityHealth>0) {
			Exec_Action				(dt);
			VERIFY				(_valid(Position()));
			Exec_Visibility			();
			VERIFY				(_valid(Position()));
			//////////////////////////////////////
			Fvector C; float R;
			//////////////////////////////////////
			// С Олеся - ПИВО!!!! (Диме :-))))
			// m_PhysicMovementControl->GetBoundingSphere	(C,R);
			//////////////////////////////////////
			Center(C);
			R = Radius();
			//////////////////////////////////////
/// #pragma todo("Oles to all AI guys: perf/logical problem: Only few objects needs 'feel_touch' why to call update for everybody?")
///			feel_touch_update		(C,R);

			net_update				uNext;
			uNext.dwTimeStamp		= Level().timeServer();
			uNext.o_model			= m_body.current.yaw;
			uNext.o_torso			= m_body.current;
			uNext.p_pos				= Position();
			uNext.fHealth			= fEntityHealth;
			NET.push_back			(uNext);
		}
		else 
		{
			net_update			uNext;
			uNext.dwTimeStamp	= Level().timeServer();
			uNext.o_model		= m_body.current.yaw;
			uNext.o_torso		= m_body.current;
			uNext.p_pos			= Position();
			uNext.fHealth		= fEntityHealth;
			NET.push_back		(uNext);
		}
	}
}

void CCustomMonster::net_update::lerp(CCustomMonster::net_update& A, CCustomMonster::net_update& B, float f)
{
	// 
	o_model			= angle_lerp	(A.o_model,B.o_model,				f);
	o_torso.yaw		= angle_lerp	(A.o_torso.yaw,B.o_torso.yaw,		f);
	o_torso.pitch	= angle_lerp	(A.o_torso.pitch,B.o_torso.pitch,	f);
	p_pos.lerp		(A.p_pos,B.p_pos,f);
	fHealth	= A.fHealth*(1.f - f) + B.fHealth*f;
}

void CCustomMonster::UpdateCL	()
{ 
	inherited::UpdateCL					();
	
	if (!g_Alive() && CSoundPlayer::playing_sounds().empty() && m_client_update_activated) {
		processing_deactivate	();
		m_client_update_activated = false;
	}

	CScriptMonster::process_sound_callbacks();
	CEventMemoryManager::update			();
	CSoundPlayer::update				(Device.fTimeDelta);
	if	(NET.empty())	return;
	
	m_dwCurrentTime	= Level().timeServer();
	
	// distinguish interpolation/extrapolation
	u32	dwTime			= Level().timeServer()-NET_Latency;
	net_update&	N		= NET.back();
	if ((dwTime > N.dwTimeStamp) || (NET.size() < 2)) {
		// BAD.	extrapolation
		NET_Last		= N;
	}
	else {
		// OK.	interpolation
		NET_WasExtrapolating		= FALSE;
		// Search 2 keyframes for interpolation
		int select		= -1;
		for (u32 id=0; id<NET.size()-1; ++id)
		{
			if ((NET[id].dwTimeStamp<=dwTime)&&(dwTime<=NET[id+1].dwTimeStamp))	select=id;
		}
		if (select>=0)
		{
			// Interpolate state
			net_update&	A			= NET[select+0];
			net_update&	B			= NET[select+1];
			u32	d1					= dwTime-A.dwTimeStamp;
			u32	d2					= B.dwTimeStamp - A.dwTimeStamp;
			float	factor			= (float(d1)/float(d2));
			Fvector					l_tOldPosition = Position();
			NET_Last.lerp			(A,B,factor);
			if (Local()) {
				if (CLSID_AI_RAT != SUB_CLS_ID) {
					NET_Last.p_pos		= l_tOldPosition;
				}
			}
			else {
				if (!bfScriptAnimation())
					SelectAnimation	(XFORM().k,direction(),speed());
			}
			
			// Signal, that last time we used interpolation
			NET_WasInterpolating	= TRUE;
			NET_Time				= dwTime;
		}
	}

	if (Local() && g_Alive()) {
#pragma todo("Dima to All : this is FAKE, network is not supported here!")
		
		if (CLSID_AI_RAT != SUB_CLS_ID) {
			move_along_path			(m_PhysicMovementControl,NET_Last.p_pos,Device.fTimeDelta);
			if (!bfScriptAnimation())
				SelectAnimation		(XFORM().k,direction(),speed());
		}
		else {
			CAI_Rat				*l_tpRat = dcast_Rat();
			R_ASSERT			(l_tpRat);
//			if (((dwTime > N.dwTimeStamp) || (NET.size() < 2))) {// && (_abs(l_tpRat->m_fSpeed) > EPS_L)) 
			{
				Fmatrix				l_tSavedTransform = XFORM();
				m_fTimeUpdateDelta	= Device.fTimeDelta;
				l_tpRat->vfComputeNewPosition(l_tpRat->m_bCanAdjustSpeed,l_tpRat->m_bStraightForward);
				float				y,p,b;
				XFORM().getHPB		(y,p,b);
				NET_Last.p_pos		= Position();
				NET_Last.o_model	= -y;
				NET_Last.o_torso.yaw= -y;
				NET_Last.o_torso.pitch= -p;
				XFORM()				= l_tSavedTransform;
				if (!bfScriptAnimation())
					SelectAnimation		(XFORM().k,Fvector().set(1,0,0),l_tpRat->m_fSpeed);
			}
//			else
//				if (!bfScriptAnimation())
//					SelectAnimation		(XFORM().k,Fvector().set(1,0,0),l_tpRat->m_fSpeed);
		}
	}

	// Use interpolated/last state
	if(g_Alive()) {
		XFORM().rotateY				(NET_Last.o_model);
		XFORM().translate_over		(NET_Last.p_pos);

		if (use_model_pitch()) {
			Fmatrix M;
			M.setXYZi (NET_Last.o_torso.pitch, 0.0f, 0.0f);
			XFORM().mulB(M);
		}
	}
}

BOOL CCustomMonster::feel_visible_isRelevant (CObject* O)
{
	CEntityAlive* E = smart_cast<CEntityAlive*>		(O);
	if (0==E)								return FALSE;
	if (E->g_Team() == g_Team())			return FALSE;
	return TRUE;
}

void CCustomMonster::eye_pp_s0			( )
{
	++eye_pp_stage;

	// Eye matrix
	CKinematics* V							= PKinematics(Visual());
	V->CalculateBones						();
	Fmatrix&	mEye						= V->LL_GetTransform(u16(eye_bone));
	Fmatrix		X;							X.mul_43	(XFORM(),mEye);
	VERIFY									(_valid(mEye));
	const CAI_Stalker						*stalker = smart_cast<const CAI_Stalker*>(this);
	const MonsterSpace::SBoneRotation		&rotation = stalker ? stalker->head_orientation() : m_body;
	eye_matrix.setHPB						(-rotation.current.yaw + m_fEyeShiftYaw,-rotation.current.pitch,0);
	eye_matrix.c.add						(X.c,m_tEyeShift);
}

void CCustomMonster::eye_pp_s1			( )
{
	++eye_pp_stage;

	// Standart visibility
	Device.Statistic.AI_Vis_Query.Begin		();
	Fmatrix									mProject,mFull,mView;
	mView.build_camera_dir					(eye_matrix.c,eye_matrix.k,eye_matrix.j);
	VERIFY									(_valid(eye_matrix));
	mProject.build_projection				(deg2rad(eye_fov),1,0.1f,eye_range);
	mFull.mul								(mProject,mView);
	feel_vision_query						(mFull,eye_matrix.c);
	Device.Statistic.AI_Vis_Query.End		();
}

void CCustomMonster::eye_pp_s2			( )
{
	++eye_pp_stage;

	// Tracing
	Device.Statistic.AI_Vis_RayTests.Begin	();
	u32 dwTime			= Level().timeServer();
	u32 dwDT			= dwTime-eye_pp_timestamp;
	eye_pp_timestamp	= dwTime;
	feel_vision_update						(this,eye_matrix.c,float(dwDT)/1000.f,transparency_threshold());
	Device.Statistic.AI_Vis_RayTests.End	();
}

void CCustomMonster::Exec_Visibility	( )
{
	if (0==Sector())				return;

	Device.Statistic.AI_Vis.Begin	();
	switch (eye_pp_stage%3)	
	{
	case 0:	eye_pp_s0();			break;
	case 1:	eye_pp_s1();			break;
	case 2:	eye_pp_s2();			break;
	}
	Device.Statistic.AI_Vis.End		();

	// Camera
	if (IsMyCamera())						
		g_pGameLevel->Cameras.Update	(eye_matrix.c,eye_matrix.k,eye_matrix.j,eye_fov,1.f,eye_range);

	// Slice
	Engine.Sheduler.Slice();
}

#ifdef DEBUG

extern void dbg_draw_frustum (float FOV, float _FAR, float A, Fvector &P, Fvector &D, Fvector &U);

void CCustomMonster::OnRender()
{
	//if (!bDebug)					return;
	//if (!psAI_Flags.test(aiDebug))	return;

//	m_PhysicMovementControl->DBG_Render();

	RCache.OnFrameEnd				();
	for (int i=0; i<1; ++i) {
		const xr_vector<STravelPoint>		&keys	= !i ? m_key_points					: m_key_points;
		const xr_vector<STravelPathPoint>	&path	= !i ? CDetailPathManager::path()	: CDetailPathManager::path();
		u32									color0	= !i ? D3DCOLOR_XRGB(0,255,0)		: D3DCOLOR_XRGB(0,0,255);
		u32									color1	= !i ? D3DCOLOR_XRGB(255,0,0)		: D3DCOLOR_XRGB(255,255,0);
		u32									color2	= !i ? D3DCOLOR_XRGB(0,0,255)		: D3DCOLOR_XRGB(0,255,255);
		u32									color3	= !i ? D3DCOLOR_XRGB(255,255,255)	: D3DCOLOR_XRGB(255,0,255);
		float								radius0 = !i ? .1f : .15f;
		float								radius1 = !i ? .2f : .3f;
		{
			for (u32 I=1; I<path.size(); ++I) {
				const DetailPathManager::STravelPathPoint&	N1 = path[I-1];	Fvector	P1; P1.set(N1.position); P1.y+=0.1f;
				const DetailPathManager::STravelPathPoint&	N2 = path[I];	Fvector	P2; P2.set(N2.position); P2.y+=0.1f;
				RCache.dbg_DrawLINE			(Fidentity,P1,P2,color0);
				if ((path.size() - 1) == I) // песледний box?
					RCache.dbg_DrawAABB			(P1,radius0,radius0,radius0,color1);
				else 
					RCache.dbg_DrawAABB			(P1,radius0,radius0,radius0,color2);
			}

			for (u32 I=1; I<keys.size(); ++I) {
				CDetailPathManager::STravelPoint	temp;
				temp		= keys[I - 1]; 
				Fvector		P1;
				P1.set		(temp.position.x,ai().level_graph().vertex_plane_y(temp.vertex_id),temp.position.y);
				P1.y		+= 0.1f;

				temp		= keys[I]; 
				Fvector		P2;
				P2.set		(temp.position.x,ai().level_graph().vertex_plane_y(temp.vertex_id),temp.position.y);
				P2.y		+= 0.1f;

				RCache.dbg_DrawLINE			(Fidentity,P1,P2,color1);
				RCache.dbg_DrawAABB			(P1,radius1,radius1,radius1,color3);
			}
		}
	}
	/*
	{
	for (u32 I=1; I<AI_Path.TravelPath_dbg.size(); ++I)
	{
		CTravelNode&	N1 = AI_Path.TravelPath_dbg[I-1];	Fvector	P1; P1.set(N1.P); P1.y+=0.1f;
		CTravelNode&	N2 = AI_Path.TravelPath_dbg[I];		Fvector	P2; P2.set(N2.P); P2.y+=0.1f;
		RCache.dbg_DrawLINE(precalc_identity,P1,P2,D3DCOLOR_XRGB(255,0,0));
		RCache.dbg_DrawAABB(P1,.1f,.1f,.1f,D3DCOLOR_XRGB(255,0,0));
	}
	}
	*/
	{
	
//	for (int I=0, N = (int)CDetailPathManager::path().size(); I<N - 1; ++I)
//	{
//		Fvector P1 = CDetailPathManager::path()[I].position;		P1.y+=0.1f;
//		Fvector P2 = CDetailPathManager::path()[I + 1].position;	P2.y+=0.1f;
//		{
//			const STravelParams&		i = velocity(CDetailPathManager::path()[I].velocity);
//			float	r = i.linear_velocity/i.angular_velocity;
//			Fmatrix						V = Fidentity;
//			V.c							= P1;
//			V.c.y						+= r + float(I)/100.f;
//			V.i.x						= .05f;//r;
//			V.j.y						= .05f;//r;
//			V.k.z						= .05f;//r;
//			RCache.dbg_DrawEllipse		(V,D3DCOLOR_XRGB(255,0,0));
//		}
//		{
//			const STravelParams&		i = velocity(CDetailPathManager::path()[I + 1].velocity);
//			float	r = i.linear_velocity/i.angular_velocity;
//			Fmatrix						V = Fidentity;
//			V.c							= P2;
//			V.c.y						+= r + float(I + 1)/100.f;
//			V.i.x						= .05f;//r;
//			V.j.y						= .05f;//r;
//			V.k.z						= .05f;//r;
//			RCache.dbg_DrawEllipse		(V,D3DCOLOR_XRGB(255,0,0));
//		}
//		RCache.dbg_DrawAABB(P1,.01f,.01f,.01f,D3DCOLOR_XRGB(255,255,255));
//		RCache.dbg_DrawAABB(P2,.01f,.01f,.01f,D3DCOLOR_XRGB(255,255,255));
//		RCache.dbg_DrawLINE(Fidentity,P1,P2,D3DCOLOR_XRGB(255,255,255));
//	}
	}

//	if (this == Level().Teams[g_Team()].Squads[g_Squad()].Leader) {
//		CGroup &Group = Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()];
//		for (unsigned i=0; i<Group.m_tpaSuspiciousNodes.size(); ++i) {
//			Fvector tP0 = ai().level_graph().vertex_position(Group.m_tpaSuspiciousNodes[i].dwNodeID);
//			tP0.y += .35f;
//			if (!Group.m_tpaSuspiciousNodes[i].dwSearched)
//				RCache.dbg_DrawAABB(tP0,.35f,.35f,.35f,D3DCOLOR_XRGB(255,0,0));
//			else
//				if (1 == Group.m_tpaSuspiciousNodes[i].dwSearched)
//					RCache.dbg_DrawAABB(tP0,.35f,.35f,.35f,D3DCOLOR_XRGB(0,255,0));
//				else
//					RCache.dbg_DrawAABB(tP0,.35f,.35f,.35f,D3DCOLOR_XRGB(255,255,0));
//			switch (Group.m_tpaSuspiciousNodes[i].dwGroup) {
//				case 0 : {
//					RCache.dbg_DrawAABB(tP0,.1f,.1f,.1f,D3DCOLOR_XRGB(255,0,0));
//					break;
//				}
//				case 1 : {
//					RCache.dbg_DrawAABB(tP0,.1f,.1f,.1f,D3DCOLOR_XRGB(0,255,0));
//					break;
//				}
//				case 2 : {
//					RCache.dbg_DrawAABB(tP0,.1f,.1f,.1f,D3DCOLOR_XRGB(0,0,255));
//					break;
//				}
//				case 3 : {
//					RCache.dbg_DrawAABB(tP0,.1f,.1f,.1f,D3DCOLOR_XRGB(255,255,0));
//					break;
//				}
//				case 4 : {
//					RCache.dbg_DrawAABB(tP0,.1f,.1f,.1f,D3DCOLOR_XRGB(255,0,255));
//					break;
//				}
//				case 5 : {
//					RCache.dbg_DrawAABB(tP0,.1f,.1f,.1f,D3DCOLOR_XRGB(0,255,255));
//					break;
//				}
//				default : {
//					RCache.dbg_DrawAABB(tP0,.1f,.1f,.1f,D3DCOLOR_XRGB(255,255,255));
//					break;
//				}
//			}
//		}
//	}

	if (psAI_Flags.test(aiFrustum))
		dbg_draw_frustum(eye_fov,eye_range,1,eye_matrix.c,eye_matrix.k,eye_matrix.j);
	
	if (psAI_Flags.test(aiMotion)) 
	{
		m_PhysicMovementControl->dbg_Draw();
	}
	if (bDebug) PKinematics(Visual())->DebugRender(XFORM());
}
#endif

void CCustomMonster::HitSignal(float /**perc/**/, Fvector& /**vLocalDir/**/, CObject* /**who/**/)
{
}

void CCustomMonster::Die	()
{
	inherited::Die			();
	Level().RemoveMapLocationByID(this->ID());
}

BOOL CCustomMonster::net_Spawn	(LPVOID DC)
{
	VERIFY						(!m_client_update_activated);

	if (!inherited::net_Spawn(DC) || !CScriptMonster::net_Spawn(DC) || !CMovementManager::net_Spawn(DC))
		return					(FALSE);
	
	CSE_Abstract				*e	= (CSE_Abstract*)(DC);
	CSE_ALifeMonsterAbstract	*E	= smart_cast<CSE_ALifeMonsterAbstract*>(e);

	eye_matrix.identity			();
	m_body.current.yaw			= m_body.target.yaw	= -E->o_Angle.y;
	m_body.current.pitch		= m_body.target.pitch	= 0;
	fEntityHealth				= E->fHealth;
	if (!g_Alive())
		set_death_time			();

	if (ai().get_level_graph() && UsedAI_Locations() && (e->ID_Parent == 0xffff))
		set_level_dest_vertex	(level_vertex_id());

	// Eyes
	if (g_Alive()) {
		processing_activate		();
		m_client_update_activated = true;
	}

	eye_bone					= PKinematics(Visual())->LL_BoneID(pSettings->r_string(cNameSect(),"bone_head"));

	// weapons
	if (Local()) {
		net_update				N;
		N.dwTimeStamp			= Level().timeServer()-NET_Latency;
		N.o_model				= -E->o_Angle.y;
		N.o_torso.yaw			= -E->o_Angle.y;
		N.o_torso.pitch			= 0;
		N.p_pos.set				(Position());
		NET.push_back			(N);

		N.dwTimeStamp			+= NET_Latency;
		NET.push_back			(N);

		setVisible				(TRUE);
		setEnabled				(TRUE);
	}

	return TRUE;
}

#ifdef DEBUG
void CCustomMonster::OnHUDDraw(CCustomHUD* /**hud/**/)
{
	HUD().pFontSmall->SetColor	(D3DCOLOR_XRGB(255,0,0));
	HUD().pFontSmall->OutSet	(0,200);
	HUD().pFontSmall->OutNext	("NET_Q_size: %d",NET.size());
	
	/*
	const float			fy = -1;
	const float			fx = 0.5f;
	pApp->pFont->OutSet	(-1,fy+0*fx);
	pApp->pFont->Color	(D3DCOLOR_XRGB(0,0xff,0));
	pApp->pFont->OutNext("LOOK");
	DumpStream			(&q_look);

	pApp->pFont->OutSet	(-1,fy+1*fx);
	pApp->pFont->Color	(D3DCOLOR_XRGB(0,0xff,0));
	pApp->pFont->OutNext("MOVE");
	DumpStream			(&q_move);
	
	pApp->pFont->OutSet	(-1,fy+2*fx);
	pApp->pFont->Color	(D3DCOLOR_XRGB(0,0xff,0));
	pApp->pFont->OutNext("ACTION");
	DumpStream			(&q_action);
	*/
}
#endif


void CCustomMonster::Exec_Action(float /**dt/**/)
{
}


void CCustomMonster::renderable_Render()
{
	inherited::renderable_Render		();
}

void CCustomMonster::Hit(float P, Fvector &dir,CObject* who, s16 element,Fvector position_in_object_space, float impulse, ALife::EHitType hit_type)
{
	inherited::Hit			(P,dir,who,element,position_in_object_space,impulse, hit_type);
#pragma todo("Dima to Kostia : Please check if this is correct")
//	CMovementManager::Hit	(P,dir,who,element,position_in_object_space,impulse, hit_type);
}

void CCustomMonster::OnEvent(NET_Packet& P, u16 type)
{
	inherited::OnEvent		(P,type);
}

void CCustomMonster::net_Destroy()
{
	inherited::net_Destroy	();
	CScriptMonster::net_Destroy();
	CMovementManager::net_Destroy();
	if (m_client_update_activated) {
		processing_deactivate	();
		m_client_update_activated = false;
	}
}

BOOL CCustomMonster::UsedAI_Locations()
{
	return					(TRUE);
}

bool CCustomMonster::use_model_pitch	() const
{
	return					(true);
}

void CCustomMonster::ChangeTeam(int team, int squad, int group)
{
	if ((team == g_Team()) && (squad == g_Squad()) && (group == g_Group())) return;

	// remove from current team
	Level().seniority_holder().team(g_Team()).squad(g_Squad()).group(g_Group()).unregister_member	(this);

	id_Team					= team;
	id_Squad				= squad;
	id_Group				= group;

	// add to new team
	Level().seniority_holder().team(g_Team()).squad(g_Squad()).group(g_Group()).register_member		(this);
}

void CCustomMonster::PitchCorrection() 
{
	CLevelGraph::SContour	contour;
	ai().level_graph().contour(contour, level_vertex_id());
	
	Fplane  P;
	P.build(contour.v1,contour.v2,contour.v3);

	Fvector position_on_plane;
	P.project(position_on_plane,Position());

	// находим проекцию точки, лежащей на векторе текущего направления
	Fvector dir_point, proj_point;
	dir_point.mad(position_on_plane, Direction(), 1.f);
	P.project(proj_point,dir_point);
	
	// получаем искомый вектор направления
	Fvector target_dir;
	target_dir.sub(proj_point,position_on_plane);

	float yaw,pitch;
	target_dir.getHP(yaw,pitch);

	m_body.target.pitch = -pitch;

}

BOOL CCustomMonster::feel_touch_on_contact	(CObject *O)
{
	CCustomZone	*custom_zone = smart_cast<CCustomZone*>(O);
	if (!custom_zone)
		return	(TRUE);

	if (custom_zone->inside(Position()))
		return	(TRUE);

	return		(FALSE);
}

BOOL CCustomMonster::feel_touch_contact		(CObject *O)
{
	CCustomZone	*custom_zone = smart_cast<CCustomZone*>(O);
	if (!custom_zone)
		return	(TRUE);

	if (custom_zone->inside(Position()))
		return	(TRUE);

	return		(FALSE);
}
