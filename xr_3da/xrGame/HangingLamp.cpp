#include "stdafx.h"
#pragma hdrstop

#include "HangingLamp.h"
#include "..\LightAnimLibrary.h"
#include "PhysicsShell.h"
 
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHangingLamp::CHangingLamp	()
{
	fHealth					= 100.f;
	light_bone_idx			= -1;
	lanim					= 0;
	light_render			= ::Render->light_create();
	light_render->set_type	(IRender_Light::SPOT);
	light_render->set_shadow(true);
}

CHangingLamp::~CHangingLamp	()
{
	::Render->light_destroy	(light_render);
}

void CHangingLamp::Load		(LPCSTR section)
{
	inherited::Load			(section);
}

BOOL CHangingLamp::net_Spawn(LPVOID DC)
{
	CSE_Abstract	*e	= (CSE_Abstract*)(DC);
	CSE_ALifeObjectHangingLamp	*lamp	= dynamic_cast<CSE_ALifeObjectHangingLamp*>(e);
	R_ASSERT				(lamp);
	cNameVisual_set			(lamp->get_visual());
	inherited::net_Spawn	(DC);
	Fcolor					clr;

	// set bone id
	light_bone_idx			= lamp->spot_bone[0]?PKinematics(Visual())->LL_BoneID(lamp->spot_bone):-1;
	clr.set					(lamp->color); clr.a = 1.f;
	clr.mul_rgb				(lamp->spot_brightness);
	fBrightness				= lamp->spot_brightness;
	light_render->set_range	(lamp->spot_range);
	light_render->set_color	(clr);
	light_render->set_cone	(lamp->spot_cone_angle);
	light_render->set_texture(lamp->spot_texture[0]?lamp->spot_texture:0);
	light_render->set_active(true);

	R_ASSERT				(Visual()&&PKinematics(Visual()));

	lanim					= LALib.FindItem(lamp->color_animator);

	if (lamp->flags.is(CSE_ALifeObjectHangingLamp::flPhysic))		CreateBody(lamp->mass);
	PKinematics(Visual())->PlayCycle("idle");
	//PKinematics(Visual())->Calculate();
	setVisible(true);
	setEnabled(true);

	return TRUE;
}

void CHangingLamp::shedule_Update	(u32 dt)
{
	inherited::shedule_Update		(dt);
}

void CHangingLamp::UpdateCL	()
{
	inherited::UpdateCL		();
	if(m_pPhysicsShell)
		renderable.xform.set	(m_pPhysicsShell->mXFORM);

	if (Alive()&&light_render->get_active()){
		Fmatrix xf;
		if (light_bone_idx>=0){
			Fmatrix& M = PKinematics(Visual())->LL_GetTransform(light_bone_idx);
			xf.mul		(XFORM(),M);
		} else {
			xf.set		(XFORM());
		}

		light_render->set_direction	(xf.k);
		light_render->set_position	(xf.c);
		if (lanim){
			int frame;
			u32 clr			= lanim->Calculate(Device.fTimeGlobal,frame); // возвращает в формате BGR
			Fcolor			fclr;
			fclr.set		((float)color_get_B(clr),(float)color_get_G(clr),(float)color_get_R(clr),1.f);
			fclr.mul_rgb	(fBrightness/255.f);
			light_render->set_color(fclr);
		}
		if (0){
			u32 clr			= 0xffffffff;
			Fcolor			fclr;
			fclr.set		((float)color_get_B(clr),(float)color_get_G(clr),(float)color_get_R(clr),1.f);
			fclr.mul_rgb	(fBrightness/255.f);
			light_render->set_color(fclr);
		}
	}
}

void CHangingLamp::renderable_Render()
{
	inherited::renderable_Render	();
}

void CHangingLamp::Hit(float P, Fvector &dir,	CObject* who, s16 element,Fvector p_in_object_space, float impulse)
{
	//inherited::Hit(P,dir,who,element,p_in_object_space,impulse);
	if(m_pPhysicsShell) m_pPhysicsShell->applyImpulseTrace(p_in_object_space,dir,impulse,element);
//	if (element==light_bone_idx)	fHealth = 0.f;
//	else							
		fHealth -= P*0.1f;
	if (!Alive())
	 	light_render->set_active(false);
}

void CHangingLamp::AddElement(CPhysicsElement* root_e, int id)
{
	CKinematics* K		= PKinematics(Visual());

	CPhysicsElement* E	= P_create_Element();
	CBoneInstance& B	= K->LL_GetInstance(id);
	E->mXFORM.set		(K->LL_GetTransform(id));
	Fobb bb			= K->LL_GetBox(id);


	if(bb.m_halfsize.magnitude()<0.05f)
	{
		bb.m_halfsize.add(0.05f);
	
	}
	E->add_Box			(bb);
	E->setMass(10.f);
	E->set_ParentElement(root_e);
	B.set_callback		(m_pPhysicsShell->GetBonesCallback(),E);
	m_pPhysicsShell->add_Element	(E);

	{
		CPhysicsJoint* J= P_create_Joint(CPhysicsJoint::full_control,root_e,E);
		J->SetAnchorVsSecondElement	(0,0,0);
		J->SetAxisDirVsSecondElement	(1,0,0,0);
		J->SetAxisDirVsSecondElement	(0,1,0,2);
		J->SetLimits				(-M_PI/2,M_PI/2,0);
		J->SetLimits				(-M_PI/2,M_PI/2,1);
		J->SetLimits				(-M_PI/2,M_PI/2,2);
		m_pPhysicsShell->add_Joint	(J);
	}

	CBoneData& BD		= K->LL_GetData(id);
	for (vecBonesIt it=BD.children.begin(); it!=BD.children.end(); it++){
		AddElement		(E,(*it)->SelfID);
	}
}

void CHangingLamp::CreateBody(float mass)
{
	m_pPhysicsShell		= P_create_Shell();
	m_pPhysicsShell->set_Kinematics(PKinematics(Visual()));
	AddElement			(0,PKinematics(Visual())->LL_BoneRoot());
	m_pPhysicsShell->mXFORM.set(renderable.xform);
	m_pPhysicsShell->SetAirResistance(0.001f, 0.02f);
	m_pPhysicsShell->setMass(mass);
	m_pPhysicsShell->SmoothElementsInertia(0.2f);


}

void CHangingLamp::net_Export(NET_Packet& P)
{
	R_ASSERT						(Local());
}

void CHangingLamp::net_Import(NET_Packet& P)
{
	R_ASSERT						(Remote());
}
