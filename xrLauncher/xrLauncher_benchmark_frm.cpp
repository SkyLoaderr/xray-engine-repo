#include "StdAfx.h"
#include "xrLauncher_benchmark_frm.h"


using namespace xrLauncher;

	public __gc class xrComboBoxItem : public System::Object
	{
	public:
		String*			m_str;
		String*			m_conf;
	public: 
		xrComboBoxItem(LPCSTR name, LPCSTR conf){m_str = new String(name); m_conf = new String(conf);}
		virtual String* ToString(){return m_str;}
	};

void xrLauncher_benchmark_frm::Init()
{

	qualityComboBox->Items->Clear();
	const CLocatorAPI::file* fn = NULL;
	string_path s;
	fn = FS.exist(s,"$game_data$","stalkerBenchmark.inf");
	R_ASSERT(fn);

	CInifile ini(fn->name);
	int ccount =	ini.line_count("quality");
	LPCSTR name;
	LPCSTR value;
	int i;
	for ( i=0 ;i<ccount; ++i){
		ini.r_line( "quality", i, &name, &value);
		
		ref_str	q_name;
		ref_str	q_conf;
		q_name = ini.r_string_wb(value,"name_visual");
		q_conf = ini.r_string_wb(value,"config_file");

		xrComboBoxItem* itm = new xrComboBoxItem(*q_name, *q_conf);
		qualityComboBox->Items->Add(itm);
	}

	ccount = ini.line_count("benchmarks");
	for ( i=0 ;i<ccount; ++i){
		ini.r_line( "benchmarks", i, &name, &value);
		
		ref_str	q_conf;
		q_conf = ini.r_string_wb(value,"command");
		m_test_cmds->Add(new String(*q_conf));
	}
}

bool xrLauncher_benchmark_frm::check_all_correct()
{
#undef MessageBox
	if(!config1checkBox->Checked&&!config2checkBox->Checked&&!config3checkBox->Checked&&!config4checkBox->Checked){
		MessageBox::Show(S"You must select at least one test.", S"Information",
          MessageBoxButtons::OK, MessageBoxIcon::Exclamation);
		return false;
	}
	if(qualityComboBox->SelectedIndex == -1){
		MessageBox::Show(S"You must select quality .", S"Information",
          MessageBoxButtons::OK, MessageBoxIcon::Exclamation);
		return false;
	}
	

	return true;
}

System::Void xrLauncher_benchmark_frm::runBenchmarkBtn_Click(System::Object *  sender, System::EventArgs *  e)
{
	if(!check_all_correct())
		return;

	prepareBenchmarkFile("tmp_benchmark.ini");
	_Close(1);
}

void xrLauncher_benchmark_frm::prepareBenchmarkFile(LPCSTR file_name)
{
	string_path s;
	FS.update_path(s,"$local_root$",file_name);
	if(FS.exist(s))
		FS.file_delete(s);

	string1024 command;
	string32 test_num;
	string512 test_cmd;
	string512 config_file;
	convert((static_cast<xrComboBoxItem*>(qualityComboBox->SelectedItem))->m_conf, config_file);
	LPCSTR nosound = (nosoundCheckBox->Checked)?"-nosound":"";
	CInifile ini(s,false,true,true);

	System::Windows::Forms::CheckBox * tests[]={config1checkBox,config2checkBox,config3checkBox,config4checkBox};
	for(int i=0; i<4; ++i){
		if(!tests[i]->Checked)
			continue;
		
		convert(m_test_cmds->Item[i]->ToString(), test_cmd);
		sprintf(command,"\"-ltx %s %s %s\"", config_file, nosound, test_cmd);

		sprintf(test_num,"test_%d",i);
		ini.w_string("benchmark",test_num,command);
	}

}

void xrLauncher_benchmark_frm::_Close(int res)
{
	m_modal_result = res;
	Close();
}

int	 xrLauncher_benchmark_frm::_Show(int initial_state)
{
	m_init_state = initial_state;
	Init();
	ShowDialog();
	return m_modal_result;
}
