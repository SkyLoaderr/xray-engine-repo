#include "stdafx.h"
#pragma hdrstop

#include "HangingLamp.h"
#include "../LightAnimLibrary.h"
#include "../xr_collide_form.h"
#include "PhysicsShell.h"
#include "Physics.h"
#include "xrserver_objects_alife.h"
#include "PHElement.h"
#include "../skeletonanimated.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHangingLamp::CHangingLamp	()
{
	Init();
}

CHangingLamp::~CHangingLamp	()
{
}
void CHangingLamp::Init()
{
	fHealth					= 100.f;
	guid_bone				= BI_NONE;
	lanim					= 0;
	ambient_power			= 0.f;
	light_render			= 0;
	light_ambient			= 0;
	glow_render				= 0;
	guid_physic_bone		= NULL;
}

void CHangingLamp::RespawnInit()
{
	Init();
	smart_cast<CKinematics*>(Visual())->LL_SetBonesVisible(u64(-1));
	
}
void CHangingLamp::Center	(Fvector& C) const 
{ 
	if (renderable.visual){
		renderable.xform.transform_tiny(C,renderable.visual->vis.sphere.P);	
	}else{
		C.set	(XFORM().c);
	}
}

float CHangingLamp::Radius	() const 
{ 
	return (renderable.visual)?renderable.visual->vis.sphere.R:EPS;
}

void CHangingLamp::Load		(LPCSTR section)
{
	inherited::Load			(section);
}

void CHangingLamp::net_Destroy()
{
	inherited::net_Destroy	();
	::Render->light_destroy	(light_render);
	::Render->light_destroy	(light_ambient);
	::Render->glow_destroy	(glow_render);
	RespawnInit();
	CPHSkeleton::RespawnInit();
}

BOOL CHangingLamp::net_Spawn(LPVOID DC)
{
	CSE_Abstract			*e		= (CSE_Abstract*)(DC);
	CSE_ALifeObjectHangingLamp	*lamp	= smart_cast<CSE_ALifeObjectHangingLamp*>(e);
	R_ASSERT				(lamp);
	inherited::net_Spawn	(DC);
	Fcolor					clr;

	// set bone id
//	CInifile* pUserData		= K->LL_UserData(); 
//	R_ASSERT3				(pUserData,"Empty HangingLamp user data!",lamp->get_visual());

	if (Visual()){
		CKinematics* K		= smart_cast<CKinematics*>(Visual());
		R_ASSERT			(Visual()&&smart_cast<CKinematics*>(Visual()));
		guid_bone			= K->LL_BoneID	(*lamp->guid_bone);	VERIFY(guid_bone!=BI_NONE);
		collidable.model	= xr_new<CCF_Skeleton>				(this);
	}
	fBrightness				= lamp->brightness;
	clr.set					(lamp->color);						clr.a = 1.f;
	clr.mul_rgb				(fBrightness);

	light_render			= ::Render->light_create();
	light_render->set_shadow(!!lamp->flags.is(CSE_ALifeObjectHangingLamp::flCastShadow));
	light_render->set_type	(lamp->flags.is(CSE_ALifeObjectHangingLamp::flTypeSpot)?IRender_Light::SPOT:IRender_Light::POINT);
	light_render->set_range	(lamp->range);
	light_render->set_color	(clr);
	light_render->set_cone	(lamp->spot_cone_angle);
	light_render->set_texture(*lamp->light_texture);

	if (lamp->glow_texture.size())	{
		glow_render				= ::Render->glow_create();
		glow_render->set_texture(*lamp->glow_texture);
		glow_render->set_color	(clr);
		glow_render->set_radius	(lamp->glow_radius);
	}

	if (lamp->flags.is(CSE_ALifeObjectHangingLamp::flPointAmbient)){
		ambient_power			= lamp->m_ambient_power;
		light_ambient			= ::Render->light_create();
		light_ambient->set_type	(IRender_Light::POINT);
		light_ambient->set_shadow(false);
		clr.mul_rgb				(ambient_power);
		light_ambient->set_range(lamp->m_ambient_radius);
		light_ambient->set_color(clr);
		light_ambient->set_texture(*lamp->m_ambient_texture);
	}

	fHealth					= lamp->m_health;

	lanim					= LALib.FindItem(*lamp->color_animator);

	CPHSkeleton::Spawn(e);
	if(smart_cast<CSkeletonAnimated*>(Visual()))	smart_cast<CSkeletonAnimated*>	(Visual())->PlayCycle("idle");
	if(smart_cast<CKinematics*>(Visual()))	
	{
		smart_cast<CKinematics*>			(Visual())->CalculateBones_Invalidate	();
		smart_cast<CKinematics*>			(Visual())->CalculateBones();
	}
	if (lamp->flags.is(CSE_ALifeObjectHangingLamp::flPhysic)&&
		!guid_physic_bone)	fHealth=0.f;

	if (Alive())			TurnOn	();
	else					{
		processing_activate		();	// temporal enable
		TurnOff					();	// -> and here is disable :)
	}
	
	if (Visual())setVisible	(TRUE);
	setEnabled				(TRUE);

	return					(TRUE);
}


