// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the XRCORE_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// XRFS_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.

#ifndef xrFSH
#define xrFSH

#pragma once

#include "xrCore.h"

// Our headers
#ifdef XRFS_EXPORTS
#	define XRFS_API __declspec(dllexport)
#else
#	define XRFS_API __declspec(dllimport)
#endif

#include "rt_compressor.h"
#include "FS.h"
#include "log.h"
#include "xr_ini.h"
#include "LocatorAPI.h"
#include "FileSystem.h"

#endif
