#include "stdafx.h"
#include "xrLauncherControl.h"

#include "../xray/xr_ioc_wrapper.h"
#include "../xrCore/LocatorAPI_wrapper.h"

using namespace xrLauncher;
using namespace System;

void xrLauncherControl::Init()
{
	InitSoundPage();
	InitModPage();
}


// sound page----------------------------------------------
#define SND_VOLUME_EFFECT		"snd_volume_eff"
#define SND_VOLUME_MUSIC		"snd_volume_music"
#define SND_ACCEL				"snd_acceleration"
#define SND_EFX					"snd_efx"
#define SND_TARGETS				"snd_targets"

void xrLauncherControl::InitSoundPage()
{
	CConsoleWrapper con;
	int  ival=0, imin=0, imax=0;
	float  fval=.0f, fmin=.0f, fmax=.0f;
	int  bval=false;

	con.GetFloat( SND_VOLUME_EFFECT, fval, fmin, fmax);
	this->sndVolEffectTrack->set_Minimum(0);
	this->sndVolEffectTrack->set_Maximum(100);
	this->sndVolEffectTrack->set_Value((int)(fval*100.0f) );

	con.GetFloat( SND_VOLUME_MUSIC, fval, fmin, fmax);
	this->sndVolMusicTrack->set_Minimum(0);
	this->sndVolMusicTrack->set_Maximum(100);
	this->sndVolMusicTrack->set_Value((int)(fval*100.0f));

	con.GetBool( SND_ACCEL, bval);
	this->sndAccelCheck->set_Checked(bval);

	con.GetBool( SND_EFX, bval);
	this->sndEfxCheck->set_Checked(bval);

	con.GetInteger( SND_TARGETS, ival, imin, imax);
	this->sndVolMusicTrack->set_Minimum(imin);
	this->sndVolMusicTrack->set_Maximum(imax);
	this->sndTargetsUpDown->set_Value(ival);

}

void xrLauncherControl::ApplySoundPage()
{
	CConsoleWrapper con;
	int  ival=0, imin=0, imax=0;
	float  fval=.0f, fmin=.0f, fmax=.0f;
	int  bval=false;

	ival = this->sndVolEffectTrack->get_Value();
	con.SetFloat(SND_VOLUME_EFFECT, (ival==0)?0.0f:ival/100.0f);

	ival = this->sndVolMusicTrack->get_Value();
	con.SetFloat(SND_VOLUME_MUSIC, (ival==0)?0.0f:ival/100.0f);

	bval = this->sndAccelCheck->get_Checked();
	con.SetBool(SND_ACCEL, bval);

	bval = this->sndEfxCheck->get_Checked();
	con.SetBool(SND_EFX, bval);

	ival = (int)this->sndTargetsUpDown->get_Value();
	con.SetInteger(SND_TARGETS, ival);

}
// mod page----------------------------------------
#include <string.h>
#include <vcclr.h>

#include "xrLauncher_utils.h"

void xrLauncherControl::InitModPage()
{
	CLocatorAPI_wrapper fs;
	LPCSTR mod_dir = fs.get_path("$mod_dir$");

	// Only get subdirectories that begin with the letter "p."
	String* dirs[] = System::IO::Directory::GetDirectories(mod_dir);
	Console::WriteLine(S"The number of mod directories  is {0}.", __box(dirs->Length));
    Collections::IEnumerator* myEnum = dirs->GetEnumerator();
    while (myEnum->MoveNext()) {
        Console::WriteLine(myEnum->Current);
		String* cur = static_cast<String*>(myEnum->Current);
		String* info_file_name = String::Concat(cur,S"\\mod.inf");
		if(System::IO::File::Exists(info_file_name)){
//			LPCSTR f_name = (char*)(void*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(info_file_name);
			string512 f_name;
			convert(info_file_name, f_name);
			CInifile_wrapper ini(f_name);
			if(!ini.section_exist("general"))
				continue;
			if(!ini.line_exist("general","name"))
				continue;
			
			String* mod_name = ini.r_string("general","name");
			this->modList->Items->Add(mod_name);
		};
			
    }

}
