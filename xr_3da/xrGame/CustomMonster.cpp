// CustomMonster.cpp: implementation of the CCustomMonster class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_console.h"
#include "CustomMonster.h"
#include "xr_weapon_list.h"
#include "customitem.h"
#include "hudmanager.h"
#include "ai_space.h"
#ifdef IGNORE_ACTOR
	#include "actor.h"
#endif

using namespace AI;

Flags32		psAI_Flags	= {0};
ENGINE_API extern float psGravity;

void CCustomMonster::SAnimState::Create(CKinematics* K, LPCSTR base)
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
//	Weapons				= 0;
	tWatchDirection		= Direction();
	m_cBodyState		= BODY_STATE_STAND;
	r_torso_speed		= PI;
	r_spine_speed		= PI;
	q_look.o_look_speed = PI;
	m_fSoundPower		= m_fStartPower = 0;
	m_dwSoundUpdate		= 0;
	eye_pp_stage		= 0;
	m_tpPath			= 0;
	vfResetPatrolData	();
	m_dwLastUpdateTime	= 0xffffffff;
	m_tEyeShift.set		(0,0,0);
	NET_WasExtrapolating = FALSE;

	ISpatial*		self				= dynamic_cast<ISpatial*> (this);
	if (self)		{
		self->spatial.type	|= STYPE_VISIBLEFORAI;
		self->spatial.type	|= STYPE_REACTTOSOUND;
	}
}

CCustomMonster::~CCustomMonster	()
{
}

