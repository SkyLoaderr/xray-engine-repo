#include "stdafx.h"
#include "ai_flesh.h"
#include "../../ai_space.h"

CAI_Flesh::CAI_Flesh()
{
	stateRest			= xr_new<CBitingRest>		(this);
	stateAttack			= xr_new<CBitingAttack>		(this, false);
	stateEat			= xr_new<CBitingEat>		(this, true);
	stateHide			= xr_new<CBitingHide>		(this);
	stateDetour			= xr_new<CBitingDetour>		(this);
	statePanic			= xr_new<CBitingPanic>		(this, false);
	stateExploreDNE		= xr_new<CBitingExploreDNE>	(this, false);
	stateExploreDE		= xr_new<CBitingExploreDE>	(this);
	stateExploreNDE		= xr_new<CBitingExploreNDE>	(this);
	CurrentState		= stateRest;

	stateTest			= xr_new<CBitingTest>		(this);

	Init();
}

CAI_Flesh::~CAI_Flesh()
{
	xr_delete(stateRest);
	xr_delete(stateAttack);
	xr_delete(stateEat);
	xr_delete(stateHide);
	xr_delete(stateDetour);
	xr_delete(statePanic);
	xr_delete(stateExploreDNE);
	xr_delete(stateExploreDE);
	xr_delete(stateExploreNDE);

	xr_delete(stateTest);
}

void CAI_Flesh::Init()
{
	inherited::Init();

	CurrentState					= stateRest;
	CurrentState->Reset				();
	m_fEyeShiftYaw					= PI_DIV_6;
}

BOOL CAI_Flesh::net_Spawn (LPVOID DC) 
{
	if (!inherited::net_Spawn(DC))
		return(FALSE);

	return TRUE;
}

