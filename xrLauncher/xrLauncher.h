#pragma once

#ifdef XRLAUNCHER_EXPORTS
#define XRLAUNCHER_API __declspec(dllexport)
#else
#define XRLAUNCHER_API __declspec(dllimport)
#endif

extern "C" __declspec(dllexport) int __cdecl RunXRLauncher(int);

struct SmodInfo{
	shared_str		m_mod_name;
	shared_str		m_descr_short;
	shared_str		m_descr_long;
	shared_str		m_version;
	shared_str		m_www;
	shared_str		m_cmd_line;
	xr_vector<shared_str>* m_credits;
	xr_vector<shared_str>* m_archieves;
	SmodInfo() {m_credits = new xr_vector<shared_str>();m_archieves = new xr_vector<shared_str>();}
};
typedef xr_vector<SmodInfo> MOD_INFO;

/*
struct SbenchmarkInfo{
	ref_str		m_name;
	ref_str		m_command_line;
	ref_str		m_script_name; //ltx quality settings
	bool		m_b_passed;
	ref_str		m_out_file;
};
typedef svector<SbenchmarkInfo,4>	BENCHMARK_INFO;
*/