#pragma once

#ifdef XRLAUNCHER_EXPORTS
#define XRLAUNCHER_API __declspec(dllexport)
#else
#define XRLAUNCHER_API __declspec(dllimport)
#endif

extern "C" __declspec(dllexport) int RunXRLauncher();

struct SmodInfo{
	string128	m_mod_name;
	string128	m_descr_short;
	string2048	m_descr_long;
	string64	m_version;
	string512	m_www;
	string512	m_cmd_line;
	xr_vector<ref_str>* m_credits;
	SmodInfo() {m_credits = new xr_vector<ref_str>();}
};
typedef xr_vector<SmodInfo> MOD_INFO;
