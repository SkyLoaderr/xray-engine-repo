// CustomMonster.cpp: implementation of the CCustomMonster class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_debug.h"
#include "CustomMonster.h"
#include "customitem.h"
#include "hudmanager.h"
#include "ai_space.h"
#ifdef IGNORE_ACTOR
	#include "actor.h"
#endif

//#include "ai_script_actions.h"
#include "ai/rat/ai_rat.h"
#include "ai/biting/ai_biting.h"
#include "ai/stalker/ai_stalker.h"

Flags32		psAI_Flags	= {0};
 
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
//	CCustomMonster*		M = dynamic_cast<CCustomMonster*> (static_cast<CObject*>(B->Callback_Param));
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
	Init				();
}

CCustomMonster::~CCustomMonster	()
{

}

void CCustomMonster::Init()
{
}

void CCustomMonster::Load		(LPCSTR section)
{
	inherited::Load				(section);
	CScriptMonster::Load		(section);
	CDamageManager::Load		(section);
	CEventMemoryManager::Load	(section);
	CLocationManager::Load		(section);
	CMaterialManager::Load		(section);
	CMemoryManager::Load		(section);
	CMovementManager::Load		(section);

	ISpatial*		self				= dynamic_cast<ISpatial*> (this);
	if (self)		{
		self->spatial.type	|= STYPE_VISIBLEFORAI;
		self->spatial.type	|= STYPE_REACTTOSOUND;
	}
//////////////////////////////////////////////////////////////////////////

	///////////
	// m_PhysicMovementControl: General
	m_PhysicMovementControl.SetParent		(this);
	m_PhysicMovementControl.Load			(section);
	//Fbox	bb;

	//// m_PhysicMovementControl: BOX
	//Fvector	vBOX0_center= pSettings->r_fvector3	(section,"ph_box0_center"	);
	//Fvector	vBOX0_size	= pSettings->r_fvector3	(section,"ph_box0_size"		);
	//bb.set	(vBOX0_center,vBOX0_center); bb.grow(vBOX0_size);
	//m_PhysicMovementControl.SetBox		(0,bb);

	//// m_PhysicMovementControl: BOX
	//Fvector	vBOX1_center= pSettings->r_fvector3	(section,"ph_box1_center"	);
	//Fvector	vBOX1_size	= pSettings->r_fvector3	(section,"ph_box1_size"		);
	//bb.set	(vBOX1_center,vBOX1_center); bb.grow(vBOX1_size);
	//m_PhysicMovementControl.SetBox		(1,bb);

	//// m_PhysicMovementControl: Foots
	//Fvector	vFOOT_center= pSettings->r_fvector3	(section,"ph_foot_center"	);
	//Fvector	vFOOT_size	= pSettings->r_fvector3	(section,"ph_foot_size"		);
	//bb.set	(vFOOT_center,vFOOT_center); bb.grow(vFOOT_size);
	//m_PhysicMovementControl.SetFoots	(vFOOT_center,vFOOT_size);

	//// m_PhysicMovementControl: Crash speed and mass
	//float	cs_min		= pSettings->r_float	(section,"ph_crash_speed_min"	);
	//float	cs_max		= pSettings->r_float	(section,"ph_crash_speed_max"	);
	//float	mass		= pSettings->r_float	(section,"ph_mass"				);
	//m_PhysicMovementControl.SetCrashSpeeds	(cs_min,cs_max);
	//m_PhysicMovementControl.SetMass		(mass);
	

	// m_PhysicMovementControl: Frictions
	/*
	float af, gf, wf;
	af					= pSettings->r_float	(section,"ph_friction_air"	);
	gf					= pSettings->r_float	(section,"ph_friction_ground");
	wf					= pSettings->r_float	(section,"ph_friction_wall"	);
	m_PhysicMovementControl.SetFriction	(af,wf,gf);

	// BOX activate
	m_PhysicMovementControl.ActivateBox	(0);
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
	CEntityAlive::reinit		();
	CScriptMonster::reinit		();
	CDamageManager::reinit		();
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
	CEntityAlive::reload		(section);
	CScriptMonster::reload		(section);
	CDamageManager::reload		(section);
	CEventMemoryManager::reload	(section);
	CLocationManager::reload	(section);
	CMaterialManager::reload	(section);
	CMemoryManager::reload		(section);
	CMovementManager::reload	(section);
//	CSelectorManager::reload	(section);
//	CSoundPlayer::reload		(section);
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
	P.w_float_q16		(fEntityHealth,-1000,1000);
	P.w_u32					(N.dwTimeStamp);
	P.w_u8					(0);
	P.w_vec3				(N.p_pos);
	P.w_angle8				(N.o_model);
	P.w_angle8				(N.o_torso.yaw);
	P.w_angle8				(N.o_torso.pitch);
}

void CCustomMonster::net_Import(NET_Packet& P)
{
	R_ASSERT				(Remote());
	net_update				N;

	u8 flags;
	
	float health;
	P.r_float_q16		(health,-1000,1000);
	fEntityHealth = health;

	P.r_u32					(N.dwTimeStamp);
	P.r_u8					(flags);
	P.r_vec3				(N.p_pos);
	P.r_angle8				(N.o_model);
	P.r_angle8				(N.o_torso.yaw);
	P.r_angle8				(N.o_torso.pitch);
	
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

	// Queue setup
	float dt			= float(DT)/1000.f;
	
	if (dt > 3) 
		return;

	// *** general stuff
	CMemoryManager::update		();
	inherited::shedule_Update	(DT);

	m_dwCurrentTime	= Level().timeServer();

	VERIFY				(_valid(Position()));
	if (Remote())		{
	} else {
		// here is monster AI call
		m_fTimeUpdateDelta				= dt;
		Device.Statistic.AI_Think.Begin	();
		if (GetScriptControl())
			ProcessScripts();
		else {
			Think						();
		}
		m_dwLastUpdateTime				= Level().timeServer();
		Device.Statistic.AI_Think.End	();

		Engine.Sheduler.Slice			();

		// Look and action streams
		if (fEntityHealth>0) {
			Exec_Action				(dt);
			VERIFY				(_valid(Position()));
			Exec_Look				(dt);
			VERIFY				(_valid(Position()));
			Exec_Visibility			();
			VERIFY				(_valid(Position()));
			//////////////////////////////////////
			Fvector C; float R;
			//////////////////////////////////////
			// � ����� - ����!!!! (���� :-))))
			// m_PhysicMovementControl.GetBoundingSphere	(C,R);
			//////////////////////////////////////
			Center(C);
			R = Radius();
			//////////////////////////////////////
			feel_touch_update		(C,R);

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
			move_along_path			(&m_PhysicMovementControl,NET_Last.p_pos,Device.fTimeDelta);
			if (!bfScriptAnimation())
				SelectAnimation		(XFORM().k,direction(),speed());
		}
		else {
			CAI_Rat				*l_tpRat = dynamic_cast<CAI_Rat*>(this);
			R_ASSERT			(l_tpRat);
			if (((dwTime > N.dwTimeStamp) || (NET.size() < 2)) && (_abs(l_tpRat->m_fSpeed) > EPS_L)) {
				Fmatrix				l_tSavedTransform = XFORM();
				m_fTimeUpdateDelta	= Device.fTimeDelta;
				l_tpRat->bfComputeNewPosition(l_tpRat->m_bCanAdjustSpeed,l_tpRat->m_bStraightForward);
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
			else
				if (!bfScriptAnimation())
					SelectAnimation		(XFORM().k,Fvector().set(1,0,0),l_tpRat->m_fSpeed);
		}
	}

	// Use interpolated/last state
	if(g_Alive()) {
		XFORM().rotateY				(NET_Last.o_model);
		XFORM().translate_over		(NET_Last.p_pos);

		Fmatrix M;
		M.setXYZi (NET_Last.o_torso.pitch, 0.0f, 0.0f);
		XFORM().mulB(M);
	}
}

BOOL CCustomMonster::feel_visible_isRelevant (CObject* O)
{
	CEntityAlive* E = dynamic_cast<CEntityAlive*>		(O);
	if (0==E)								return FALSE;
	if (E->g_Team() == g_Team())			return FALSE;
	return TRUE;
}

void CCustomMonster::eye_pp_s0			( )
{
	++eye_pp_stage;

	// Eye matrix
	CKinematics* V							= PKinematics(Visual());
	V->Calculate							();
	Fmatrix&	mEye						= V->LL_GetTransform(u16(eye_bone));
	Fmatrix		X;							X.mul_43	(XFORM(),mEye);
	VERIFY									(_valid(mEye));
	const CAI_Stalker						*stalker = dynamic_cast<const CAI_Stalker*>(this);
	const CMovementManager::SBoneRotation	&rotation = stalker ? stalker->head_orientation() : m_body;
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
	feel_vision_update						(this,eye_matrix.c,float(dwDT)/1000.f);
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

extern void dbg_draw_frustum (float FOV, float _FAR, float A, Fvector &P, Fvector &D, Fvector &U);
#ifdef DEBUG
void CCustomMonster::OnRender()
{
	//if (!bDebug)					return;
	if (!psAI_Flags.test(aiDebug))	return;

//	m_PhysicMovementControl.DBG_Render();

	RCache.OnFrameEnd				();
	{
	for (u32 I=1; I<CDetailPathManager::path().size(); ++I) {
		const CDetailPathManager::STravelPathPoint&	N1 = CDetailPathManager::path()[I-1];	Fvector	P1; P1.set(N1.position); P1.y+=0.1f;
		const CDetailPathManager::STravelPathPoint&	N2 = CDetailPathManager::path()[I];		Fvector	P2; P2.set(N2.position); P2.y+=0.1f;
		RCache.dbg_DrawLINE			(Fidentity,P1,P2,D3DCOLOR_XRGB(0,255,0));
		if ((CDetailPathManager::path().size() - 1) == I) // ��������� box?
			RCache.dbg_DrawAABB			(P1,.1f,.1f,.1f,D3DCOLOR_XRGB(255,0,0));
		else 
			RCache.dbg_DrawAABB			(P1,.1f,.1f,.1f,D3DCOLOR_XRGB(0,0,255));
		
		/**/
		Fvector         T;
        Fvector4        S;
        
        T.set			(CDetailPathManager::path()[I].position); T.y+=(Radius()*2);
        Device.mFullTransform.transform (S,T);
        
//		pApp->pFont->Size       (0.07f/sqrtf(_abs(S.w)));
//		pApp->pFont->Color      (color_rgba(0,255,0,(S.z<=0)?0:255));
//		pApp->pFont->Out		(S.x,-S.y,"%d",I);
		/**/
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
	
	for (int I=0; I<(int)CDetailPathManager::path().size() - 1; ++I)
	{
		Fvector P1 = CDetailPathManager::path()[I].position;		P1.y+=0.1f;
		Fvector P2 = CDetailPathManager::path()[I + 1].position;	P2.y+=0.1f;
		{
			xr_map<u32,STravelParams>::const_iterator	i = m_movement_params.find(CDetailPathManager::path()[I].velocity);
			VERIFY						(i != m_movement_params.end());
			float	r = (*i).second.linear_velocity/(*i).second.angular_velocity;
			Fmatrix						V = Fidentity;
			V.c							= P1;
			V.c.y						+= r;
			V.i.x						= .05f;//r;
			V.j.y						= .05f;//r;
			V.k.z						= .05f;//r;
			RCache.dbg_DrawEllipse		(V,D3DCOLOR_XRGB(255,0,0));
		}
		{
			xr_map<u32,STravelParams>::const_iterator	i = m_movement_params.find(CDetailPathManager::path()[I + 1].velocity);
			VERIFY										(i != m_movement_params.end());
			float	r = (*i).second.linear_velocity/(*i).second.angular_velocity;
			Fmatrix						V = Fidentity;
			V.c							= P2;
			V.c.y						+= r;
			V.i.x						= .05f;//r;
			V.j.y						= .05f;//r;
			V.k.z						= .05f;//r;
			RCache.dbg_DrawEllipse		(V,D3DCOLOR_XRGB(255,0,0));
		}
		RCache.dbg_DrawAABB(P1,.01f,.01f,.01f,D3DCOLOR_XRGB(255,255,255));
		RCache.dbg_DrawAABB(P2,.01f,.01f,.01f,D3DCOLOR_XRGB(255,255,255));
		RCache.dbg_DrawLINE(Fidentity,P1,P2,D3DCOLOR_XRGB(255,255,255));
	}
	}

	if (this == Level().Teams[g_Team()].Squads[g_Squad()].Leader) {
		CGroup &Group = Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()];
		for (unsigned i=0; i<Group.m_tpaSuspiciousNodes.size(); ++i) {
			Fvector tP0 = ai().level_graph().vertex_position(Group.m_tpaSuspiciousNodes[i].dwNodeID);
			tP0.y += .35f;
			if (!Group.m_tpaSuspiciousNodes[i].dwSearched)
				RCache.dbg_DrawAABB(tP0,.35f,.35f,.35f,D3DCOLOR_XRGB(255,0,0));
			else
				if (1 == Group.m_tpaSuspiciousNodes[i].dwSearched)
					RCache.dbg_DrawAABB(tP0,.35f,.35f,.35f,D3DCOLOR_XRGB(0,255,0));
				else
					RCache.dbg_DrawAABB(tP0,.35f,.35f,.35f,D3DCOLOR_XRGB(255,255,0));
			switch (Group.m_tpaSuspiciousNodes[i].dwGroup) {
				case 0 : {
					RCache.dbg_DrawAABB(tP0,.1f,.1f,.1f,D3DCOLOR_XRGB(255,0,0));
					break;
				}
				case 1 : {
					RCache.dbg_DrawAABB(tP0,.1f,.1f,.1f,D3DCOLOR_XRGB(0,255,0));
					break;
				}
				case 2 : {
					RCache.dbg_DrawAABB(tP0,.1f,.1f,.1f,D3DCOLOR_XRGB(0,0,255));
					break;
				}
				case 3 : {
					RCache.dbg_DrawAABB(tP0,.1f,.1f,.1f,D3DCOLOR_XRGB(255,255,0));
					break;
				}
				case 4 : {
					RCache.dbg_DrawAABB(tP0,.1f,.1f,.1f,D3DCOLOR_XRGB(255,0,255));
					break;
				}
				case 5 : {
					RCache.dbg_DrawAABB(tP0,.1f,.1f,.1f,D3DCOLOR_XRGB(0,255,255));
					break;
				}
				default : {
					RCache.dbg_DrawAABB(tP0,.1f,.1f,.1f,D3DCOLOR_XRGB(255,255,255));
					break;
				}
			}
		}
	}

	if (psAI_Flags.test(aiFrustum))
		dbg_draw_frustum(eye_fov,eye_range,1,eye_matrix.c,eye_matrix.k,eye_matrix.j);
	
	if (psAI_Flags.test(aiMotion)) 
	{
		//m_PhysicMovementControl.dbg_Draw();
	}
