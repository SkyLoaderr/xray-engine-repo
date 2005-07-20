#include "stdafx.h"
#ifdef DEBUG
#include "ode_include.h"
#include "../StatGraph.h"
#include "PHDebug.h"
#endif
#include "alife_space.h"
#include "hit.h"
#include "PHDestroyable.h"
#include "car.h"
#include "../skeletoncustom.h"

CCar::SCarSound::SCarSound(CCar* car)
{
	volume                 =1.f;
	pcar=car;
	relative_pos.set(0.f,0.5f,-1.f);
}

CCar::SCarSound::~SCarSound()
{
	
}
void CCar::SCarSound::Init()
{
	CInifile* ini=smart_cast<CKinematics*>(pcar->Visual())->LL_UserData();
	if (ini->section_exist("car_sound") && ini->line_exist("car_sound","snd_volume"))
	{
		volume  			= ini->r_float("car_sound","snd_volume");
		snd_engine.create	(TRUE,ini->r_string("car_sound","snd_name"));//
		if(ini->line_exist("car_sound","relative_pos"))
		{
			relative_pos.set(ini->r_fvector3("car_sound","relative_pos"));
		}
		if(ini->line_exist("car_sound","transmission_switch"))
		{
			snd_transmission.create(TRUE,ini->r_string("car_sound","transmission_switch"));
		}
	
	
	} else {
		Msg					("! Car doesn't contain sound params");
	}
	eCarSound=sndOff;
}

void CCar::SCarSound::Update()
{
	if(eCarSound==sndOff) return;
	//float		velocity						= V.magnitude();
	float		scale							= 0.2f+pcar->m_current_rpm/pcar->m_torque_rpm; clamp(scale,0.2f,2.0f);

#pragma todo("Dima to Kostya : С тебя - пиво (Черниговское белое 0.5л)!")
	if (snd_engine._feedback())
	{
		Fvector pos;
		pcar->XFORM().transform_tiny(pos,relative_pos);
		snd_engine.set_position		(pos);
	}

	switch (eCarSound)
	{
	case sndDrive:
	case sndStarting:
		snd_engine.set_frequency		(scale);
		snd_engine.set_volume			(volume);
		break;
	case sndStalling:
	case sndStoping:
		u32 time_passed=Device.dwTimeGlobal-time_state_start;
		if(time_passed>2500) 
		{
			SwitchOff();
			return;
		}
		snd_engine.set_volume(volume*1000.f/time_passed);
		break;
	}
}

void CCar::SCarSound::SwitchOn()
{
	Fvector pos;
	pcar->XFORM().transform_tiny(pos,relative_pos);
	snd_engine.play_at_pos			(pcar,pos,TRUE);
	pcar->processing_activate();
}
void CCar::SCarSound::Destroy()
{
	SwitchOff();
	snd_engine.destroy	();
	snd_transmission.destroy();
}

void CCar::SCarSound::SwitchOff()
{
	snd_engine.stop();
	eCarSound=sndOff;
	pcar->processing_deactivate();
}

void CCar::SCarSound::Start()
{
	if(eCarSound==sndOff) SwitchOn();
	eCarSound=sndStarting;
	time_state_start=Device.dwTimeGlobal;
}

void CCar::SCarSound::Stall()
{
	if(eCarSound==sndOff)return;
	eCarSound=sndStalling;
	time_state_start=Device.dwTimeGlobal;
}

void CCar::SCarSound::Stop()
{
	if(eCarSound==sndOff)return;
	eCarSound=sndStoping;
	time_state_start=Device.dwTimeGlobal;
}

void CCar::SCarSound::Drive()
{

	if(eCarSound==sndOff) SwitchOn();
	eCarSound=sndDrive;
	time_state_start=Device.dwTimeGlobal;
}
void CCar::SCarSound::TransmissionSwitch()
{
	Fvector pos;
	pcar->XFORM().transform_tiny(pos,relative_pos);
	if(snd_transmission._handle())
	{
		snd_transmission.play_at_pos(pcar,pos);
	}
}

