#include "stdafx.h"
#include "xrLauncherControl.h"

using namespace xrLauncher;

void xrLauncherControl::Init()
{
	InitSoundPage();
	InitModPage();
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
	this->sndVolMusicTrack->set_Minimum(imin);
	this->sndVolMusicTrack->set_Maximum(imax);
	this->sndTargetsUpDown->set_Value(ival);

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

void xrLauncherControl::InitModPage()
{
	m_mod_info->clear();
	this->modList->Items->Clear();
	this->modCreditsList->Items->Clear();

	
	FS_Path* pth = FS.get_path("$mod_dir$");
	LPCSTR mod_dir = pth->m_Path;

	xr_vector<LPSTR>* dirs =	FS.file_list_open("$mod_dir$",FS_ListFolders);

	xr_vector<LPSTR>::const_iterator it = dirs->begin();
	string_path s;
    for (;it!=dirs->end();++it) {
		LPCSTR cur = (*it);
		const CLocatorAPI::file* fn = FS.exist(s,"$mod_dir$",cur,"mod.inf");
		if(!fn )
			continue;

		CInifile ini(fn->name);
		if(!ini.section_exist("general"))
			continue;

		LPCSTR pStr = 0;
		m_mod_info->push_back( SmodInfo() );
		SmodInfo& info = m_mod_info->back();

		if(ini.line_exist("general","name")){
			pStr = ini.r_string("general","name");
			strcpy(info.m_mod_name, pStr);
			this->modList->Items->Add(new String(info.m_mod_name) );
		};
		if(ini.line_exist("general","description_short")){
			pStr = ini.r_string("general","description_short");
			strcpy(info.m_descr_short, pStr);
		};
		if(ini.line_exist("general","description_long")){
			pStr = ini.r_string("general","description_long");
			strcpy(info.m_descr_long, pStr);
		};

		if(ini.line_exist("general","version")){
			pStr = ini.r_string("general","version");
			strcpy(info.m_version, pStr);
		};

		if(ini.line_exist("general","www")){
			pStr = ini.r_string("general","www");
			strcpy(info.m_www, pStr);
		};

		if(ini.line_exist("general","command_line")){
			pStr = ini.r_string("general","command_line");
			strcpy(info.m_cmd_line, pStr);
		};

		info.m_credits->clear();
		if(ini.section_exist("creator")){
		int lc = ini.line_count("creator");
		
		info.m_credits->resize(lc+1);

		LPCSTR name;
		LPCSTR value;
		for (int i=0 ;i<lc; ++i){
			ini.r_line( "creator", i, &name, &value);
			if(0==i)
				info.m_credits->at(i) = "Credits:";
			info.m_credits->at(i+1) = value;
		}
	}

    };

	FS.file_list_close(dirs);
	modList_SelectedIndexChanged(0,0);
}


System::Void xrLauncherControl::modList_SelectedIndexChanged(System::Object *  sender, System::EventArgs *  e)
{
	modShortDescrLbl->Text	= S"Short description: ";
	modLongDescrLbl->Text	= S"Long description: ";
	modLinkLbl->Text		= S"Website: ";
	modVersionLbl->Text		= S"Version: ";
	modCreditsList->Items->Clear();
	int index = modList->SelectedIndex;
	if (-1 == index){
		modRunBtn->Enabled = false;
		return;
	}

    SmodInfo& info = m_mod_info->at(index);
	modShortDescrLbl->Text = String::Concat(modShortDescrLbl->Text, new String(info.m_descr_short) );
	modLongDescrLbl->Text = String::Concat(modLongDescrLbl->Text, new String(info.m_descr_long) );
	modLinkLbl->Text = String::Concat(modLinkLbl->Text, new String(info.m_www) );
	modVersionLbl->Text = String::Concat(modVersionLbl->Text, new String(info.m_version) );

	for(u32 i=0; i<info.m_credits->size();++i)
		modCreditsList->Items->Add(new String(*(info.m_credits->at(i))) );

	modRunBtn->Enabled = xr_strlen(info.m_cmd_line)>0;
}

System::Void xrLauncherControl::modRunBtn_Click(System::Object *  sender, System::EventArgs *  e)
{
	int index = modList->SelectedIndex;
	  if (-1 == index)
		  return;

    SmodInfo& info = m_mod_info->at(index);
	CConsole* con = ::Console;
	con->Execute(info.m_cmd_line);
	Close();
}


System::Void xrLauncherControl::runStalker_Click(System::Object *  sender, System::EventArgs *  e)
{
	CConsole* con = ::Console;
	con->Execute("start server(andy_test/single) client(localhost)");
	Close();
}