void CAI_Flesh::Load(LPCSTR section)
{
	inherited::Load(section);

	MotionMan.AddReplacedAnim(&m_bDamaged, eAnimRun,		eAnimRunDamaged);
	MotionMan.AddReplacedAnim(&m_bDamaged, eAnimWalkFwd,	eAnimWalkDamaged);

	BEGIN_LOAD_SHARED_MOTION_DATA();

	// define animation set
	MotionMan.AddAnim(eAnimStandIdle,		"stand_idle_",			-1, 0,									0,											PS_STAND);
	MotionMan.AddAnim(eAnimStandTurnLeft,	"stand_turn_ls_",		-1, 0,									inherited::_sd->m_fsTurnNormalAngular,		PS_STAND);
	MotionMan.AddAnim(eAnimStandTurnRight,	"stand_turn_rs_",		-1, 0,									inherited::_sd->m_fsTurnNormalAngular,		PS_STAND);
	
	MotionMan.AddAnim(eAnimLieIdle,			"lie_idle_",			-1, 0,									0,											PS_LIE);
	MotionMan.AddAnim(eAnimSleep,			"lie_idle_",			-1, 0,									0,											PS_LIE);
	
	MotionMan.AddAnim(eAnimWalkFwd,			"stand_walk_fwd_",		-1, inherited::_sd->m_fsWalkFwdNormal,	inherited::_sd->m_fsWalkAngular,			PS_STAND);
	MotionMan.AddAnim(eAnimWalkDamaged,		"stand_walk_fwd_dmg_",	-1, inherited::_sd->m_fsWalkFwdDamaged,	inherited::_sd->m_fsWalkAngular,			PS_STAND);
	MotionMan.AddAnim(eAnimWalkBkwd,		"stand_walk_bkwd_",		-1, inherited::_sd->m_fsWalkBkwdNormal,	inherited::_sd->m_fsWalkAngular,			PS_STAND);
	
	MotionMan.AddAnim(eAnimRun,				"stand_run_",			-1,	inherited::_sd->m_fsRunFwdNormal,	inherited::_sd->m_fsRunAngular,				PS_STAND);
	MotionMan.AddAnim(eAnimRunDamaged,		"stand_run_dmg_",		-1,	inherited::_sd->m_fsRunFwdDamaged,	inherited::_sd->m_fsRunAngular,				PS_STAND);

	MotionMan.AddAnim(eAnimAttack,			"stand_attack_",		-1, 0,									inherited::_sd->m_fsRunAngular,				PS_STAND);
	MotionMan.AddAnim(eAnimAttackRat,		"stand_attack_rat_",	-1, 0,									inherited::_sd->m_fsRunAngular,				PS_STAND);
	MotionMan.AddAnim(eAnimAttackFromBack,	"stand_attack_back_",	-1, 0,									0,											PS_STAND);
	MotionMan.AddAnim(eAnimCheckCorpse,		"stand_eat_",			 1,	0,									0,											PS_STAND);

	MotionMan.AddAnim(eAnimEat,				"stand_eat_",			-1, 0,									0,											PS_STAND);
	MotionMan.AddAnim(eAnimDie,				"stand_idle_",			-1, 0,									0,											PS_STAND);
	
	MotionMan.AddAnim(eAnimStandLieDown,	"stand_lie_down_",		-1, 0,									0,											PS_STAND);
	MotionMan.AddAnim(eAnimLieStandUp,		"lie_stand_up_",		-1, 0,									0,											PS_LIE);

	MotionMan.AddAnim(eAnimSteal,			"stand_crawl_",			-1, inherited::_sd->m_fsSteal,			inherited::_sd->m_fsWalkAngular,			PS_STAND);
	MotionMan.AddAnim(eAnimDragCorpse,		"stand_drag_",			-1, inherited::_sd->m_fsDrag,			inherited::_sd->m_fsWalkAngular,			PS_STAND);

	MotionMan.AddAnim(eAnimScared,			"stand_scared_",		-1,	0,									0,											PS_STAND);
	MotionMan.AddAnim(eAnimThreaten,		"stand_threaten_",		-1,	0,									0,											PS_STAND);
	
	// define transitions
	MotionMan.AddTransition(PS_STAND,	PS_LIE,		eAnimStandLieDown,		false);
	MotionMan.AddTransition(PS_LIE,		PS_STAND,	eAnimLieStandUp,		false);

	// define links from Action to animations
	MotionMan.LinkAction(ACT_STAND_IDLE,	eAnimStandIdle,	eAnimStandTurnLeft, eAnimStandTurnRight, PI_DIV_6);
	MotionMan.LinkAction(ACT_SIT_IDLE,		eAnimLieIdle);
	MotionMan.LinkAction(ACT_LIE_IDLE,		eAnimLieIdle);
	MotionMan.LinkAction(ACT_WALK_FWD,		eAnimWalkFwd);
	MotionMan.LinkAction(ACT_WALK_BKWD,		eAnimWalkBkwd);
	MotionMan.LinkAction(ACT_RUN,			eAnimRun);
	MotionMan.LinkAction(ACT_EAT,			eAnimEat);
	MotionMan.LinkAction(ACT_SLEEP,			eAnimSleep);
	MotionMan.LinkAction(ACT_REST,			eAnimLieIdle);
	MotionMan.LinkAction(ACT_DRAG,			eAnimDragCorpse);
	MotionMan.LinkAction(ACT_ATTACK,		eAnimAttack, eAnimStandTurnLeft, eAnimStandTurnRight, PI_DIV_2);
	MotionMan.LinkAction(ACT_STEAL,			eAnimSteal);
	MotionMan.LinkAction(ACT_LOOK_AROUND,	eAnimScared);

	// Добавить анимации атак
	MotionMan.AA_PushAttackAnim(eAnimAttack,		0, 600,		800,	Fvector().set(0.f,0.f,0.f),	Fvector().set(0.f,0.f,3.5f),	inherited::_sd->m_fHitPower, Fvector().set(0.f,0.f,3.f));
	MotionMan.AA_PushAttackAnim(eAnimAttack,		1, 600,		800,	Fvector().set(0.f,0.f,0.f),	Fvector().set(0.f,0.f,3.5f),	inherited::_sd->m_fHitPower, Fvector().set(0.f,0.f,3.f));
	MotionMan.AA_PushAttackAnim(eAnimAttackFromBack,0, 400,		600,	Fvector().set(0.f,0.f,0.f),	Fvector().set(0.f,0.f,-3.5f),	inherited::_sd->m_fHitPower, Fvector().set(0.f,0.f,-3.f));
	MotionMan.AA_PushAttackAnim(eAnimAttackRat,		0, 600,		800,	Fvector().set(0.f,0.f,0.f),	Fvector().set(0.f,0.f,3.5f),	inherited::_sd->m_fHitPower, Fvector().set(0.f,0.f,1.f), AA_FLAG_ATTACK_RAT);

	END_LOAD_SHARED_MOTION_DATA();
}


