#include "StdAfx.h"
#include "xrLauncher_main_frm.h"
#include "xrLauncher_about_dlg.h"

using namespace xrLauncher;
//keys ImageList ->
//					0==not_focused
//					1==pressed
//					2==disabled

void xrLauncher_main_frm::Init()
{
	InitMod();
}

void xrLauncher_main_frm::addFileInfo(LPCSTR fn)
{
		CInifile ini(fn);
		if(!ini.section_exist("general"))
			return;

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
			
			info.m_credits->resize(lc);

			LPCSTR name;
			LPCSTR value;
			for (int i=0 ;i<lc; ++i){
				ini.r_line( "creator", i, &name, &value);
				info.m_credits->at(i) = value;
			}
		}
}

void xrLauncher_main_frm::InitMod()
{
	m_mod_info->clear();
	this->modList->Items->Clear();

	const CLocatorAPI::file* fn = NULL;
	string_path s;
	fn = FS.exist(s,"$game_data$","stalkerGame.inf");
	if(fn)
		addFileInfo(fn->name);
	
	FS_Path* pth = FS.get_path("$mod_dir$");
	LPCSTR mod_dir = pth->m_Path;

	xr_vector<LPSTR>* dirs =	FS.file_list_open("$mod_dir$",FS_ListFolders);

	xr_vector<LPSTR>::const_iterator it = dirs->begin();
    for (;it!=dirs->end();++it) {
		LPCSTR cur = (*it);
		const CLocatorAPI::file* fn = FS.exist(s,"$mod_dir$",cur,"mod.inf");
		if(!fn )
			continue;

		addFileInfo(fn->name);
	}


	FS.file_list_close(dirs);
	modList_SelectedIndexChanged(0,0);
}


System::Void xrLauncher_main_frm::modList_SelectedIndexChanged(System::Object *  sender, System::EventArgs *  e)
{
	int index = modList->SelectedIndex;
	if (-1 == index){
		playBtn->Enabled = false;
		return;
	}

    SmodInfo& info = m_mod_info->at(index);

	playBtn->Enabled = xr_strlen(info.m_cmd_line)>0;

}

System::Void xrLauncher_main_frm::playBtn_Click(System::Object *  sender, System::EventArgs *  e)
{
	int index = modList->SelectedIndex;
	  if (-1 == index)
		  return;

    SmodInfo& info = m_mod_info->at(index);
	CConsole* con = ::Console;
	con->Execute(info.m_cmd_line);
	Close();
}

System::Void xrLauncher_main_frm::aboutBtn_Click(System::Object *  sender, System::EventArgs *  e)
{
	if(!m_about_dlg)
		m_about_dlg = new xrLauncher_about_dlg();
	

	int index = modList->SelectedIndex;
	  if (-1 == index)
		  return;

    SmodInfo& info = m_mod_info->at(index);

	m_about_dlg->Init(info);
	m_about_dlg->ShowDialog();
}
