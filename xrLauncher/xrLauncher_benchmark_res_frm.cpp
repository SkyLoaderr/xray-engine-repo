#include "StdAfx.h"
#include "xrLauncher_benchmark_res_frm.h"

using namespace xrLauncher;

void xrLauncher_benchmark_res_frm::_Close(int res)
{
	m_modal_result = res;
	Close();
}

int	 xrLauncher_benchmark_res_frm::_Show(int initial_state)
{
	m_init_state = initial_state;
	Init();
	ShowDialog();
	return m_modal_result;
}

void xrLauncher_benchmark_res_frm::Init()
{
	//read benchmark result from files

	string_path s;
	FS.update_path(s,"$server_root$","tmp_benchmark.ini");
	if(!FS.exist(s))
		return;

	CInifile ini(s,true,true,false);
	int ccount =	ini.line_count("benchmark");
	LPCSTR	s_name;
	LPCSTR	s_value;

	System::Windows::Forms::Label * mins[]={c1_min_lbl,c2_min_lbl,c3_min_lbl,c4_min_lbl,};
	System::Windows::Forms::Label * maxs[]={c1_max_lbl,c2_max_lbl,c3_max_lbl,c4_max_lbl,};
	System::Windows::Forms::Label * avgs[]={c1_avg_lbl,c2_avg_lbl,c3_avg_lbl,c4_avg_lbl,};
	int config_no;
	for (int i=0 ;i<ccount; ++i){
		ini.r_line( "benchmark", i, &s_name, &s_value);
		string128 res_file;
		if(!strstr(s_value,"-benchmark "))
			continue;

		sscanf(s_name, "test_%d" ,&config_no);
		sscanf( strstr(s_value,"-benchmark ")+11,"%[^ ] ",&res_file);
		FS.update_path(s,"$server_root$",res_file);
		CInifile res_ini(s);
		LPCSTR s_min,s_max,s_avg;
		s_min = res_ini.r_string("general","min");
		s_max = res_ini.r_string("general","max");
		s_avg = res_ini.r_string("general","avg");
		mins[config_no]->Text = String::Format("{0} fps.",new String(s_min));
		maxs[config_no]->Text = String::Format("{0} fps.",new String(s_max));
		avgs[config_no]->Text = String::Format("{0} fps.",new String(s_avg));
	}

}