//////////////////////////////////////////////////////////
//	Module			:	Virtual Othello					//
//	File			:	vo.cpp							//
//	Creation date	:	19.12.2000						//
//	Author			:	Dmitriy Iassenev				//
//////////////////////////////////////////////////////////

#include "stdafx.h"

#include "vo.h"
#include "game.h"
#include "interface.h"
#include "misc.h"

#include "mg_pvs.h"
#include "_math.h"

#define SHAPE_CHARACTER '*'

int __cdecl main(int argc, char *argv[])
{
	fVOOutput = fopen("vo.log","at");
	fprintf(fVOOutput,"\n");

	char caDate[100]=__DATE__, caTmp[100];
	bool bBrace = false;
	for (int i=0 ; i<100; i++)
		if ((caDate[i] != ' ') || (!bBrace)) {
			if (caDate[i] == ' ') 
				bBrace = true;
			else 
				bBrace = false;
        
			caTmp[i] = caDate[i];
		}
		else 
			caTmp[i] = '0';
    
	char caString[79];
	memset(caString,0,sizeof(caString));

	memset(caString,SHAPE_CHARACTER,78*sizeof(char));
	vfDualPrintF("%s\n",caString);
	memset(caString + 1, ' ',76*sizeof(char));
	vfDualPrintF("%s\n",caString);
	vfDualPrintF("%c                      Virtual Othello Player v0.4                           %c\n",SHAPE_CHARACTER,SHAPE_CHARACTER);
	vfDualPrintF("%c               for Microsoft Windows 95,98,ME,NT,2000,XP                    %c\n",SHAPE_CHARACTER,SHAPE_CHARACTER);
	vfDualPrintF("%c                 Copyleft(C) 2000-2002 Dmitriy Iassenev                     %c\n",SHAPE_CHARACTER,SHAPE_CHARACTER);
	vfDualPrintF("%c                    Compiled on %s %s                        %c\n",SHAPE_CHARACTER,caTmp,__TIME__,SHAPE_CHARACTER);
	
	vfDualPrintF("%s\n",caString);
	memset(caString,SHAPE_CHARACTER,78*sizeof(char));
	vfDualPrintF("%s\n\n",caString);
    
	bool bBlack = true;
	bool bSolve = false;
    
	if (argc>1) 
		if ((argv[1][0] == 'w') || (argv[1][0] == 'W'))
			bBlack = false;
		else
			if ((argv[1][0] == 'b') || (argv[1][0] == 'B'))
				bBlack = true;
			else
				bSolve = true;
	
	if (!bSolve) {
		if (argc>2)
			dStartTime = atoi(argv[2]);
		
#ifndef STATIC_TT
		if (argc>3)
			MAX_TT_HASH_BITS = atoi(argv[3]);
#endif
		
		vfInitAll	();

		vfDualPrintF("\n%s\n",caString);

		vfPrepareForGame();
		vfInitBoard		(tGlobalSearchParameters.taBoard);

		TBoardCell		tColor			= BLACK;
		char			cCurrentValue	= 0;
		u8				ucEmpties		= 0;
		vfPrepareForMove(tGlobalSearchParameters.taBoard,tColor,&cCurrentValue,&ucEmpties);

		u64				start, finish;

		SetPriorityClass	(GetCurrentProcess(),REALTIME_PRIORITY_CLASS);
		SetThreadPriority	(GetCurrentThread(),THREAD_PRIORITY_TIME_CRITICAL);

		start				= CPU::qwfGetCycleCount();
		mini_max			(10,ucEmpties,tColor,cCurrentValue);
		finish				= CPU::qwfGetCycleCount();

		SetThreadPriority	(GetCurrentThread(),THREAD_PRIORITY_NORMAL);
		SetPriorityClass	(GetCurrentProcess(),NORMAL_PRIORITY_CLASS);

		__asm				emms;

		vfDualPrintF		("%I64u : %f = %f\n", qwVariationCount, float(finish - start)/float(CPU::qwCyclesPerSecond), float(qwVariationCount)*float(CPU::qwCyclesPerSecond)/float(finish - start));
		//vfPlayGame(bBlack);
	}
	else {
		if (argc>2) {
			vfInitAll();

			vfDualPrintF("\n%s\n",caString);

			vfSolveGames(argv[2]);
		}
		else
			vfDualPrintF("File with positions to solve hasn't specified");
	}
	fclose(fVOOutput);
	
	//vfGenerateMPCStats();
	//vfAutoPlay(iPly,bBlack);

	return(0);
}