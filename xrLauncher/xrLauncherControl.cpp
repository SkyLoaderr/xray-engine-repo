#include "stdafx.h"
#include "xrLauncherControl.h"

using namespace xrLauncher;


void xrLauncherControl::_Close(int res)
{
	m_modal_result = res;
	Close();
}

int	 xrLauncherControl::_Show(int initial_state)
{
	m_init_state = initial_state;
	Init();
	ShowDialog();
	return m_modal_result;
}

bool xrLauncherControl::isChanged()
{
	return SoundChanged();
}

void xrLauncherControl::ApplyChanges()
{
	ApplySoundPage();
}

void xrLauncherControl::Init()
{
	InitSoundPage();
}


#define SND_VOLUME_EFFECT		"snd_volume_eff"
#define SND_VOLUME_MUSIC		"snd_volume_music"
#define SND_ACCEL				"snd_acceleration"
#define SND_EFX					"snd_efx"
#define SND_TARGETS				"snd_targets"

void xrLauncherControl::InitSoundPage()
{
	CConsole* con = ::Console;
	int  ival=0, imin=0, imax=0;
	float  fval=.0f, fmin=.0f, fmax=.0f;
	int  bval=false;

	con->GetFloat( SND_VOLUME_EFFECT, fval, fmin, fmax);
	this->sndVolEffectTrack->set_Minimum(0);
	this->sndVolEffectTrack->set_Maximum(100);
	this->sndVolEffectTrack->set_Value((int)(fval*100.0f) );

	con->GetFloat( SND_VOLUME_MUSIC, fval, fmin, fmax);
	this->sndVolMusicTrack->set_Minimum(0);
	this->sndVolMusicTrack->set_Maximum(100);
	this->sndVolMusicTrack->set_Value((int)(fval*100.0f));

	con->GetBool( SND_ACCEL, bval);
	this->sndAccelCheck->set_Checked(!!bval);

	con->GetBool( SND_EFX, bval);
	this->sndEfxCheck->set_Checked(!!bval);

	con->GetInteger( SND_TARGETS, ival, imin, imax);
	this->sndTargetsUpDown->set_Value(ival);
	this->sndTargetsUpDown->set_Minimum(imin);
	this->sndTargetsUpDown->set_Maximum(imax);

}

void xrLauncherControl::ApplySoundPage()
{
	string256 buff;
	int  ival=0, imin=0, imax=0;
	float  fval=.0f, fmin=.0f, fmax=.0f;
	int  bval=false;

	ival = this->sndVolEffectTrack->get_Value();
	CConsole* con = ::Console;
	sprintf		(buff,"%s %f", SND_VOLUME_EFFECT, (ival==0)?0.0f:ival/100.0f);
	con->Execute(buff);

	ival = this->sndVolMusicTrack->get_Value();
	sprintf		(buff,"%s %f", SND_VOLUME_MUSIC, (ival==0)?0.0f:ival/100.0f);
	con->Execute(buff);

	bval = this->sndAccelCheck->get_Checked();
	sprintf		(buff,"%s %s",SND_ACCEL,(bval)?"on":"off" );
	con->Execute(buff);

	bval = this->sndEfxCheck->get_Checked();
	sprintf		(buff,"%s %s",SND_EFX,(bval)?"on":"off" );
	con->Execute(buff);

	ival = (int)this->sndTargetsUpDown->get_Value();
	sprintf		(buff,"%s %d", SND_TARGETS, ival);
	con->Execute(buff);

}

bool xrLauncherControl::SoundChanged()
{
	int  ival=0, imin=0, imax=0;
	float  fval=.0f, fmin=.0f, fmax=.0f;
	int  bval=false;
	CConsole* con = ::Console;

	con->GetFloat( SND_VOLUME_EFFECT, fval, fmin, fmax);
	if( fabs(fval-sndVolEffectTrack->get_Value()/100.0f)>0.1)
		return true;

	con->GetFloat( SND_VOLUME_MUSIC, fval, fmin, fmax);
	if( fabs(fval-sndVolMusicTrack->get_Value()/100.0f)>0.1)
		return true;

	con->GetBool( SND_ACCEL, bval);
	if(!!sndAccelCheck->get_Checked()!=bval)
		return true;

	con->GetBool( SND_EFX, bval);
	if(!!sndEfxCheck->get_Checked()!=bval)
		return true;

	con->GetInteger( SND_TARGETS, ival, imin, imax);
	if(ival != sndTargetsUpDown->get_Value())
		return true;

	return false;
}

System::Void xrLauncherControl::ApplyButton_Click(System::Object *  sender, System::EventArgs *  e)
{
	if(isChanged())
		ApplyChanges();
}

System::Void xrLauncherControl::OkButton_Click(System::Object *  sender, System::EventArgs *  e)
{
#undef MessageBox
	if(isChanged())	{
		if(MessageBox::Show(S"Settings was changed. Apply changes now?", S"Request",
          MessageBoxButtons::YesNo, MessageBoxIcon::Question )==DialogResult::Yes )
			ApplyChanges();
		else
			return;

		_Close(0);
	}else
		_Close(0);
}
