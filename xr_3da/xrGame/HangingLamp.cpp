#include "stdafx.h"
#pragma hdrstop

#include "HangingLamp.h"
#include "../LightAnimLibrary.h"
#include "PhysicsShell.h"
#include "Physics.h"
#include "xrserver_objects_alife.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHangingLamp::CHangingLamp	()
{
	fHealth					= 100.f;
	guid_bone				= BI_NONE;
	lanim					= 0;
	light_render			= ::Render->light_create();
	light_render->set_type	(IRender_Light::SPOT);
	light_render->set_shadow(true);

	glow_render				= ::Render->glow_create();
}

CHangingLamp::~CHangingLamp	()
{
	::Render->light_destroy	(light_render);
	::Render->glow_destroy	(glow_render);
}

void CHangingLamp::Load		(LPCSTR section)
{
	inherited::Load			(section);
}

BOOL CHangingLamp::net_Spawn(LPVOID DC)
{
	CSE_Abstract			*e		= (CSE_Abstract*)(DC);
	CSE_ALifeObjectHangingLamp	*lamp	= dynamic_cast<CSE_ALifeObjectHangingLamp*>(e);
	R_ASSERT				(lamp);
	inherited::net_Spawn	(DC);
	Fcolor					clr;

	// set bone id
	R_ASSERT				(Visual()&&PKinematics(Visual()));
	CKinematics* K			= PKinematics(Visual());
	CInifile* pUserData		= K->LL_UserData(); 
	R_ASSERT3				(pUserData,"Empty HangingLamp user data!",lamp->get_visual());

	guid_bone				= K->LL_BoneID	(pUserData->r_string("hanging_lamp_definition","guide_bone"));	VERIFY(guid_bone!=BI_NONE);
	clr.set					(lamp->color);						clr.a = 1.f;
	clr.mul_rgb				(lamp->brightness);
	fBrightness				= lamp->brightness;
	light_render->set_range	(lamp->spot_range);
	light_render->set_color	(clr);
	light_render->set_cone	(deg2rad(pUserData->r_float			("hanging_lamp_definition","spot_angle")));
	light_render->set_texture(pUserData->r_string				("hanging_lamp_definition","spot_texture"));

	glow_render->set_texture(pUserData->r_string				("hanging_lamp_definition","glow_texture"));
	glow_render->set_color	(clr);
	glow_render->set_radius	(pUserData->r_float					("hanging_lamp_definition","glow_radius"));

	fHealth					= lamp->m_health;

	lanim					= LALib.FindItem(*lamp->color_animator);

	if (lamp->flags.is(CSE_ALifeObjectHangingLamp::flPhysic))		CreateBody(lamp);
	if(PSkeletonAnimated(Visual()))	PSkeletonAnimated(Visual())->PlayCycle("idle");
	
	PKinematics(Visual())->Calculate();
	
	if (Alive())			TurnOn	();
	else					TurnOff	();
	
	setVisible				(TRUE);
	setEnabled				(TRUE);

	return					(TRUE);
}

void CHangingLamp::shedule_Update	(u32 dt)
{
	inherited::shedule_Update		(dt);
}

void CHangingLamp::UpdateCL	()
{
	inherited::UpdateCL		();

	if(m_pPhysicsShell)
		m_pPhysicsShell->InterpolateGlobalTransform(&XFORM());

	if (Alive()&&light_render->get_active())
	{
		Fmatrix xf;
		if (guid_bone>=0)
		{
			Fmatrix& M = PKinematics(Visual())->LL_GetTransform(u16(guid_bone));
			xf.mul		(XFORM(),M);
		} 
		else 
		{
			xf.set		(XFORM());
		}

		light_render->set_rotation	(xf.k,xf.i);
		light_render->set_position	(xf.c);
		glow_render->set_position	(xf.c);
		//glow_render->set_direction	(xf.k);
		
		if (lanim)
		{
			int frame;
			u32 clr			= lanim->Calculate(Device.fTimeGlobal,frame); // возвращает в формате BGR
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

void CHangingLamp::TurnOn()
{
	light_render->set_active(true);
	glow_render->set_active (true);
	PKinematics(Visual())->LL_SetBoneVisible(guid_bone, TRUE, TRUE);
}

void CHangingLamp::TurnOff()
{
	light_render->set_active(false);
	glow_render->set_active (false);
	PKinematics(Visual())->LL_SetBoneVisible(guid_bone, FALSE, TRUE);
}

void CHangingLamp::Hit(float P,Fvector &dir, CObject* who,s16 element,
					   Fvector p_in_object_space, float impulse, ALife::EHitType /**hit_type/**/)
{
	//inherited::Hit(P,dir,who,element,p_in_object_space,impulse);
	if(m_pPhysicsShell) m_pPhysicsShell->applyImpulseTrace(p_in_object_space,dir,impulse,element);

	if (element==guid_bone)	fHealth =	0.f;
	else					fHealth -=	P*0.1f;

	if (!Alive())	TurnOff	();
}

static BONE_P_MAP bone_map=BONE_P_MAP();
void CHangingLamp::CreateBody(CSE_ALifeObjectHangingLamp	*lamp)
{

	if (!Visual()) return;
	if(m_pPhysicsShell) return;
	CKinematics* pKinematics=PKinematics(Visual());

	m_pPhysicsShell		= P_create_Shell();

	bone_map					.clear();
	LPCSTR	fixed_bones=*lamp->fixed_bones;
	if(fixed_bones)
	{
		int count =					_GetItemCount(fixed_bones);
		for (int i=0 ;i<count; ++i) 
		{
			string64					fixed_bone							;
			_GetItem					(fixed_bones,i,fixed_bone)			;
			u16 fixed_bone_id=pKinematics->LL_BoneID(fixed_bone)			;
			R_ASSERT2(BI_NONE!=fixed_bone_id,"wrong fixed bone")			;
			bone_map.insert(mk_pair(fixed_bone_id,physicsBone()))			;
		}
	
	}
	else
	{
		bone_map.insert(mk_pair(pKinematics->LL_GetBoneRoot(),physicsBone()))			;
	}

	m_pPhysicsShell->build_FromKinematics(pKinematics,&bone_map);
	m_pPhysicsShell->set_PhysicsRefObject(this);
	m_pPhysicsShell->mXFORM.set(XFORM());
	m_pPhysicsShell->Activate(true,true);//,
	//m_pPhysicsShell->SmoothElementsInertia(0.3f);
	m_pPhysicsShell->SetAirResistance();//0.0014f,1.5f
	BONE_P_PAIR_IT i=bone_map.begin(),e=bone_map.end();

	for(;i!=e;i++)
	{
		CPhysicsElement* fixed_element=i->second.element;
		R_ASSERT2(fixed_element,"fixed bone has no physics");
		FixBody(fixed_element->get_body());
	}

	m_pPhysicsShell->mXFORM.set(XFORM());
	m_pPhysicsShell->SetAirResistance(0.001f, 0.02f);
	SAllDDOParams disable_params;
	disable_params.Load(PKinematics(Visual())->LL_UserData());
	m_pPhysicsShell->set_DisableParams(disable_params);
}

void CHangingLamp::net_Export(NET_Packet& P)
{
	VERIFY					(Local());
}

void CHangingLamp::net_Import(NET_Packet& P)
{
	VERIFY					(Remote());
}

BOOL CHangingLamp::UsedAI_Locations()
{
	return					(FALSE);
}
