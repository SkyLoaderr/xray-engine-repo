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
	return SoundChanged() || RenderChanged();
}

void xrLauncherControl::ApplyChanges()
{
	ApplySoundPage();
	ApplyRenderPage();
}

void xrLauncherControl::Init()
{
	InitSoundPage();
	InitRenderPage();
	goSoundBtn_Click(0,0);
}


#define SND_VOLUME_EFFECT		"snd_volume_eff"
#define SND_VOLUME_MUSIC		"snd_volume_music"
#define SND_ACCEL				"snd_acceleration"
#define SND_EFX					"snd_efx"
#define SND_TARGETS				"snd_targets"
#define SND_RATE				"snd_freq"
#define SND_MODEL				"snd_model"

void setCBActive(ComboBox* box, LPCSTR str)
{
	String* s = new String(str);
	int idx = box->Items->IndexOf(s);
	box->SelectedIndex = idx;
}

LPCSTR getCBActive(ComboBox* box)
{
	static string512 buff;
	ZeroMemory(&buff,sizeof(string512));
	convert(box->SelectedItem->ToString(),buff);
	return buff;
}
bool testCBActive(ComboBox* box, LPCSTR str)
{
	return 0 == box->SelectedItem->ToString()->CompareTo(str);
}

void xrLauncherControl::InitSoundPage()
{
	CConsole* con = ::Console;
	int  ival=0, imin=0, imax=0;
	float  fval=.0f, fmin=.0f, fmax=.0f;
	int  bval=false;

	con->GetBool( SND_ACCEL, bval);
	sndAccelCheck->set_Checked(!!bval);

	con->GetBool( SND_EFX, bval);
	sndEfxCheck->set_Checked(!!bval);

	con->GetInteger( SND_TARGETS, ival, imin, imax);
	sndTargetsUpDown->set_Value(ival);
	sndTargetsUpDown->set_Minimum(imin);
	sndTargetsUpDown->set_Maximum(imax);
	
	setCBActive(soundSampleRateCombo, con->GetToken(SND_RATE));
	setCBActive(soundQualityCombo, con->GetToken(SND_MODEL) );
	disableSoundCheck->set_Checked( testCoreParam("-nosound") );


}

void xrLauncherControl::ApplySoundPage()
{
	Log("ApplySoundPage -begin");
	if(!SoundChanged())
		return;
	string256 buff;
	int  ival=0, imin=0, imax=0;
	float  fval=.0f, fmin=.0f, fmax=.0f;
	int  bval=false;
	CConsole* con = ::Console;
	Log("ApplySoundPage -sndAccelCheck");
	bval = sndAccelCheck->get_Checked();
	sprintf		(buff,"%s %s",SND_ACCEL,(bval)?"on":"off" );
	con->Execute(buff);

	Log("ApplySoundPage -sndEfxCheck");
	bval = sndEfxCheck->get_Checked();
	sprintf		(buff,"%s %s",SND_EFX,(bval)?"on":"off" );
	con->Execute(buff);

	Log("ApplySoundPage -sndTargetsUpDown");
	ival = (int)sndTargetsUpDown->get_Value();
	sprintf		(buff,"%s %d", SND_TARGETS, ival);
	con->Execute(buff);

	Log("ApplySoundPage -soundSampleRateCombo");
	sprintf		(buff,"%s %s", SND_RATE, getCBActive(soundSampleRateCombo) );
	con->Execute(buff);

	Log("ApplySoundPage -soundQualityCombo");
	sprintf		(buff,"%s %s", SND_MODEL, getCBActive(soundQualityCombo));
	con->Execute(buff);

	Log("ApplySoundPage -disableSoundCheck");
	if(disableSoundCheck->get_Checked()){
		setCoreParam(" -nosound");
	}else{
		resetCoreParam(" -nosound");

	}

}

bool xrLauncherControl::SoundChanged()
{
	Log("SoundChanged - begin");
	int  ival=0, imin=0, imax=0;
	float  fval=.0f, fmin=.0f, fmax=.0f;
	int  bval=false;
	CConsole* con = ::Console;

	con->GetBool( SND_ACCEL, bval);
	if(!!sndAccelCheck->get_Checked()!=!!bval)
		return true;

	con->GetBool( SND_EFX, bval);
	if(!!sndEfxCheck->get_Checked()!=!!bval)
		return true;

	con->GetInteger( SND_TARGETS, ival, imin, imax);
	if(ival != sndTargetsUpDown->get_Value())
		return true;

	if( !testCBActive(soundSampleRateCombo, con->GetToken(SND_RATE)) )
		return true;

	if( !testCBActive(soundQualityCombo, con->GetToken(SND_MODEL)) )
		return true;

	if( testCoreParam("-nosound") !=disableSoundCheck->get_Checked() )
		return true;

	return false;
}

