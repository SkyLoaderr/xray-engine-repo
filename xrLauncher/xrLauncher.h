#pragma once

#ifdef XRLAUNCHER_EXPORTS
#define XRLAUNCHER_API __declspec(dllexport)
#else
#define XRLAUNCHER_API __declspec(dllimport)
#endif

extern "C" __declspec(dllexport) int RunXRLauncher();

struct SmodInfo{
	ref_str		m_mod_name;
	ref_str		m_descr_short;
	ref_str		m_descr_long;
	ref_str		m_version;
	ref_str		m_www;
	ref_str		m_cmd_line;
	xr_vector<ref_str>* m_credits;
	SmodInfo() {m_credits = new xr_vector<ref_str>();}
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