void CAI_Flesh::StateSelector()
{
	
	dbg_info.node_id = dynamic_cast<CEntity *>(Level().CurrentEntity())->level_vertex_id();
	
	VisionElem ve;

	if (C && H && I)			SetState(statePanic);
	else if (C && H && !I)		SetState(statePanic);
	else if (C && !H && I)		SetState(statePanic);
	else if (C && !H && !I) 	SetState(statePanic);
	else if (D && H && I)		SetState(stateAttack);
	else if (D && H && !I)		SetState(stateAttack);		//тихо подобраться и начать аттаку
	else if (D && !H && I)		SetState(statePanic);
	//else if (D && !H && !I) 	SetState(stateHide);		// отход перебежками через укрытия
	else if (D && !H && !I) 	SetState(statePanic);
	else if (E && H && I)		SetState(stateAttack); 
	else if (E && H && !I)  	SetState(stateAttack);		//тихо подобраться и начать аттаку
//	else if (E && !H && I) 		SetState(stateDetour); 
//	else if (E && !H && !I)		SetState(stateDetour); 
	else if (E && !H && I) 		SetState(stateAttack); 
	else if (E && !H && !I)		SetState(stateAttack); 
	else if (F && H && I) 		SetState(stateAttack); 		
	else if (F && H && !I)  	SetState(stateAttack); 
	else if (F && !H && I)  	SetState(stateAttack); 
	else if (F && !H && !I) 	SetState(stateAttack);
//	else if (F && !H && I)  	SetState(stateDetour); 
//	else if (F && !H && !I) 	SetState(stateHide);
	else if (A && !K && !H)		SetState(stateExploreNDE);  // SetState(stateExploreDNE);  // слышу опасный звук, но не вижу, враг не выгодный		(ExploreDNE)
	else if (A && !K && H)		SetState(stateExploreNDE);  // SetState(stateExploreDNE);	//SetState(stateExploreDE);	// слышу опасный звук, но не вижу, враг выгодный			(ExploreDE)		
	else if (B && !K && !H)		SetState(stateExploreNDE);	// слышу не опасный звук, но не вижу, враг не выгодный	(ExploreNDNE)
	else if (B && !K && H)		SetState(stateExploreNDE);	// слышу не опасный звук, но не вижу, враг выгодный		(ExploreNDE)
	else if (GetCorpse(ve) && (ve.obj->m_fFood > 1) && ((GetSatiety() < 0.85f) || flagEatNow))	
		SetState(stateEat);
	else						SetState(stateRest); 


	HDebug->Clear();
	HDebug->SetActive(true);
	HDebug->Add(0,cName(),D3DCOLOR_XRGB(255,0,128));
}