void	CHangingLamp::SpawnInitPhysics	(CSE_Abstract	*D)	
{
	CSE_ALifeObjectHangingLamp	*lamp	= smart_cast<CSE_ALifeObjectHangingLamp*>(D);	
	if (lamp->flags.is(CSE_ALifeObjectHangingLamp::flPhysic))		CreateBody(lamp);
}

void	CHangingLamp::CopySpawnInit		()
{
	CPHSkeleton::CopySpawnInit();
	CKinematics* K=smart_cast<CKinematics*>(Visual());
	if(
		guid_physic_bone&&
		(
		!K->LL_GetBoneVisible(guid_bone)||
		!K->LL_GetBoneVisible(guid_physic_bone->m_SelfID)
		)
		)
	{
		TurnOff();
		guid_physic_bone=NULL;
	}
}
void	CHangingLamp::net_Save			(NET_Packet& P)	
{
	inherited::net_Save(P);
	CPHSkeleton::SaveNetState(P);
}

BOOL	CHangingLamp::net_SaveRelevant	()
{
	return BOOL(PPhysicsShell()!=NULL);
}

void CHangingLamp::shedule_Update	(u32 dt)
{
	CPHSkeleton::Update(dt);


	inherited::shedule_Update		(dt);
}

void CHangingLamp::UpdateCL	()
{
	inherited::UpdateCL		();

	if(m_pPhysicsShell)
	{
		m_pPhysicsShell->InterpolateGlobalTransform(&XFORM());
		if(guid_physic_bone)guid_physic_bone->BonesCallBack(&smart_cast<CKinematics*>(Visual())->LL_GetBoneInstance(u16(guid_physic_bone->m_SelfID)));

	}

	if (Alive() && light_render->get_active())
	{
		Fmatrix xf;
		if (guid_bone!=BI_NONE)
		{
			Fmatrix& M = smart_cast<CKinematics*>(Visual())->LL_GetTransform(u16(guid_bone));
			xf.mul		(XFORM(),M);
			VERIFY(!fis_zero(DET(xf)));
		}
		else 
		{
			xf.set		(XFORM());
		}

		light_render->set_rotation	(xf.k,xf.i);
		light_render->set_position	(xf.c);
		if (glow_render)	glow_render->set_position	(xf.c);
		if (light_ambient){	
			light_ambient->set_rotation	(xf.k,xf.i);
			light_ambient->set_position	(xf.c);
		}
		
		if (lanim)
		{
			int frame;
			u32 clr					= lanim->CalculateBGR(Device.fTimeGlobal,frame); // возвращает в формате BGR
			Fcolor					fclr;
			fclr.set				((float)color_get_B(clr),(float)color_get_G(clr),(float)color_get_R(clr),1.f);
			fclr.mul_rgb			(fBrightness/255.f);
			light_render->set_color	(fclr);
			if (glow_render)		glow_render->set_color	(fclr);
			if (light_ambient) {
				fclr.mul_rgb		(ambient_power);
				light_ambient->set_color(fclr);
			}
		}
	}
}

