#include "stdafx.h"
#include "artifact.h"
//#include "actor.h"

xr_set<CArtifact*> CArtifact::m_all;

CArtifact::CArtifact(void) {
	shedule.t_min = 20;
	shedule.t_max = 50;
	m_jumpHeight = 0;
}

CArtifact::~CArtifact(void) {
	SoundDestroy(m_detectorSound);
}

#define CHOOSE_MAX(x,inst_x,y,inst_y,z,inst_z)\
	if(x>y)\
		if(x>z){inst_x;}\
		else{inst_z;}\
	else\
		if(y>z){inst_y;}\
		else{inst_z;}

void CArtifact::Load(LPCSTR section) {
	// verify class
	LPCSTR Class = pSettings->r_string(section,"class");
	CLASS_ID load_cls = TEXT2CLSID(Class);
	R_ASSERT(load_cls==SUB_CLS_ID);

	inherited::Load(section);

	m_detectorDist = pSettings->r_float(section,"detector_dist");
	LPCSTR m_detectorSoundName = pSettings->r_string(section,"detector_sound");
	SoundCreate(m_detectorSound, m_detectorSoundName);

	if(pSettings->line_exist(section, "jump_height")) m_jumpHeight = pSettings->r_float(section,"jump_height");
}

BOOL CArtifact::net_Spawn(LPVOID DC) {
	m_all.insert(this);
	inherited::net_Spawn(DC);
	setVisible					(true);
	setEnabled					(true);
	//xrSE_MercuryBall* E			= (xrSE_MercuryBall*)DC;
	//cNameVisual_set				(E->s_Model);

	CKinematics* V				= PKinematics(Visual());
	if(V) V->PlayCycle			("idle");

	if (0==m_pPhysicsShell)
	{
		// Physics (Box)
		Fobb								obb;
		Visual()->vis.box.get_CD			(obb.m_translate,obb.m_halfsize);
		obb.m_rotate.identity				();

		// Physics (Elements)
		CPhysicsElement* E					= P_create_Element	();
		R_ASSERT							(E);
		
		Fvector ax;
		float	radius;
		CHOOSE_MAX(	obb.m_halfsize.x,ax.set(obb.m_rotate.i) ; ax.mul(obb.m_halfsize.x); radius=_min(obb.m_halfsize.y,obb.m_halfsize.z) ;obb.m_halfsize.y/=2.f;obb.m_halfsize.z/=2.f,
					obb.m_halfsize.y,ax.set(obb.m_rotate.j) ; ax.mul(obb.m_halfsize.y); radius=_min(obb.m_halfsize.x,obb.m_halfsize.z) ;obb.m_halfsize.x/=2.f;obb.m_halfsize.z/=2.f,
					obb.m_halfsize.z,ax.set(obb.m_rotate.k) ; ax.mul(obb.m_halfsize.z); radius=_min(obb.m_halfsize.y,obb.m_halfsize.x) ;obb.m_halfsize.y/=2.f;obb.m_halfsize.x/=2.f
					)
		//radius*=1.4142f;
		Fsphere sphere1,sphere2;
		sphere1.P.add						(obb.m_translate,ax);
		sphere1.R							=radius*1.4142f;

		sphere2.P.sub						(obb.m_translate,ax);
		sphere2.R							=radius/2.f;

		E->add_Box							(obb);
		E->add_Sphere						(sphere1);
		E->add_Sphere						(sphere2);
	
		// Physics (Shell)
		m_pPhysicsShell						= P_create_Shell	();
		R_ASSERT							(m_pPhysicsShell);
		m_pPhysicsShell->add_Element		(E);
		m_pPhysicsShell->setDensity			(2000.f);
		m_pPhysicsShell->Activate			(svXFORM(),0,svXFORM(),true);
		m_pPhysicsShell->mDesired.identity	();
		m_pPhysicsShell->fDesiredStrength	= 0.f;
		m_pPhysicsShell->SetAirResistance();
	}
	return TRUE;
}

void CArtifact::net_Destroy() {
	if(m_pPhysicsShell) m_pPhysicsShell->Deactivate();
	xr_delete(m_pPhysicsShell);
	m_all.erase(this);
	inherited::net_Destroy();
}

void CArtifact::OnH_A_Chield() {
	inherited::OnH_A_Chield		();
	setVisible					(false);
	setEnabled					(false);
	if(m_pPhysicsShell) m_pPhysicsShell->Deactivate();
}

