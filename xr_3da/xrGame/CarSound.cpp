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
		snd_engine_start.create(TRUE,	READ_IF_EXISTS(ini,r_string,"car_sound","engine_start","car\\test_car_start"));
		snd_engine_stop.create(TRUE,	READ_IF_EXISTS(ini,r_string,"car_sound","engine_stop","car\\test_car_stop"));
		engine_start_delay=READ_IF_EXISTS(ini,r_float,"car_sound","engine_sound_start_dellay",0.25f);
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
void CCar::SCarSound::SetSoundPosition(ref_sound &snd)
{
	if (snd._feedback())
	{
		Fvector pos;
		pcar->XFORM().transform_tiny(pos,relative_pos);
		snd.set_position		(pos);
	}
}
void CCar::SCarSound::UpdateStarting()
{	
	SetSoundPosition(snd_engine_start);

	if(snd_engine._feedback())
	{
			UpdateDrive();
	} else
	{
		if(time_state_start+snd_engine_start._handle()->length_ms()*engine_start_delay<Device.dwTimeGlobal)
		{
			snd_engine.play(pcar,sm_Looped);
			UpdateDrive();
		}
	}

	if(!snd_engine_start._feedback())Drive();
}
void CCar::SCarSound::UpdateStoping()
{
	SetSoundPosition(snd_engine_stop);
	if(!snd_engine_stop._feedback())SwitchOff();
}
void CCar::SCarSound::UpdateStalling()
{
	SetSoundPosition(snd_engine_stop);
	if(!snd_engine_stop._feedback())SwitchOff();
}
void CCar::SCarSound::UpdateDrive()
{
float		scale							= 0.2f+pcar->m_current_rpm/pcar->m_torque_rpm; clamp(scale,0.2f,2.0f);
			snd_engine.set_frequency		(scale);
			SetSoundPosition(snd_engine);
}
void CCar::SCarSound::SwitchState(ESoundState new_state)
{
	eCarSound=new_state;
	time_state_start=Device.dwTimeGlobal;
}
void CCar::SCarSound::Update()
{
	if(eCarSound==sndOff) return;
	
	switch (eCarSound)
	{
	case sndStarting	:UpdateStarting	()	;	break;
	case sndDrive		:UpdateDrive	()	;	break;
	case sndStalling	:UpdateStalling	()	;	break;
	case sndStoping		:UpdateStalling	()	;	break;
	}
	

}

void CCar::SCarSound::SwitchOn()
{
	pcar->processing_activate();
}
void CCar::SCarSound::Destroy()
{
	SwitchOff();
	snd_engine.destroy	();
	snd_transmission.destroy();
	snd_engine_stop.destroy();
	snd_engine_start.destroy();
}

void CCar::SCarSound::SwitchOff()
{
	eCarSound=sndOff;
	pcar->processing_deactivate();
}

void CCar::SCarSound::Start()
{
	if(eCarSound==sndOff) SwitchOn();
	SwitchState(sndStarting);
	snd_engine_start.play(pcar);
	SetSoundPosition(snd_engine_start);
}

void CCar::SCarSound::Stall()
{
	if(eCarSound==sndOff)return;
	SwitchState(sndStalling);
	snd_engine.stop_deffered();
	snd_engine_stop.play(pcar);
	SetSoundPosition(snd_engine_stop);
}

void CCar::SCarSound::Stop()
{
	if(eCarSound==sndOff)return;
	SwitchState(sndStoping);
	snd_engine.stop_deffered();
	snd_engine_stop.play(pcar);
	SetSoundPosition(snd_engine_stop);
}

void CCar::SCarSound::Drive()
{
	if(eCarSound==sndOff) SwitchOn();
	SwitchState(sndDrive);
	if(!snd_engine._feedback())snd_engine.play(pcar,sm_Looped);
	SetSoundPosition(snd_engine);
}
void CCar::SCarSound::TransmissionSwitch()
{
	if(snd_transmission._handle()&&eCarSound!=sndOff)
	{
		snd_transmission.play(pcar);
		SetSoundPosition(snd_transmission);
	}
}