bool CAI_Flesh::AA_CheckHit()
{
	// Проверка состояния анимации (атака)
	SAttackAnimation apt_anim;

	bool was_hit = false;	

	if (MotionMan.AA_CheckTime(m_dwCurrentTime,apt_anim)) {
		SetSoundOnce(SND_TYPE_ATTACK_HIT, m_dwCurrentTime);

		VisionElem ve;
		if (!GetEnemy(ve)) return false;
		CObject *obj = dynamic_cast<CObject *>(ve.obj);
	
		// перевод из локальных координат в мировые
		Fvector trace_from;
		XFORM().transform_tiny(trace_from, apt_anim.trace_from);
		
		Fvector trace_to;
		XFORM().transform_tiny(trace_to, apt_anim.trace_to);

		// вычисление вектора направления проверки хита
		Fvector dir;
		dir.sub(trace_to, trace_from);
		float dist = dir.magnitude();
		dir.normalize();
		
		// делаем поправку на pitch (fix it)
		Fvector new_dir;
		Fvector C1, C2;
		Center(C1);
		obj->Center(C2);
		new_dir.sub(C2,C1);
		new_dir.normalize();
		dir.y = new_dir.y;
		dir.normalize();
		
		// перевод из локальных координат в мировые вектора направления импульса
		Fvector hit_dir;
		XFORM().transform_dir(hit_dir,apt_anim.hit_dir);
		hit_dir.normalize();

		// трассировка нужна?
		if ((apt_anim.flags & AA_FLAG_FIRE_ANYWAY) == AA_FLAG_FIRE_ANYWAY) {
			HitEntity(ve.obj, apt_anim.damage,hit_dir);	// не нужна
			was_hit = true;
		} else if ((apt_anim.flags & AA_FLAG_ATTACK_RAT) == AA_FLAG_ATTACK_RAT) {
			
			// TestIntersection конуса(копыта) и сферы(крысы)
			Fvector dir;	dir.set(0.f,-1.f,0.f);		// направление конуса
			Fvector vC;		ve.obj->Center(vC);			// центр сферы

			if (ConeSphereIntersection(trace_from, PI_DIV_6, dir, vC, ve.obj->Radius())) {
				HitEntity(ve.obj,apt_anim.damage,dir);
				was_hit = true;
			}

		} else 	{ // нужна
			if (RayPickEnemy(obj, trace_from, dir, dist, 0.2f, 20)) {
				HitEntity(ve.obj, apt_anim.damage, hit_dir);
				was_hit = true;
			}
		}

		if (!ve.obj->g_Alive()) AddCorpse(ve);	
		
		if (AS_Active()) AS_Check(was_hit);
		MotionMan.AA_UpdateLastAttack(m_dwCurrentTime);
	}
	return was_hit;
}


// возвращает true, если после выполнения этой функции необходимо прервать обработку
// т.е. если активирована последовательность
void CAI_Flesh::CheckSpecParams(u32 spec_params)
{
	if ((spec_params & ASP_DRAG_CORPSE) == 	ASP_DRAG_CORPSE) MotionMan.SetCurAnim(eAnimDragCorpse);

	if ((spec_params & ASP_CHECK_CORPSE) == ASP_CHECK_CORPSE) {
		MotionMan.Seq_Add(eAnimCheckCorpse);
		MotionMan.Seq_Switch();
	}

	if ((spec_params & ASP_BACK_ATTACK) == ASP_BACK_ATTACK) {
		MotionMan.Seq_Add(eAnimAttackFromBack);
		MotionMan.Seq_Switch();
	}

	if ((spec_params & ASP_ATTACK_RAT) == ASP_ATTACK_RAT) MotionMan.SetCurAnim(eAnimAttackRat);
}


