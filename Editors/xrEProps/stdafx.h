//----------------------------------------------------
// file: stdafx.h
//----------------------------------------------------
#ifndef stdafxH
#define stdafxH

#pragma once

#include <xrCore.h>

//refs
namespace CDB{
	class MODEL;
};
#include "Sound.h"             

#include "xrEProps.h"

#define ENGINE_API
#define DLL_API		__declspec(dllimport)

#include "Defines.h"                 

// libs
#pragma comment		(lib,"xrSoundB.lib")
#pragma comment		(lib,"xrCoreB.lib")
#pragma comment		(lib,"EToolsB.lib")

#endif //stdafxH

