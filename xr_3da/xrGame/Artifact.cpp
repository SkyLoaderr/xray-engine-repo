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
}

BOOL CArtifact::net_Spawn(LPVOID DC) 
{
	m_all.insert(this);
	return		(inherited::net_Spawn(DC));
}

void CArtifact::net_Destroy() 
{
	m_all.erase(this);
	inherited::net_Destroy();
}

void CArtifact::OnH_A_Chield() 
{
	inherited::OnH_A_Chield		();
}

void CArtifact::OnH_B_Independent() 
{
	inherited::OnH_B_Independent();
}

void CArtifact::UpdateCL() 
{
	inherited::UpdateCL();
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