#include "stdafx.h"
#include "artifact.h"
#include "PhysicsShell.h"
//#include "actor.h"

xr_set<CArtifact*> CArtifact::m_all;

#define CHOOSE_MAX(x,inst_x,y,inst_y,z,inst_z)\
	if(x>y)\
		if(x>z){inst_x;}\
		else{inst_z;}\
	else\
		if(y>z){inst_y;}\
		else{inst_z;}



CArtifact::CArtifact(void) 
{
	shedule.t_min = 20;
	shedule.t_max = 50;

	m_sParticlesName = NULL;

	m_pTrailLight = NULL;
}


CArtifact::~CArtifact(void) 
{
	SoundDestroy(m_detectorSound);
}

void CArtifact::Load(LPCSTR section) 
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

BOOL CArtifact::net_Spawn(LPVOID DC) 
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

	return result;	
}

void CArtifact::net_Destroy() 
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

void CArtifact::OnH_A_Chield() 
{
	inherited::OnH_A_Chield		();

	StopLights();
	if (*m_sParticlesName) 
	{	
		CParticlesPlayer::StopParticles(m_sParticlesName);
	}
}

void CArtifact::OnH_B_Independent() 
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

void CArtifact::UpdateCL() 
{
	inherited::UpdateCL();

	UpdateLights();
}

void CArtifact::shedule_Update	(u32 dt) 
{
	inherited::shedule_Update(dt);
}

void CArtifact::renderable_Render() 
{
	inherited::renderable_Render();
}

void CArtifact::SoundCreate(ref_sound& dest, LPCSTR s_name, int iType, BOOL bCtrlFreq) 
{
	string256 temp;
	if (FS.exist(temp,"$game_sounds$",s_name,".ogg")) 
	{
		Sound->create(dest,TRUE,s_name,iType);
		return;
	}
	Debug.fatal	("Can't find ref_sound '%s'",s_name,*cName());
}

void CArtifact::SoundDestroy(ref_sound& dest) 
{
	::Sound->destroy			(dest);
}

void CArtifact::create_physic_shell	()
{
	create_box2sphere_physic_shell	();
}

//////////////////////////////////////////////////////////////////////////
//	Lights
//////////////////////////////////////////////////////////////////////////
void CArtifact::StartLights()
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

void CArtifact::StopLights()
{
	if(!m_bLightsEnabled) return;
	m_pTrailLight->set_active(false);
}

void CArtifact::UpdateLights()
{
	if(!m_bLightsEnabled || !m_pTrailLight->get_active()) return;
	m_pTrailLight->set_position(Position());
}