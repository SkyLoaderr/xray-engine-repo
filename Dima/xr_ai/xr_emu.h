///////////////////////////////////////////////////////////////
//	Module 		: xr_emu.h
//	Created 	: 20.03.2002
//  Modified 	: 22.03.2002
//	Author		: Dmitriy Iassenev
//	Description : Emulation of the FPS X-Ray
///////////////////////////////////////////////////////////////

#ifndef __XR_EMU__
#define __XR_EMU__

///////////////////////////////////////////////////////////////
// included headers
///////////////////////////////////////////////////////////////

#include "xr_types.h"

///////////////////////////////////////////////////////////////
// macros definitions
///////////////////////////////////////////////////////////////

#define SQR(x) (x)*(x)

///////////////////////////////////////////////////////////////
// exportes functions
///////////////////////////////////////////////////////////////

extern void vfEmulateGamePlay(int iStartNode,int iFinishNode);
extern double dfGetDistance(NodeCompressed tNode0, NodeCompressed tNode1);
extern int ifConvertToDWord(NodeLink tLink);
extern double dfCriteria(NodeCompressed tCurrentNode, NodeCompressed tFinishNode);

#endif