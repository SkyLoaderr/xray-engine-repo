#pragma once

#ifdef XRLAUNCHER_EXPORTS
#define XRLAUNCHER_API __declspec(dllexport)
#else
#define XRLAUNCHER_API __declspec(dllimport)
#endif

extern "C" __declspec(dllexport) int RunXRLauncher();