void CArtifact::OnH_B_Independent() {
	inherited::OnH_B_Independent();
	setVisible					(true);
	setEnabled					(true);
	CObject*	E		= dynamic_cast<CObject*>(H_Parent());
	R_ASSERT		(E);
	svTransform.set(E->clXFORM());
	vPosition.set(svTransform.c);
	if(m_pPhysicsShell) {
		Fmatrix trans;
		Level().Cameras.unaffected_Matrix(trans);
		Fvector l_fw; l_fw.set(trans.k);// l_fw.mul(2.f);
		Fvector l_up; l_up.set(svTransform.j); l_up.mul(2.f);
		Fmatrix l_p1, l_p2;
		l_p1.set(svTransform); l_p1.c.add(l_up); l_up.mul(1.2f); //l_p1.c.add(l_fw);
		l_p2.set(svTransform); l_p2.c.add(l_up); /*l_fw.mul(1.f+m_force);*/ l_p2.c.add(l_fw);
		//Log("aaa",l_p1.c);
		//Log("bbb",l_p2.c);
		//m_pPhysicsShell->Activate(l_p1, 0, l_p2);
		Fvector l_vel,a_vel;
		float fi,teta,r;
		l_vel.add(l_up,l_fw);
		fi=  ::Random.randF(0.f,2.f*M_PI);
		teta=::Random.randF(0.f,M_PI);
		r=	 ::Random.randF(2.f*M_PI,3.f*M_PI);
		float rxy=r*_sin(teta);
		a_vel.set(rxy*_cos(fi),rxy*_sin(fi),r*_cos(teta));
		//a_vel.set(::Random.randF(ri*2.f*M_PI,ri*3.f*M_PI),::Random.randF(ri*2.f*M_PI,ri*3.f*M_PI),::Random.randF(ri*2.f*M_PI,ri*3.f*M_PI));

		m_pPhysicsShell->Activate(l_p1, l_vel, a_vel);
		svTransform.set(l_p1);
		vPosition.set(svTransform.c);
	}
}

void CArtifact::UpdateCL() {
	inherited::UpdateCL();
	if(getVisible() && m_pPhysicsShell) {
		m_pPhysicsShell->Update	();
		svTransform.set			(m_pPhysicsShell->mXFORM);
		vPosition.set(m_pPhysicsShell->mXFORM.c);
		if(m_jumpHeight) {
			Fvector l_dir; l_dir.set(0, -1.f, 0);
			Collide::ray_query RQ;
			setEnabled(false);
			if(Level().ObjectSpace.RayPick(vPosition, l_dir, m_jumpHeight, RQ)) {
				l_dir.y = 1.f; m_pPhysicsShell->applyImpulse(l_dir, 30.f * Device.fTimeDelta * m_pPhysicsShell->getMass());
			}
			setEnabled(true);
		}
	} else if(H_Parent()) svTransform.set(H_Parent()->clXFORM());
}

void CArtifact::Update(u32 dt) {
	inherited::Update(dt);
	if(getVisible() && m_pPhysicsShell) {
		//float l_force = 100.f;
		//Fvector l_dir; l_dir.set(0, 1.f, 0);
		//if(vPosition.y < 1.f) {
		//	m_pPhysicsShell->applyImpulse(l_dir, .05f * dt * m_pPhysicsShell->getMass());
		//}
	}
}

void CArtifact::OnVisible() {
	//if(m_pHUD && H_Parent() && dynamic_cast<CActor*>(H_Parent())) {
	//	Fmatrix trans;
	//	Level().Cameras.affected_Matrix(trans);
	//	m_pHUD->UpdatePosition(trans);

	//	PKinematics(m_pHUD->Visual())->Update();
	//	if(m_showHUD) {
	//		::Render->set_Transform		(&m_pHUD->Transform());
	//		::Render->add_Visual		(m_pHUD->Visual());
	//	} else {
	//	}
	//}
	if(getVisible() && !H_Parent()) {
		::Render->set_Transform		(&clTransform);
		::Render->add_Visual		(Visual());
	}
}

void CArtifact::SoundCreate(sound& dest, LPCSTR s_name, int iType, BOOL bCtrlFreq) {
	string256 temp;
	if (FS.exist(temp,"$game_sounds$",s_name)) {
		Sound->create(dest,TRUE,s_name,iType);
		return;
	}
	Debug.fatal	("Can't find sound '%s'",s_name,cName());
}

void CArtifact::SoundDestroy(sound& dest) {
	::Sound->destroy			(dest);
}