////////////////////////////////////////////////////////////////////////////////////////////////
// Функция ConeSphereIntersection
// Пересечение конуса (не ограниченного) со сферой
// Необходима для определения пересечения копыта плоти с баунд-сферой крысы
// Параметры: ConeVertex - вершина конуса, ConeAngle - угол конуса (между поверхностью и высотой)
// ConeDir - направление конуса, SphereCenter - центр сферы, SphereRadius - радиус сферы
bool CAI_Flesh::ConeSphereIntersection(Fvector ConeVertex, float ConeAngle, Fvector ConeDir, Fvector SphereCenter, float SphereRadius)
{
	float fInvSin = 1.0f/_sin(ConeAngle);
	float fCosSqr = _cos(ConeAngle)*_cos(ConeAngle);

	
	Fvector kCmV;	kCmV.sub(SphereCenter,ConeVertex);
	Fvector kD		= kCmV;
	Fvector tempV	= ConeDir;
	tempV.mul		(SphereRadius* fInvSin);
	kD.add			(tempV);

	float fDSqrLen = kD.square_magnitude();
	float fE = kD.dotproduct(ConeDir);
	if ( fE > 0.0f && fE*fE >= fDSqrLen*fCosSqr ) {
		
		float fSinSqr = _sin(ConeAngle)*_sin(ConeAngle);

		fDSqrLen = kCmV.square_magnitude();
		fE = -kCmV.dotproduct(ConeDir);
		if ( fE > 0.0f && fE*fE >= fDSqrLen*fSinSqr ) {
			float fRSqr = SphereRadius*SphereRadius;
			return fDSqrLen <= fRSqr;
		} else return true;
	} 
	
	return false;
}