//	if (bDebug) PKinematics(Visual())->DebugRender(XFORM());


}
#endif

void CCustomMonster::HitSignal(float /**perc/**/, Fvector& /**vLocalDir/**/, CObject* /**who/**/)
{
}

void CCustomMonster::Die	()
{
}

BOOL CCustomMonster::net_Spawn	(LPVOID DC)
{
	if (!inherited::net_Spawn(DC) || !CScriptMonster::net_Spawn(DC))
		return					(FALSE);
	
	CDamageManager::Load		(cNameSect());

	CSE_Abstract				*e	= (CSE_Abstract*)(DC);
	CSE_ALifeMonsterAbstract	*E	= dynamic_cast<CSE_ALifeMonsterAbstract*>(e);
	
	set_level_dest_vertex	(level_vertex_id());
	
	eye_matrix.identity		();
	m_body.current.yaw		= m_body.target.yaw	= -E->o_Angle.y;
	m_body.current.pitch	= m_body.target.pitch	= 0;
	fEntityHealth			= E->fHealth;

	R_ASSERT				(MT_SKELETON_ANIM == Visual()->Type);

	// Eyes
	eye_bone				= PKinematics(Visual())->LL_BoneID(pSettings->r_string(cNameSect(),"bone_head"));

	// weapons
	if (Local()) {
		net_update				N;
		N.dwTimeStamp			= Level().timeServer()-NET_Latency;
		N.o_model				= E->o_Angle.y;
		N.o_torso.yaw			= E->o_Angle.y;
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

//void CCustomMonster::feel_touch_new				(CObject* O)
//{
//	NET_Packet	P;
//
//	// Test for weapon
//	CWeapon* W	= dynamic_cast<CWeapon*>	(O);
//	if (W)
//	{
//		// Search if we have similar type of weapon
//		/*
//		CWeapon* T = Weapons->getWeaponByWeapon	(W);
//		if (T)	
//		{
//			// We have similar weapon - just get ammo out of it
//			u_EventGen	(P,GE_TRANSFER_AMMO,ID());
//			P.w_u16		(u16(W->ID()));
//			P.w_u16		(u16(T->ID()));
//			u_EventSend	(P);
//			return;
//		} else {
//			// We doesn't have similar weapon - pick up it
//			u_EventGen	(P,GE_OWNERSHIP_TAKE,ID());
//			P.w_u16		(u16(W->ID()));
//			u_EventSend	(P);
//			return;
//		}
//		*/
//		Log("~~~~~~~~~~~~~~~~~~~~~ EVENT");
//		// We doesn't have similar weapon - pick up it
//		u_EventGen	(P,GE_OWNERSHIP_TAKE,ID());
//		P.w_u16		(u16(W->ID()));
//		u_EventSend	(P);
//		return;
//	}
//
//	// 


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
}

void CCustomMonster::OnEvent(NET_Packet& P, u16 type)
{
	inherited::OnEvent		(P,type);
}

void CCustomMonster::net_Destroy()
{
	inherited::net_Destroy	();
	CScriptMonster::net_Destroy();
}

BOOL CCustomMonster::UsedAI_Locations()
{
	return					(TRUE);
}