#define RND_VSYNC				"rs_no_v_sync"
#define RND_FORCE60				"rs_refresh_60hz"
#define RND_RASTER				"r__supersample"
#define RND_TEXTURELOD			"texture_lod"



void xrLauncherControl::InitRenderPage()
{
	int  ival=0, imin=0, imax=0;
	float  fval=.0f, fmin=.0f, fmax=.0f;
	int  bval=false;
	CConsole* con = ::Console;
	string16 sss;
	if(testCoreParam("-r2"))
		strcpy(sss,"R2");
	else
		strcpy(sss,"R1");

	setCBActive(renderCombo,sss);
	
	con->GetBool( RND_VSYNC, bval);
	vertSyncCheck->set_Checked(!bval);

	con->GetBool( RND_FORCE60, bval);
	force60HzCheck->set_Checked(!!bval);

	disableDistortionCheck->set_Checked( testCoreParam("-nodistort") );

	con->GetInteger( RND_TEXTURELOD, ival, imin, imax);
	textureLodTrack->set_Value(ival);

	con->GetInteger( RND_RASTER, ival, imin, imax);
	rasterTrack->set_Value(ival);

	ditherShadowsCheck->set_Checked( testCoreParam("-sjitter")   );
	disableShadowsCheck->set_Checked( testCoreParam("-notsh") );
}

void xrLauncherControl::ApplyRenderPage()
{
	Log("ApplyRenderPage - begin");
	string256 buff;
	int  ival=0, imin=0, imax=0;
	float  fval=.0f, fmin=.0f, fmax=.0f;
	int  bval=false;
	CConsole* con = ::Console;

	if(!RenderChanged())
		return;

	Log("ApplyRenderPage - renderCombo");
	if( testCBActive(renderCombo,"R2") ){
			// todo: add "-r2" into command line
			setCoreParam(" -r2");
	}else{
			// todo: remove "-r2" from command line, if exists
			resetCoreParam(" -r2");
	};

	Log("ApplyRenderPage - vertSyncCheck");
	bval = vertSyncCheck->get_Checked();
	sprintf		(buff,"%s %s",RND_VSYNC,(!bval)?"on":"off" );
	con->Execute(buff);

	Log("ApplyRenderPage - force60HzCheck");
	bval = force60HzCheck->get_Checked();
	sprintf		(buff,"%s %s",RND_FORCE60,(bval)?"on":"off" );
	con->Execute(buff);

	Log("ApplyRenderPage - textureLodTrack");
	ival = textureLodTrack->get_Value();
	sprintf		(buff,"%s %d", RND_TEXTURELOD, ival);
	con->Execute(buff);

	Log("ApplyRenderPage - rasterTrack");
	ival = rasterTrack->get_Value();
	sprintf		(buff,"%s %d", RND_RASTER, ival);
	con->Execute(buff);

	Log("ApplyRenderPage - disableDistortionCheck");
	if(disableDistortionCheck->get_Checked()){
		//add -nodistort
		setCoreParam(" -nodistort");
	}else{
		//remove -nodistort
		resetCoreParam(" -nodistort");
	}

	Log("ApplyRenderPage - ditherShadowsCheck");
	if(ditherShadowsCheck->get_Checked() ){
		//add -sjitter
		setCoreParam(" -sjitter");
	}else{
		//remove -sjitter
		resetCoreParam(" -sjitter");
	};

	Log("ApplyRenderPage - disableShadowsCheck");
	if(disableShadowsCheck->get_Checked() ){
		//add -notsh
		setCoreParam(" -notsh");
	}else{
		//remove -notsh
		resetCoreParam(" -notsh");
	};

}

bool xrLauncherControl::RenderChanged()
{
	Log("RenderChanged - begin");
	int  ival=0, imin=0, imax=0;
	float  fval=.0f, fmin=.0f, fmax=.0f;
	int  bval=false;
	CConsole* con = ::Console;

	if( ( testCBActive(renderCombo,"R1") && testCoreParam("-r2") ) ||
		( testCBActive(renderCombo,"R2") && !testCoreParam("-r2"))    )
			return true;
	
	con->GetBool( RND_VSYNC, bval);
	if(!bval != vertSyncCheck->get_Checked())
		return true;

	con->GetBool( RND_FORCE60, bval);
	if(!!bval != force60HzCheck->get_Checked())
		return true;
	if(disableDistortionCheck->get_Checked() != testCoreParam("-nodistort") )
		return true;

	con->GetInteger( RND_TEXTURELOD, ival, imin, imax);
	if(textureLodTrack->get_Value() != ival)
		return true;

	con->GetInteger( RND_RASTER, ival, imin, imax);
	if(rasterTrack->get_Value() != ival)
		return true;

	if(ditherShadowsCheck->get_Checked() != testCoreParam("-sjitter"))
		return true;

	if(disableShadowsCheck->get_Checked() != testCoreParam("-notsh"))
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