void CHangingLamp::renderable_Render()
{
	inherited::renderable_Render	();
}

void CHangingLamp::TurnOn	()
{
	light_render->set_active						(true);
	if (glow_render)	glow_render->set_active		(true);
	if (light_ambient)	light_ambient->set_active	(true);
	if (Visual())		smart_cast<CKinematics*>(Visual())->LL_SetBoneVisible(guid_bone, TRUE, TRUE);
	processing_activate		();
}

void CHangingLamp::TurnOff	()
{
	light_render->set_active						(false);
	if (glow_render)	glow_render->set_active		(false);
	if (light_ambient)	light_ambient->set_active	(false);
	if (Visual())		smart_cast<CKinematics*>(Visual())->LL_SetBoneVisible(guid_bone, FALSE, TRUE);
	if(!PPhysicsShell())//if we have physiccs_shell it will call processing deactivate when disable
			processing_deactivate	();
		
}

void CHangingLamp::Hit(float P,Fvector &dir, CObject* who,s16 element,
					   Fvector p_in_object_space, float impulse, ALife::EHitType hit_type)
{
	//inherited::Hit(P,dir,who,element,p_in_object_space,impulse);
	BOOL	bWasAlive		= Alive		();

	if(m_pPhysicsShell) m_pPhysicsShell->applyHit(p_in_object_space,dir,impulse,element,hit_type);

	if (element==guid_bone)	fHealth =	0.f;
	else					fHealth -=	P*0.1f;

	if (bWasAlive && (!Alive()))		TurnOff	();
}

static BONE_P_MAP bone_map=BONE_P_MAP();
void CHangingLamp::CreateBody(CSE_ALifeObjectHangingLamp	*lamp)
{
	if (!Visual())			return;
	if (m_pPhysicsShell)	return;
	
	CKinematics* pKinematics= smart_cast<CKinematics*>	(Visual());

	m_pPhysicsShell			= P_create_Shell();

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

	bone_map.insert(mk_pair(guid_bone,physicsBone()));

	m_pPhysicsShell->build_FromKinematics(pKinematics,&bone_map);
	m_pPhysicsShell->set_PhysicsRefObject(this);
	m_pPhysicsShell->mXFORM.set(XFORM());
	m_pPhysicsShell->Activate(true);//,
	//m_pPhysicsShell->SmoothElementsInertia(0.3f);
	m_pPhysicsShell->SetAirResistance();//0.0014f,1.5f

	BONE_P_PAIR_IT g_i= bone_map.find(guid_bone);
	guid_physic_bone=smart_cast<CPHElement*>(g_i->second.element);
	bone_map.erase(g_i);

	//if(!lanim)pKinematics->Calculate();
	//Fmatrix InvET;
	//InvET.set(pKinematics->LL_GetTransform(guid_physic_bone->m_SelfID));
	//InvET.invert();
	//guid_bone_offset.mul(InvET,pKinematics->LL_GetTransform(guid_bone));

	BONE_P_PAIR_IT i=bone_map.begin(),e=bone_map.end();
	for(;i!=e;i++)
	{
		CPhysicsElement* fixed_element=i->second.element;
		///R_ASSERT2(fixed_element,"fixed bone has no physics");
		if(fixed_element)fixed_element->Fix();
	}

	m_pPhysicsShell->mXFORM.set(XFORM());
	m_pPhysicsShell->SetAirResistance(0.001f, 0.02f);
	SAllDDOParams disable_params;
	disable_params.Load(smart_cast<CKinematics*>(Visual())->LL_UserData());
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
