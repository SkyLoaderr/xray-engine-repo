///////////////////////////////////////////////////////////////
//	Module 		: xr_ai.cpp
//	Created 	: 20.03.2002
//  Modified 	: 22.03.2002
//	Author		: Dmitriy Iassenev
//	Description : Artificial Intelligence for FPS X-Ray
///////////////////////////////////////////////////////////////

#include <stdio.h>
#include <memory.h>

#include "xr_ai.h"
#include "xr_cluster.h"
#include "xr_emu.h"
#include "xr_misc.h"

void main( int argc, char *argv[], char *envp[])
{
	char s[79];
	memset(s,0,sizeof(s));
	memset(s,'*',sizeof(s)-1);
	printf("%s\n",s);
	printf("* AI FPS X-Ray simulator\n");
	printf("%s\n",s);
	vfLoadLevelMap("data\\level_00.map");
	vfClusteriseNodes(0.1);
	//vfEmulateGamePlay(127280,25687);
	vfFreeAll();
}