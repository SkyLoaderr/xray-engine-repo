#include "stdafx.h"
#include "artifact.h"

CArtifact::CArtifact(void)
{
}

CArtifact::~CArtifact(void)
{
}

#define CHOOSE_MAX(x,inst_x,y,inst_y,z,inst_z)\
	if(x>y)\
		if(x>z){inst_x;}\
		else{inst_z;}\
	else\
		if(y>z){inst_y;}\
		else{inst_z;}

BOOL CArtifact::net_Spawn(LPVOID DC) {
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
		CHOOSE_MAX(	obb.m_halfsize.x,ax.set(obb.m_rotate.i) ; ax.mul(obb.m_halfsize.x); radius=min(obb.m_halfsize.y,obb.m_halfsize.z) ;obb.m_halfsize.y/=2.f;obb.m_halfsize.z/=2.f,
					obb.m_halfsize.y,ax.set(obb.m_rotate.j) ; ax.mul(obb.m_halfsize.y); radius=min(obb.m_halfsize.x,obb.m_halfsize.z) ;obb.m_halfsize.x/=2.f;obb.m_halfsize.z/=2.f,
					obb.m_halfsize.z,ax.set(obb.m_rotate.k) ; ax.mul(obb.m_halfsize.z); radius=min(obb.m_halfsize.y,obb.m_halfsize.x) ;obb.m_halfsize.y/=2.f;obb.m_halfsize.x/=2.f
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
	} else if(H_Parent()) svTransform.set(H_Parent()->clXFORM());
}