void CCustomMonster::Load		(LPCSTR section)
{
	inherited::Load		(section);
	///////////
	// Movement: General
	Movement.SetParent		(this);
	Fbox	bb;

	// Movement: BOX
	Fvector	vBOX0_center= pSettings->r_fvector3	(section,"ph_box0_center"	);
	Fvector	vBOX0_size	= pSettings->r_fvector3	(section,"ph_box0_size"		);
	bb.set	(vBOX0_center,vBOX0_center); bb.grow(vBOX0_size);
	Movement.SetBox		(0,bb);

	// Movement: BOX
	Fvector	vBOX1_center= pSettings->r_fvector3	(section,"ph_box1_center"	);
	Fvector	vBOX1_size	= pSettings->r_fvector3	(section,"ph_box1_size"		);
	bb.set	(vBOX1_center,vBOX1_center); bb.grow(vBOX1_size);
	Movement.SetBox		(1,bb);

	// Movement: Foots
	Fvector	vFOOT_center= pSettings->r_fvector3	(section,"ph_foot_center"	);
	Fvector	vFOOT_size	= pSettings->r_fvector3	(section,"ph_foot_size"		);
	bb.set	(vFOOT_center,vFOOT_center); bb.grow(vFOOT_size);
	Movement.SetFoots	(vFOOT_center,vFOOT_size);

	// Movement: Crash speed and mass
	float	cs_min		= pSettings->r_float	(section,"ph_crash_speed_min"	);
	float	cs_max		= pSettings->r_float	(section,"ph_crash_speed_max"	);
	float	mass		= pSettings->r_float	(section,"ph_mass"				);
	Movement.SetCrashSpeeds	(cs_min,cs_max);
	Movement.SetMass		(mass);
	

	// Movement: Frictions
	/*
	float af, gf, wf;
	af					= pSettings->r_float	(section,"ph_friction_air"	);
	gf					= pSettings->r_float	(section,"ph_friction_ground");
	wf					= pSettings->r_float	(section,"ph_friction_wall"	);
	Movement.SetFriction	(af,wf,gf);

	// BOX activate
	Movement.ActivateBox	(0);
	*/
	////////

	Position().y			+= EPS_L;
	
	m_current			= 0;

	// Health & Armor
	fArmor				= 0;
	
	// Sheduler
	shedule.t_min			= 50;
	shedule.t_max			= 500; // 30 * NET_Latency / 4;

	// Msg				("! cmonster size: %d",sizeof(*this));
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

void CCustomMonster::SelectAnimation(const Fvector& _view, const Fvector& _move, float speed)
{
	R_ASSERT(fsimilar(_view.magnitude(),1));
	R_ASSERT(fsimilar(_move.magnitude(),1));

	CMotionDef*	S=0;

	if (fHealth<=0) {
		// Die
		S = m_death;
	} else {
		if (speed<0.2f) {
			// idle
			S = m_idle;
		} else {
			Fvector view = _view; view.y=0; view.normalize_safe();
			Fvector move = _move; move.y=0; move.normalize_safe();
			float	dot  = view.dotproduct(move);
			
			SAnimState* AState = &m_walk;
//			if (bCrouched)	AState = &m_crouch_walk;
//			else			
			AState = &m_walk;
			
			if (speed>2.f){
//				if (bCrouched)	AState = &m_crouch_run;
//				else			
				AState = &m_run;
			}
			
			if (dot>0.7f){
				S = AState->fwd;
			}else if ((dot<=0.7f)&&(dot>=-0.7f)){
				Fvector cross; cross.crossproduct(view,move);
				if (cross.y>0){
					S = AState->rs;
				}else{
					S = AState->ls;
				}
			}else //if (dot<-0.7f)
			{
				S = AState->back;
			}
		}
	}
	if (S!=m_current){ 
		m_current = S;
		if (S) PKinematics(Visual())->PlayCycle(S);
	}
}

void CCustomMonster::net_Export(NET_Packet& P)					// export to server
{
	R_ASSERT				(Local());

	// export last known packet
	R_ASSERT				(!NET.empty());
	net_update& N			= NET.back();
	P.w_float_q16		(fHealth,-1000,1000);
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
	P.r_float_q16		(fHealth,-1000,1000);
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

void CCustomMonster::Exec_Physics( float dt )
{
	// 
	Engine.Sheduler.Slice	();
}

void CCustomMonster::shedule_Update	( u32 DT )
{
	if (!Remote()) {
		if ((fHealth>0) || bfExecMovement())
			// функция должна выполняться до inherited::shedule_Update, для smooth movement
			Exec_Movement	(float(DT)/1000.f);  
	}

	// *** general stuff
	inherited::shedule_Update	(DT);

	// Queue shrink
	u32	dwTimeCL		= Level().timeServer()-NET_Latency;
	VERIFY				(!NET.empty());
	while ((NET.size()>2) && (NET[1].dwTimeStamp<dwTimeCL)) NET.pop_front();
	
	// Queue setup
	float dt			= float(DT)/1000.f;
	if (dt > 3)
		return;
	
	if (Remote())		{
	} else {
		// here is monster AI call
		m_fTimeUpdateDelta				= dt;
		Device.Statistic.AI_Think.Begin	();
		Think							();
		m_dwLastUpdateTime				= Level().timeServer();
		Device.Statistic.AI_Think.End	();

		Engine.Sheduler.Slice			();

		// Look and action streams
		if (fHealth>0) {
			Exec_Action				(dt);
			Exec_Look				(dt);
			Exec_Visibility			();
			//////////////////////////////////////
			Fvector C; float R;
			//////////////////////////////////////
			// С Олеся - ПИВО!!!! (Диме :-))))
			// Movement.GetBoundingSphere	(C,R);
			//////////////////////////////////////
			Center(C);
			R = Radius();
			//////////////////////////////////////
			feel_touch_update		(C,R);

			net_update				uNext;
			uNext.dwTimeStamp		= Level().timeServer();
			uNext.o_model			= r_torso_current.yaw;
			uNext.o_torso			= r_torso_current;
			uNext.p_pos				= Position();
			uNext.fHealth			= fHealth;
			NET.push_back			(uNext);
		}
		else 
		{
			Exec_Physics			(dt);
			if (bfExecMovement()) 
			{
				//Exec_Movement		(dt);
				net_update			uNext;
				uNext.dwTimeStamp	= Level().timeServer();
				uNext.o_model		= r_torso_current.yaw;
				uNext.o_torso		= r_torso_current;
				uNext.p_pos			= Position();
				uNext.fHealth		= fHealth;
				NET.push_back		(uNext);
			}
			else {
				net_update			uNext;
				uNext.dwTimeStamp	= Level().timeServer();
				uNext.o_model		= r_torso_current.yaw;
				uNext.o_torso		= r_torso_current;
				uNext.p_pos			= Position();
				uNext.fHealth		= fHealth;
				NET.push_back		(uNext);
			}
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
	fHealth			= A.fHealth*(1.f - f) + B.fHealth*f;
}

void CCustomMonster::UpdateCL	()
{ 
	inherited::UpdateCL();
	if	(NET.empty())	return;

	// distinguish interpolation/extrapolation
	u32	dwTime			= Level().timeServer()-NET_Latency;
	net_update&	N		= NET.back();
	if (NET.size()<2) {
		// BAD.	extrapolation
		Log("Extrapolation");
		NET_Last		= N;
	}
	else
		if (dwTime > N.dwTimeStamp) {
			int select		= -1;
			u32				dwMaxTime = 0;
			for (u32 id=0; id<NET.size()-1; id++)
			{
				if (NET[id].dwTimeStamp >= dwMaxTime)
					select=id;
			}
			if (select >= 0) {
				if (select == (int)NET.size()-1)
					select--;
				// Interpolate state
				net_update&	A			= NET[select+0];
				net_update&	B			= NET[select+1];
				u32	d1					= dwTime-A.dwTimeStamp;
				u32	d2					= B.dwTimeStamp - A.dwTimeStamp;
				float					f = (float(d1)/float(d2));
				
//				NET_Last.p_pos.sub		(B.p_pos,A.p_pos);
//				if (dwTime - B.dwTimeStamp < 150)
//					NET_Last.p_pos.mul		(f);
//				else
//					NET_Last.p_pos.mul		((float(d2 + 150)/float(d2)));
//				NET_Last.p_pos.add		(A.p_pos);
				NET_Last.lerp			(A,B,_min(f,2.f));
				
				Fvector					dir;
				AI_Path.Direction		(dir);
				SelectAnimation			(XFORM().k,dir,AI_Path.fSpeed);
				
				// Signal, that last time we used interpolation
				NET_WasExtrapolating	= TRUE;
				NET_Time				= dwTime;
			}
		}
		else {
			// OK.	interpolation
			NET_WasExtrapolating		= FALSE;
			// Search 2 keyframes for interpolation
			int select		= -1;
			for (u32 id=0; id<NET.size()-1; id++)
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
				NET_Last.lerp			(A,B,factor);
				
				Fvector					dir;
				AI_Path.Direction		(dir);
				SelectAnimation			(XFORM().k,dir,AI_Path.fSpeed);
				
				// Signal, that last time we used interpolation
				NET_WasInterpolating	= TRUE;
				NET_Time				= dwTime;
			}
		}

	// Use interpolated/last state
	// mTransformCL	= mTransform;
	XFORM().rotateY			(NET_Last.o_model);
	XFORM().translate_over	(NET_Last.p_pos);

	if (Remote())		{
		XFORM().rotateY			(N.o_model);
		XFORM().translate_over	(N.p_pos);
		Position().set				(NET_Last.p_pos);

	}
}

BOOL CCustomMonster::feel_visible_isRelevant (CObject* O)
{
	CEntityAlive* E = dynamic_cast<CEntityAlive*>		(O);
	if (0==E)								return FALSE;
	if (E->g_Team() == g_Team())			return FALSE;
	return TRUE;
}

void CCustomMonster::GetVisible			(objVisible& R)
{
	xr_vector<feel_visible_Item>::iterator I=feel_visible.begin(),E=feel_visible.end();
	for (; I!=E; I++)	if (positive(I->fuzzy)) {
		CEntityAlive *tpEntityAlive = dynamic_cast<CEntityAlive *>(I->O);
#ifdef IGNORE_ACTOR
		CActor		 *tpActor = dynamic_cast<CActor *>(I->O);
		if (tpEntityAlive && (tfGetRelationType(tpEntityAlive) != eRelationTypeFriend) && !tpActor)
#else
		if (tpEntityAlive && (tfGetRelationType(tpEntityAlive) != eRelationTypeFriend))// && !tpActor)
#endif
			R.insert(I->O);
	}
}

void CCustomMonster::eye_pp_s0			( )
{
	eye_pp_stage						++;

	// Eye matrix
	Device.Statistic.TEST0.Begin			();
	CKinematics* V							= PKinematics(Visual());
	V->Calculate							();
	Fmatrix&	mEye						= V->LL_GetTransform(eye_bone);
	Fmatrix		X;							X.mul_43	(XFORM(),mEye);
	eye_matrix.setHPB						(-r_current.yaw,-r_current.pitch,0);
//	eye_matrix.c.set						(X.c);
	eye_matrix.c.add						(X.c,m_tEyeShift);
	Device.Statistic.TEST0.End				();
}

void CCustomMonster::eye_pp_s1			( )
{
	eye_pp_stage						++;

	// Standart visibility
	Device.Statistic.AI_Vis_Query.Begin		();
	Fmatrix									mProject,mFull,mView;
	mView.build_camera_dir					(eye_matrix.c,eye_matrix.k,eye_matrix.j);
	mProject.build_projection				(deg2rad(eye_fov),1,0.1f,eye_range);
	mFull.mul								(mProject,mView);
	feel_vision_query						(mFull,eye_matrix.c);
	Device.Statistic.AI_Vis_Query.End		();
}

void CCustomMonster::eye_pp_s2			( )
{
	eye_pp_stage						++;

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

//	Movement.DBG_Render();

	RCache.OnFrameEnd				();
	{
	for (u32 I=1; I<AI_Path.TravelPath.size(); I++)
	{
		CTravelNode&	N1 = AI_Path.TravelPath[I-1];	Fvector	P1; P1.set(N1.P); P1.y+=0.1f;
		CTravelNode&	N2 = AI_Path.TravelPath[I];		Fvector	P2; P2.set(N2.P); P2.y+=0.1f;
		RCache.dbg_DrawLINE			(Fidentity,P1,P2,D3DCOLOR_XRGB(0,255,0));
		RCache.dbg_DrawAABB			(P1,.1f,.1f,.1f,D3DCOLOR_XRGB(0,0,255));
		
		/**/
		Fvector         T;
        Fvector4        S;
        
        T.set			(AI_Path.TravelPath[I].P); T.y+=(Radius()*2);
        Device.mFullTransform.transform (S,T);
        
//		pApp->pFont->Size       (0.07f/sqrtf(_abs(S.w)));
//		pApp->pFont->Color      (D3DCOLOR_RGBA(0,255,0,(S.z<=0)?0:255));
//		pApp->pFont->Out		(S.x,-S.y,"%d",I);
		/**/
	}
	}
	/*
	{
	for (u32 I=1; I<AI_Path.TravelPath_dbg.size(); I++)
	{
		CTrevelNode&	N1 = AI_Path.TravelPath_dbg[I-1];	Fvector	P1; P1.set(N1.P); P1.y+=0.1f;
		CTrevelNode&	N2 = AI_Path.TravelPath_dbg[I];		Fvector	P2; P2.set(N2.P); P2.y+=0.1f;
		RCache.dbg_DrawLINE(precalc_identity,P1,P2,D3DCOLOR_XRGB(255,0,0));
		RCache.dbg_DrawAABB(P1,.1f,.1f,.1f,D3DCOLOR_XRGB(255,0,0));
	}
	}
	*/
	{
	
	for (u32 I=0; I<AI_Path.Segments.size(); I++)
	{
		PSegment& S = AI_Path.Segments[I];
		Fvector P1;	P1.set(S.v1); P1.y+=0.1f;
		Fvector P2;	P2.set(S.v2); P2.y+=0.1f;
		RCache.dbg_DrawAABB(P1,.01f,.01f,.01f,D3DCOLOR_XRGB(255,255,255));
		RCache.dbg_DrawAABB(P2,.01f,.01f,.01f,D3DCOLOR_XRGB(255,255,255));
		RCache.dbg_DrawLINE(Fidentity,P1,P2,D3DCOLOR_XRGB(255,255,255));
	}
	}

	if (this == Level().Teams[g_Team()].Squads[g_Squad()].Leader) {
		CGroup &Group = Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()];
		for (unsigned i=0; i<Group.m_tpaSuspiciousNodes.size(); i++) {
			Fvector tP0 = getAI().tfGetNodeCenter(Group.m_tpaSuspiciousNodes[i].dwNodeID);
			tP0.y += .35f;
			if (Group.m_tpaSuspiciousNodes[i].dwSearched == 0)		
				RCache.dbg_DrawAABB(tP0,.35f,.35f,.35f,D3DCOLOR_XRGB(255,0,0));
			else
				if (Group.m_tpaSuspiciousNodes[i].dwSearched == 1)		
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
		Movement.dbg_Draw();
	}
//	if (bDebug) PKinematics(Visual())->DebugRender(XFORM());
}
#endif

void CCustomMonster::HitSignal(float perc, Fvector& vLocalDir, CObject* who)
{
}

void CCustomMonster::Die	()
{
}

BOOL CCustomMonster::net_Spawn	(LPVOID DC)
{
	if (!inherited::net_Spawn(DC))	return FALSE;

	CSE_Abstract			*e	= (CSE_Abstract*)(DC);
	CSE_ALifeMonsterAbstract			*E	= dynamic_cast<CSE_ALifeMonsterAbstract*>(e);

	AI_Path.DestNode		= AI_NodeID;

	eye_matrix.identity		();

	r_torso_current.yaw		= r_torso_target.yaw	= -E->o_Angle.y;
	r_torso_current.pitch	= r_torso_target.pitch	= 0;

	R_ASSERT				(Visual()->Type==MT_SKELETON);

	// Eyes
	eye_bone				= PKinematics(Visual())->LL_BoneID(pSettings->r_string(cNameSect(),"bone_head"));

	// weapons
	if (Local())	
	{
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
	} else {
	}

	return TRUE;
}

void CCustomMonster::OnHUDDraw(CCustomHUD* hud)
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

void CCustomMonster::feel_touch_new				(CObject* O)
{
	NET_Packet	P;

	// Test for weapon
	CWeapon* W	= dynamic_cast<CWeapon*>	(O);
	if (W)
	{
		// Search if we have similar type of weapon
		/*
		CWeapon* T = Weapons->getWeaponByWeapon	(W);
		if (T)	
		{
			// We have similar weapon - just get ammo out of it
			u_EventGen	(P,GE_TRANSFER_AMMO,ID());
			P.w_u16		(u16(W->ID()));
			P.w_u16		(u16(T->ID()));
			u_EventSend	(P);
			return;
		} else {
			// We doesn't have similar weapon - pick up it
			u_EventGen	(P,GE_OWNERSHIP_TAKE,ID());
			P.w_u16		(u16(W->ID()));
			u_EventSend	(P);
			return;
		}
		*/
		Log("~~~~~~~~~~~~~~~~~~~~~ EVENT");
		// We doesn't have similar weapon - pick up it
		u_EventGen	(P,GE_OWNERSHIP_TAKE,ID());
		P.w_u16		(u16(W->ID()));
		u_EventSend	(P);
		return;
	}

	// 
}

void CCustomMonster::Exec_Action(float dt)
{
}

// Развернуть объект в направление движения
void CCustomMonster::SetDirectionLook()
{
	int k = ps_Size();
	Msg("PositionStack: [Size = %i]", k);
	for (int j =0; j < k; j++) {
		Msg("Elem[%i] = %f,%f,%f]",j,VPUSH(ps_Element(k-j-1).vPosition));
	}


	int i = ps_Size();		// position stack size
	if (i > 1) {
		CObject::SavedPosition tPreviousPosition = ps_Element(i - 2), tCurrentPosition = ps_Element(i - 1);
		tWatchDirection.sub(tCurrentPosition.vPosition,tPreviousPosition.vPosition);
		if (tWatchDirection.magnitude() > EPS_L) {
			tWatchDirection.normalize();
			mk_rotation(tWatchDirection,r_torso_target);
		}
	}
	else
		r_torso_target.pitch = 0;
	r_target = r_torso_target;
	
}
