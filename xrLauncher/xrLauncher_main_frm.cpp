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
			info.m_mod_name = ini.r_string_wb("general","name");
			modListCombo->Items->Add(new String(*info.m_mod_name) );
		};
		if(ini.line_exist("general","description_short")){
			info.m_descr_short = ini.r_string_wb("general","description_short");
		};
		if(ini.line_exist("general","description_long")){
			info.m_descr_long = ini.r_string_wb("general","description_long");
		};

		if(ini.line_exist("general","version")){
			info.m_version = ini.r_string_wb("general","version");
		};

		if(ini.line_exist("general","www")){
			info.m_www = ini.r_string_wb("general","www");
		};

		if(ini.line_exist("general","command_line")){
			info.m_cmd_line = ini.r_string_wb("general","command_line");
		};

		if(ini.line_exist("general","image")){
			string16	_drive;
			string_path	_dir;
			string256	_fname;
			string16	_ext;
			_splitpath(fn,_drive,_dir,_fname,_ext);
			string_path pic_name;

			info.m_picture_name = ini.r_string_wb("general","image");
			strconcat(pic_name,_drive,_dir,*info.m_picture_name);
			info.m_picture_name = pic_name;
		}else
			info.m_picture_name = "";


		if(ini.line_exist("general","creator")){
			info.m_credits_l = ini.r_string_wb("general","creator");
		};
/*
		info.m_credits->clear();
		if(ini.section_exist("creator")){
			int lc = ini.line_count("creator");
			
			info.m_credits->resize(lc);

			LPCSTR name;
			LPCSTR value;
			for (int i=0 ;i<lc; ++i){
				ini.r_line( "creator", i, &name, &value);
				info.m_credits->at(i) = ini.r_string_wb( "creator", name);
			}
		}
*/
		info.m_archieves->clear();
		if(ini.section_exist("packages")){
			int lc = ini.line_count("packages");
			
			info.m_archieves->resize(lc);

			LPCSTR name;
			LPCSTR value;
			for (int i=0 ;i<lc; ++i){
				ini.r_line( "packages", i, &name, &value);
				info.m_archieves->at(i) = ini.r_string_wb( "packages", name);
			}
		}
}

void xrLauncher_main_frm::InitMod()
{
	m_mod_info->clear();
	modListCombo->Items->Clear();

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
	modListCombo->SelectedIndex = 0;
	modList_SelectedIndexChanged(0,0);
}


System::Void xrLauncher_main_frm::modList_SelectedIndexChanged(System::Object *  sender, System::EventArgs *  e)
{
	int index = modListCombo->SelectedIndex;
	if (-1 == index){
		playBtn->Enabled = false;
		return;
	}

    SmodInfo& info = m_mod_info->at(index);

	playBtn->Enabled = xr_strlen(info.m_cmd_line)>0;

	pictureBox1->BackgroundImage->Dispose();
	
	if(xr_strlen(info.m_picture_name)){
		Image * img = Image::FromFile(*info.m_picture_name);
		pictureBox1->BackgroundImage = img;
	}else
		pictureBox1->BackgroundImage = imageList2->Images->get_Item(0);

}

System::Void xrLauncher_main_frm::playBtn_Click(System::Object *  sender, System::EventArgs *  e)
{
//	int index = modList->SelectedIndex;
	int index = modListCombo->SelectedIndex;
	  if (-1 == index)
		  return;

    SmodInfo& info = m_mod_info->at(index);
	string_path pack_path;
	for(u32 i=0;i<info.m_archieves->size();++i){
		FS.update_path(pack_path,"$server_root$",*(info.m_archieves->at(i)));
		FS.register_archieve(pack_path);
	}
	CConsole* con = ::Console;
	con->Execute("cfg_save tmp_launcher.ltx");

	LPCSTR	c_name		= con->ConfigFile;
	SetFileAttributes	(c_name,FILE_ATTRIBUTE_NORMAL);
	IReader* R			= FS.r_open(c_name);
	IReader* R2			= FS.r_open("tmp_launcher.ltx");
	IWriter* W			= FS.w_open(c_name);
	W->w(R->pointer(), R->length() );
	W->w(R2->pointer(), R2->length() );
	

	FS.w_close			(W);
	FS.r_close			(R2);
	FS.r_close			(R);
	
	strcpy(Core.Params,*info.m_cmd_line);
	_Close(0);
}

System::Void xrLauncher_main_frm::aboutBtn_Click(System::Object *  sender, System::EventArgs *  e)
{
	if(!m_about_dlg)
		m_about_dlg = new xrLauncher_about_dlg();
	

//	int index = modList->SelectedIndex;
	int index = modListCombo->SelectedIndex;
	  if (-1 == index)
		  return;

    SmodInfo& info = m_mod_info->at(index);

	m_about_dlg->Init(info);
	m_about_dlg->ShowDialog();
}

System::Void xrLauncher_main_frm::benchmarkBtn_Click(System::Object *  sender, System::EventArgs *  e)
{
	if(!m_benchmark_dlg)
		m_benchmark_dlg = new xrLauncher_benchmark_frm();

	int res = m_benchmark_dlg->_Show(0);
	if (1==res)//run benchmark???
		_Close(1);
	
}

void	 xrLauncher_main_frm::_Close(int res)
{
	m_modal_result = res;
	Close();
}

int	 xrLauncher_main_frm::_Show(int initial_state)
{
	m_init_state = initial_state;
	Init();
	ShowDialog();
	return m_modal_result;
}

System::Void xrLauncher_main_frm::settingsBtn_Click(System::Object *  sender, System::EventArgs *  e)
{
	if(!m_settings_dlg)
		m_settings_dlg = new xrLauncherControl();

	int res = m_settings_dlg->_Show(0);
}