void CAI_Flesh::OnRender()
{
	inherited::OnRender();
	

//	if (dbg_info.node_vec.empty()) return;
//	
//	for (u32 i=0;i<dbg_info.node_vec.size()-1;i++) {
//		Fvector pos = ai().level_graph().vertex_position(ai().game_graph().vertex(dbg_info.node_vec[i])->level_vertex_id());
//		RCache.dbg_DrawAABB(pos,0.35f,0.35f,0.35f,D3DCOLOR_XRGB(255,0,128));
//	}
//
//	if (!dbg_info.node_vec.empty()) {
//		Fvector pos = ai().level_graph().vertex_position(ai().game_graph().vertex(dbg_info.node_vec[dbg_info.node_vec.size()-1])->level_vertex_id());
//		RCache.dbg_DrawAABB(pos,0.35f,0.35f,0.35f,D3DCOLOR_XRGB(255,128,128));
//	}
//


//	CLevelGraph::SContour con;
//	ai().level_graph().contour(con, dbg_info.node_id);
//	
//	RCache.dbg_DrawLINE(Fidentity,con.v1,con.v2,D3DCOLOR_XRGB(255,0,128));
//	RCache.dbg_DrawLINE(Fidentity,con.v2,con.v3,D3DCOLOR_XRGB(255,0,128));
//	RCache.dbg_DrawLINE(Fidentity,con.v3,con.v4,D3DCOLOR_XRGB(255,0,128));
//	RCache.dbg_DrawLINE(Fidentity,con.v4,con.v1,D3DCOLOR_XRGB(255,0,128));
//	
//
//	Fvector v1;
//	v1 = ai().level_graph().vertex_position(dbg_info.node_id);
//	Fvector v2;
//	
//	if (!Level().CurrentEntity()) return;
//	v2.mad(v1,Level().CurrentEntity()->Direction(),3.f);
//
//	RCache.dbg_DrawLINE(Fidentity,v1,v2,D3DCOLOR_XRGB(255,0,128));
//
//	// v1,v2 - cur_dir
//	
//	// build normal
//	Fvector vec1,vec2;
//	Fvector norm;
//	
//	vec1.sub(con.v1, con.v2);
//	vec2.sub(con.v2, con.v3);
//	norm.crossproduct(vec1,vec2);
//	norm.invert();
//	norm.normalize();
//
//	Fvector norm_end;
//	norm_end.mad(v1,norm,3.0f);
//
//	// show normal
//	RCache.dbg_DrawLINE(Fidentity,v1,norm_end,D3DCOLOR_XRGB(255,128,128));



	// JIM TEMP

	//	if (!pM->dbg_info.active) return;
	//	RCache.dbg_DrawAABB(pM->dbg_info.pos,0.15f,0.15f,0.15f,D3DCOLOR_XRGB(255,0,128));
	//
	//	Fvector up_vect;
	//	up_vect = pM->dbg_info.pos;
	//	up_vect.y += 1.5f;
	//	RCache.dbg_DrawLINE(Fidentity,pM->dbg_info.pos,up_vect,D3DCOLOR_XRGB(255,0,128));

	//
	//
	//	CAI_Biting::s_dbg::TNODES_MAP_IT I, B = pM->dbg_info.nodes.begin();
	//	CAI_Biting::s_dbg::TNODES_MAP_IT E = pM->dbg_info.nodes.end();
	//	
	//	for (I = B; I!=E; I++) {
	//		u32		node_id = I->first;
	//		TTime	d_time	= TTime((Level().timeServer() - I->second) / 500);
	//		u8 col;
	//
	//		Fvector pos = ai().level_graph().vertex_position(node_id);
	//		
	//		if (d_time > 0xff) col = 0xff;
	//		else col = 0xff - u8(d_time);
	//
	//		RCache.dbg_DrawAABB(pos,0.25f,0.25f,0.25f,D3DCOLOR_XRGB(128,col,128));
	//	}

	//	for (u32 i=0; i<pM->dbg_info.node_vec.size();i++) {
	//		Fvector pos = ai().level_graph().vertex_position(pM->dbg_info.node_vec[i]);
	//		u8 col;
	//		
	//		
	//		col = u8(float(ai().level_graph().vertex_cover(pM->dbg_info.node_vec[i]) / 4.f * 255.f));
	//
	////		LOG_EX2("NODE %u: COVER = [%f] COL = %u", *"*/  i, ai().level_graph().vertex_cover(pM->dbg_info.node_vec[i]),col /*"*);
	//
	//		for (float f = 0.25f; f > 0.2f; f-=0.01)
	//			RCache.dbg_DrawAABB(pos,f,f,f,D3DCOLOR_XRGB(0x0,0x0,col));
	//	}


	// Test Tracer


	// Show_dir

	//	// source data
	//	Fvector dir;
	//	dir = Direction();
	//	float dist = 20.f;
	//	float R = 10.f;
	//
	//	float max_alpha = atanf(R/dist);
	//	
	//	Fvector from_pos;
	//	Center(from_pos);
	//
	//	u32 num = 100;
	//	//////////////////////////////////////////////////////////////////////////
	//	
	//	// show firts elem
	//	
	//	// get_pos
	//	Fvector pos;
	//
	//	pos.mad(from_pos,dir,dist);
	//	RCache.dbg_DrawAABB(pos,0.1f,0.1f,0.1f,D3DCOLOR_XRGB(0xff,0x0,0x0));	
	//
	//
	//	Fvector res;
	//	float src_h, src_p;	
	//	dir.getHP(src_h,src_p);
	//
	//	Fvector newV;
	//	float new_h,new_p;
	//
	//
	//	// show other N elems
	//	for (u32 i=0;i<num;i++) {
	//		
	//		
	//
	//		new_h = src_h + ::Random.randF(-max_alpha, max_alpha);
	//		new_p = src_p + ::Random.randF(-max_alpha, max_alpha);		
	//
	//		newV = dir;
	//		newV.setHP(new_h,new_p);
	//		newV.normalize();
	//
	//		res.mad(from_pos,newV,dist);
	//		// find new dir
	//
	////		u8 col = u8 (255.f * ( sqrt((new_h-cur_h) * (new_h-cur_h) + (new_p - cur_p) * (new_p - cur_p)) / (max_alpha)));
	//
	//		RCache.dbg_DrawAABB(res,0.1f,0.1f,0.1f,D3DCOLOR_XRGB(0x0,0x0,0xff));	
	//	}
	//



	//	for (uint i = 0; i<pM->dbg_info.vec.size(); i++) {
	//		RCache.dbg_DrawAABB(pM->dbg_info.vec[i],0.01f,0.01f,0.01f,D3DCOLOR_XRGB(0x0,0x0,0xff));
	//	}
	//
	//	RCache.dbg_DrawLINE(Fidentity,pM->dbg_info.dir_from,pM->dbg_info.dir_to,D3DCOLOR_XRGB(255,0,128));
}


