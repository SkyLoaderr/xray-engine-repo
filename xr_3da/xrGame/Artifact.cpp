#include "stdafx.h"
#include "artifact.h"
#include "PhysicsShell.h"
#include "PhysicsShellHolder.h"
#include "game_cl_base.h"
#include "level.h"

xr_set<CArtefact*> CArtefact::m_all;

#define CHOOSE_MAX(x,inst_x,y,inst_y,z,inst_z)\
	if(x>y)\
		if(x>z){inst_x;}\
		else{inst_z;}\
	else\
		if(y>z){inst_y;}\
		else{inst_z;}



CArtefact::CArtefact(void) 
{
	shedule.t_min = 20;
	shedule.t_max = 50;

	m_sParticlesName = NULL;

	m_pTrailLight = NULL;
}


CArtefact::~CArtefact(void) 
{
	SoundDestroy(m_detectorSound);
}

void CArtefact::Load(LPCSTR section) 
{
	// verify class
	LPCSTR Class = pSettings->r_string(section,"class");
	CLASS_ID load_cls = TEXT2CLSID(Class);
	R_ASSERT(load_cls==SUB_CLS_ID);

	inherited::Load(section);

	LPCSTR m_detectorSoundName = pSettings->r_string(section,"detector_sound");
	SoundCreate(m_detectorSound, m_detectorSoundName);

	m_fDetectionDist = pSettings->r_float(section,"detector_dist");

	if (pSettings->line_exist(section, "particles"))
	{
		m_sParticlesName = pSettings->r_string(section, "particles");
	}

	m_bLightsEnabled = !!pSettings->r_bool(section, "lights_enabled");
	if(m_bLightsEnabled)
	{
		sscanf(pSettings->r_string(section,"trail_light_color"), "%f,%f,%f", 
			&m_TrailLightColor.r, &m_TrailLightColor.g, &m_TrailLightColor.b);
		m_fTrailLightRange	= pSettings->r_float(section,"trail_light_range");
	}
}

BOOL CArtefact::net_Spawn(LPVOID DC) 
{
	BOOL result = inherited::net_Spawn(DC);
	m_all.insert(this);
	if (*m_sParticlesName) 
	{
		Fvector dir;
		dir.set(0,1,0);
		CParticlesPlayer::StartParticles(m_sParticlesName,dir,ID(),-1, false);
	}

	VERIFY(m_pTrailLight == NULL);
	m_pTrailLight = ::Render->light_create();
	m_pTrailLight->set_shadow(true);

	StartLights();
	/////////////////////////////////////////
	m_CarringBoneID = u16(-1);
	/////////////////////////////////////////
	CSkeletonAnimated	*K=PSkeletonAnimated(Visual());
	if(K)K->PlayCycle("idle");
	return result;	
}

void CArtefact::net_Destroy() 
{
	m_all.erase(this);
	if (*m_sParticlesName) 
	{	
		CParticlesPlayer::StopParticles(m_sParticlesName);
	}
	inherited::net_Destroy();

	StopLights();
	::Render->light_destroy(m_pTrailLight);
	m_pTrailLight = NULL;
}

void CArtefact::OnH_A_Chield() 
{
	inherited::OnH_A_Chield		();

	StopLights();
	if (GameID() == GAME_SINGLE)
	{
		if (*m_sParticlesName) 
		{	
			CParticlesPlayer::StopParticles(m_sParticlesName);
		}
	}
	else
	{
		CKinematics* K	= PKinematics(H_Parent()->Visual());
		if (K)
			m_CarringBoneID			= K->LL_BoneID("bip01_head");
		else
			m_CarringBoneID = u16(-1);
	}
}

void CArtefact::OnH_B_Independent() 
{
	inherited::OnH_B_Independent();

	StartLights();
	if (*m_sParticlesName) 
	{
		Fvector dir;
		dir.set(0,1,0);
		CParticlesPlayer::StartParticles(m_sParticlesName,dir,ID(),-1, false);
	}
}

void CArtefact::UpdateCL() 
{
	Fvector vel = {0, 0, 0};
	if (H_Parent()) 
	{
		CPhysicsShellHolder* pPhysicsShellHolder = dynamic_cast<CPhysicsShellHolder*>(H_Parent());
		if(pPhysicsShellHolder) pPhysicsShellHolder->PHGetLinearVell(vel);
	}

	CParticlesPlayer::SetParentVel(vel);
	inherited::UpdateCL();

	UpdateLights();
}

void CArtefact::shedule_Update	(u32 dt) 
{
	inherited::shedule_Update(dt);
}

void CArtefact::renderable_Render() 
{
	inherited::renderable_Render();
}

void CArtefact::SoundCreate(ref_sound& dest, LPCSTR s_name, int iType, BOOL bCtrlFreq) 
{
	string256 temp;
	if (FS.exist(temp,"$game_sounds$",s_name,".ogg")) 
	{
		Sound->create(dest,TRUE,s_name,iType);
		return;
	}
	Debug.fatal	("Can't find ref_sound '%s'",s_name,*cName());
}

void CArtefact::SoundDestroy(ref_sound& dest) 
{
	::Sound->destroy			(dest);
}

void CArtefact::create_physic_shell	()
{
	///create_box2sphere_physic_shell	();
	m_pPhysicsShell=P_build_Shell(this,false);
	m_pPhysicsShell->Deactivate();
}

//////////////////////////////////////////////////////////////////////////
//	Lights
//////////////////////////////////////////////////////////////////////////
void CArtefact::StartLights()
{
	if(!m_bLightsEnabled) return;

	//включить световую подсветку от двигателя
	m_pTrailLight->set_color(m_TrailLightColor.r, 
		m_TrailLightColor.g, 
		m_TrailLightColor.b);

	m_pTrailLight->set_range(m_fTrailLightRange);
	m_pTrailLight->set_position(Position()); 
	m_pTrailLight->set_active(true);
}

void CArtefact::StopLights()
{
	if(!m_bLightsEnabled) return;
	m_pTrailLight->set_active(false);
}

void CArtefact::UpdateLights()
{
	if(!m_bLightsEnabled || !m_pTrailLight->get_active()) return;
	m_pTrailLight->set_position(Position());
}