//////////////////////////////////////////////////////////
//	Module			:	Interface routines				//
//	File			:	interface.cpp					//
//	Creation date	:	22.01.2001						//
//	Author			:	Dmitriy Iassenev				//
//////////////////////////////////////////////////////////

#include "stdafx.h"

#include "board.h"
#include "game.h"
#include "interface.h"
#include "misc.h"
#include "mg_pvs.h"
#include "mpc.h"
#include "tt.h"
#include "_time.h"

#define TestBit(p,i,j)  ((p) &  ((u64)1 << (((i) << 3) | (j))))

#define POSITION_COUNT   1773
#define MAX_FIXED_DEPTH    20//30 //17
#define MPC_STAGE_COUNT    60
#define START_STAGE			6
#define START_POSITION	  971

u32 dwaNumbers[65][100000];
u32 dwaCounts[65];
u32 dwaStageNumbers[MPC_STAGE_COUNT][POSITION_COUNT]; 
u32 dwaStageCounts[MPC_STAGE_COUNT];

#define cool(x) (x - floor(x) >= 0.5? ceil(x) : floor(x))

void vfSearchMPCDepths(TBoardCell *taBoard,TBoardCell tColor, u8 ucPly,SFeature *taResults)
{
	char       cCurrentValue = 0;
	u8      ucEmpties = 0;

	vfPrepareForMove(taBoard,tColor,&cCurrentValue,&ucEmpties);
	
	if (ucEmpties == 1) {
		u32 dwFlipCount;
		bfMakeMoveIfAvailable(tColor,taGlobalMoveStack[0],&cCurrentValue,&dwFlipCount);
		vfUndo(dwFlipCount,tColor == BLACK ? WHITE : BLACK);
		vfPrintPrincipalVariation(taGlobalMoveStack[0],1,tColor,cCurrentValue*128,true,1,1);
		return;
	}
	
	SFeature   tValue = 0;
	SFeature   tOldValue = -INFINITY;
	
	tOldValue = tLastValue;

	if (ucEmpties > 0) {
		for (u8 ucDepth = 0; ucDepth < ucPly; ucDepth++) {
			taResults[ucDepth] = (tColor == WHITE ? -1 : 1)*tfSearchGameRoot(ucDepth,ucEmpties,-INFINITY,+INFINITY,tColor,cCurrentValue,cGlobalTimeStamp,false);
		}
	}
}

/**
void vfExtractBoard(TBoardCell *taBoard, TBoard tBoard)
{
	memset(taBoard,DUMMY,BOARD_SIZE*sizeof(TBoardCell));
	for (int i=0; i<8; i++)
		for (int j=0; j<8; j++)
			if (TestBit(tBoard.empties,i,j))
				if (TestBit(tBoard.colors,i,j))
					taBoard[Index(i,j)] = BLACK;
				else
					taBoard[Index(i,j)] = WHITE;
			else
				taBoard[Index(i,j)] = EMPTY;
}

#define bmBetween(i,j,k) ((i >= j) && (i <= k))

void vfGenerateMPCStats()
{
	FILE *faStages[MPC_STAGE_COUNT];
	FILE *faMPCStages[MPC_STAGE_COUNT];
	FILE *faTextOutput[MPC_STAGE_COUNT];
	
	TBoardCell taBoard[BOARD_SIZE];
	TBoardCell tColor;
	SFeature   taResults[MAX_FIXED_DEPTH];
	SFeature   tTemp;
	TBoard	   tCompressedBoard;
	char	   caFileName[20];
	char	   cGameResult;

	char       caMaxDepths[60];

	vfPrepareForGame();

	memset(caFileName,0,sizeof(caFileName));
	for (int i=START_STAGE; i<MPC_STAGE_COUNT; i++) {
		
		caFileName[13] = '0' + (i+1) / 10;
		caFileName[14] = '0' + (i+1) % 10;
		
		memcpy(caFileName,"mpc\\position.",13*sizeof(char)) ;
		faStages[i] = fopen(caFileName,"rb");
		
		memcpy(caFileName,"res\\mpcstats.",13*sizeof(char)) ;
		faMPCStages[i] = fopen(caFileName,"wb");
		
		memcpy(caFileName,"res\\txtstats.",13*sizeof(char)) ;
		faTextOutput[i] = fopen(caFileName,"wt");

		caMaxDepths[i] = 13;
		/**
		if (bmBetween(i,0,3))
			caMaxDepths[i] = 13;
		else
			if (bmBetween(i,4,10))
				caMaxDepths[i] = 13;
			else
				if (bmBetween(i,11,33))
					caMaxDepths[i] = 13;
				else
					if (bmBetween(i,34,35))
						caMaxDepths[i] = 13;
					else
						if (bmBetween(i,36,40))
							caMaxDepths[i] = 13;
						else
							if (bmBetween(i,41,59))
								caMaxDepths[i] = 19;
							else
								if (bmBetween(i,43,59))
									caMaxDepths[i] = 25;
//		if (bmBetween(i,0,3))
//			caMaxDepths[i] = 0;
//		else
//			if (bmBetween(i,4,10))
//				caMaxDepths[i] = 13;
//			else
//				if (bmBetween(i,11,33))
//					caMaxDepths[i] = 11;
//				else
//					if (bmBetween(i,34,38))
//						caMaxDepths[i] = 13;
//					else
//						if (bmBetween(i,39,41))
//							caMaxDepths[i] = 15;
//						else
//							if (bmBetween(i,42,42))
//								caMaxDepths[i] = 17;
//							else
//								if (bmBetween(i,43,59))
//									caMaxDepths[i] = 16;
	}

	memset(dwaStageNumbers, 0, sizeof(dwaStageNumbers));
	memset(dwaStageCounts, 0, sizeof(dwaStageCounts));
	
	for ( i=START_STAGE; i<MPC_STAGE_COUNT; i++) {
		memset(dwaNumbers, 0, sizeof(dwaNumbers));
		memset(dwaCounts, 0, sizeof(dwaCounts));
		for (int j=0; !feof(faStages[i]); j++) {
			fread(&tCompressedBoard,1,sizeof(tCompressedBoard),faStages[i]);
			fread(&tColor,1,sizeof(TBoardCell),faStages[i]);
			fread(&cGameResult,1,sizeof(char),faStages[i]);
			u32 dwIndex = (cGameResult + 64) >> 1;
			dwaNumbers[dwIndex][dwaCounts[dwIndex]++] = j;
		}
		fseek(faStages[i],0,SEEK_SET);
//		printf("Stage #%2d\n",i);
//		printf("Result  Count\n");
		u32 dwSum = 0;
		for ( j=0; j<65; j++)
			dwSum+=dwaCounts[j];
			;//printf(" %3d%8d\n",(j << 1) - 64,dwaCounts[j]);
		//printf("Total : %d\n",dwSum);

		dwaStageCounts[i] = 0;
		//u32 ok = 0;
		for ( j=0; j<65; j++) {
			u32 dwNumber = (u32)cool((f64)(POSITION_COUNT*dwaCounts[j])/dwSum);
			for (u32 k=0; k<dwNumber && dwaStageCounts[i] < POSITION_COUNT; k++) {
				//if (dwaNumbers[j][k] < START_POSITION)
				//	ok++;
				//else
					dwaStageNumbers[i][dwaStageCounts[i]++] = dwaNumbers[j][k];
			}
		}
		//printf("%d\n",ok);
	}

	//return;
	
	for ( i=START_POSITION; i<POSITION_COUNT; i++) {
		
		qwStartThink = qwfClock();
		
		printf("Game #%4d \n",i);
		for (int j=START_STAGE; j<MPC_STAGE_COUNT; j++) {
			if (j == START_STAGE)
				printf(" #");
			if (i == START_POSITION)
				fprintf(faTextOutput[j],"#### ");
			for (int k=0; k<caMaxDepths[j]; k++) {
				if (j == START_STAGE)
					printf("   %2d ",k);
				if (i == START_POSITION)
					fprintf(faTextOutput[j],"   %2d ",k);
			}
			if (j == START_STAGE)
				printf(" Score\n");
			if (i == START_POSITION)
				fprintf(faTextOutput[j],"Score\n");
		}

		for ( j=START_STAGE; j<MPC_STAGE_COUNT; j++) {
			
			if (dwaStageCounts[j] <= (u32)i)
				continue;

			fseek(faStages[j],18*dwaStageNumbers[j][i],SEEK_SET);
			fread(&tCompressedBoard,1,sizeof(tCompressedBoard),faStages[j]);
			fread(&tColor,1,sizeof(TBoardCell),faStages[j]);
			fread(&cGameResult,1,sizeof(char),faStages[j]);

			tTemp = (SFeature)(cGameResult)*128;
			
			vfExtractBoard(taBoard,tCompressedBoard);

			//vfSearchMPCDepths(taBoard,tColor,MAX_FIXED_DEPTH,taResults);
			vfSearchMPCDepths(taBoard,tColor,caMaxDepths[j],taResults);
			
			fwrite(taResults,sizeof(taResults),1,faMPCStages[j]);
			fwrite(&tTemp,1,sizeof(SFeature),faMPCStages[j]);

			printf("%2d",j+1);
			fprintf(faTextOutput[j],"%4d",i);

			for (int k=0; k<caMaxDepths[j]; k++) {
				printf("%6d",taResults[k]);
				fprintf(faTextOutput[j],"%6d",taResults[k]);
			}

			printf("%6d\n",tTemp);
			fprintf(faTextOutput[j],"%6d\n",tTemp);

			if (i % 1 == 0) {
				fflush(faMPCStages[j]);
				fflush(faTextOutput[j]);
			}
		}
		qwStopThink = qwfClock();
		
		printf("Time elapsed : %8.3f\n",(f64)(s64(qwStopThink - qwStartThink))/TICKS_PER_SECOND);
	}
	
	for ( i=START_STAGE; i<MPC_STAGE_COUNT; i++) {
		fclose(faStages[i]);
		fclose(faMPCStages[i]);
		fclose(faTextOutput[i]);
	}
}
/**/

void vfLoadGame(TBoardCell *taBoard, TBoardCell *tColor, u8 *ucEmpties)
{
	//char s[] = "O--OOOOX-OOOOOOXOOXXOOOXOOXOOOXXOOOOOOXX---OOOOX----O--X-------- X";
/**
	char s[67] = "\
--O--O--\
--OOOO--\
-OOXOOX-\
OOOOOOXX\
-XXXOXO-\
-XXXXXXO\
--XXX--X\
-XXX----\
 X";
/**/
/**
	char s[67] = "\
--------\
----OX--\
--OXXO--\
--XOOO--\
-XXXXX--\
--XXOX--\
---X----\
--------\
 X";
/**/
/**
	char s[67] = "\
--------\
-----O--\
--XXOX--\
--OOOXO-\
--OOOX--\
--OXOX--\
----OO--\
--------\
 X";
/**
	char s[67] = "\
-XXXXXX-\
--XOO---\
-XOXOX-X\
-OOOXOXX\
-XOOXXXX\
-XXOOXXX\
--XOOO--\
--X-----\
 X";
/**
	char s[67] = "\
--------\
----X---\
-XOXOO--\
--OXXO--\
--XXXX--\
-XOXXO--\
-----O--\
--------\
 X";
/**
	char s[67] = "\
----O---\
--X-O---\
-XXXOO--\
--XXOOOO\
-XXXXOO-\
-XXXXXXX\
--XXOO--\
--------\
 O";
/**/
/**
	char s[67] = "\
-XXXXXXX\
--XXXXXO\
OXXXXXOO\
-XXXXOXO\
XXXXOXXO\
-XXXXXXO\
---OOOOO\
----O--X\
 X";
/**/
/**
	char s[67] = "\
-------O\
--XXX-OO\
XXXXXOXO\
XXXXOOXO\
XXXXOXXO\
OXOOXXXO\
-OOOXX-O\
-XXXXX--\
 O";
/**/
	char s[130] = //"-XXXX---OX-OXX-OOXXXOOOOOXOXOOOOOXXOXOOOOXXXXXOO-XXXXXXO-XXXXXX- O";//"O--OOOOX-OOOOOOXOOXXOOOXOOXOOOXXOOOOOOXX---OOOOX----O--X-------- X";//"O---X----OOOXX--OOOOXOX-OOXOXXXX-OXXOXX--OOXXOX---X-XXO--XXXXX--";//"OOOOXXXOXOXXXXXOXOXXXXOOXXXOXXOOXOOXOOOOXOXOOOOOXOOOOOOOXOXXXX-- X";//"-OOOOO----OOOOX--OOOOOO-XXXXXOO--XXOOX--OOXOXX----OXXO---OOO--O- X";//"-XXXXXX--XOOOO---OXOOOOOOOOXOXOO-OXOXOOO-OOOXXOX--XXXXOO--XXXX-- X";//"O--OOOOX-OOOOOOXOOXXOOOXOOXOOOXXOOOOOOXX---OOOOX----O--X-------- X";//"--XO-O----OOOO--OOXOXXO-OOOOXXOOOOOXXOX-OXOXXXXX--XXXX----X-O-X- X";/**"O--OOOOX-OOOOOOXOOXXOOOXOOXOOOXXOOOOOOXX---OOOOX----O--X-------- X";/**"-OOOOO----OOOOX--OOOOOO-XXXXXOO--XXOOX--OOXOXX----OXXO---OOO--O- X";/**/

/**
"\
o x x x x x x x \
o x x o o o o o \
o x x x x x o o \
o o o x x o o o \
o o o o o x x o \
x o x o o x x - \
- - o o o o x x \
- - - - o o - x \
x";

/**
"\
- - - - - - - - \
- - - - . . - - \
- - - x o . - - \
- . - x x - . - \
- . o x x x . - \
- . - x x x x - \
- - - - - o . - \
- - - - - - . - \
o";
/**
"\
- - o o o o o - \
- . o x x o o o \
. x x x o o o o \
. x x o o o x o \
. x o x x x x . \
x o o x x x x x \
o o o o o x x . \
o - o - - . x . \
o";

/**
"\
- o o o o o o - \
- . o x x o . o \
- . x o x x o o \
. x x x o x x o \
. . x x o x x . \
- . x x o x x x \
- . . o x x . . \
- - o . o x . - \
o";
/**
"\
- - - - - - - - \
- . . x - - - - \
. o o x x x x - \
- x o o o o x x \
- x o o o o x x \
- x o x o o x o \
- . x o o o o . \
- x x x x x . - \
x";

	//*ucEmpties = 0;
	//vfInitBoard(taBoard);
	/**
	char s[67] = "\
--------\
-----O--\
--XXOX--\
--OOOXO-\
--OOOX--\
--OXOX--\
----OO--\
--------\
 X";
/**
"\
- - - - - - - - \
- - o - - o - - \
- - o o x o - - \
- - x x x x - - \
- - x x x x - - \
- x x o x x o - \
- - - - - - - - \
- - - - - - - - \
x";
 /**/
	
	
 
	//"--XXXXX--OOOXX-O-OOOXXOX-OXOXOXXOXXXOXXX--XOXOXX-XXXOOO--OOOOO-- X";
	//"-XXXXXX---XOOOO--XOXXOOX-OOOOOOOOOOOXXOOOOOXXOOX--XXOO----XXXXX- X";
	//"----OX----OOXX---OOOXX-XOOXXOOOOOXXOXXOOOXXXOOOOOXXXXOXO--OOOOOX X";
	//"-XXXXXX-X-XXXOO-XOXXXOOXXXOXOOOX-OXOOXXX--OOOXXX--OOXX----XOXXO- X";
	//"-OOOOO----OXXO-XXXOXOXX-XXOXOXXOXXOOXOOOXXXXOO-OX-XOOO---XXXXX-- X";
	//"--OXXX--OOOXXX--OOOXOXO-OOXOOOX-OOXXXXXXXOOXXOX--OOOOX---XXXXXX- X";
	//"--OXXO--XOXXXX--XOOOXXXXXOOXXXXXXOOOOXXX-XXXXXXX--XXOOO----XXOO- X";
	//"---X-X--X-XXXX--XXXXOXXXXXXOOOOOXXOXXXO-XOXXXXO-XOOXXX--XOOXXO-- O";
	//"--XOXX--O-OOXXXX-OOOXXXX-XOXXXOXXXOXOOOXOXXOXOXX--OXOO----OOOO-- O";
	//"-XXXX-----OXXX--XOXOXOXXOXOXXOXXOXXOXOOOXXXOXOOX--OXXO---OOOOO-- O";
	//"---O-XOX----XXOX---XXOOXO-XXOXOXXXXOOXOX-XOOXXXXXOOOXX-XOOOOOOO- O";
	//"--O--O--X-OOOOX-XXOOOXOOXXOXOXOOXXOXXOOOXXXXOOOO--OXXX---XXXXX-- O";
	//"--XXXXX--OOOXX---OOOXXXX-OXOXOXXOXXXOXXX--XOXOXX--OXOOO--OOOOO-- X";
	//"--XXXXX---OOOX---XOOXXXX-OOOOOOOOOOXXXOOOOOXXOOX--XXOO----XXXXX- X";
	//"----O------OOX---OOOXX-XOOOXOOOOOXXOXXOOOXXXOOOOOXXXOOXO--OOOOOX X";
	//"-XXXXXX-X-XXXOO-XOXXXOOXXOOXXXOX-OOOXXXX--OOXXXX---OOO----XOX-O- X";
	//"-OOOOO----OXXO-XXXOOOXX-XXOXOXXOXXOOXOOOXXXXOO-OX-XOO----XXXX--- X";
	//"-XXX------OOOX--XOOOOOXXOXOXOOXXOXXOOOOOXXXOXOOX--OXXO---OOOOO-- X";
	//"--OXXO--XOXXXX--XOOOOXXXXOOOXXXXX-OOOXXX--OOOOXX--XXOOO----XXOO- X";

	//"XXXOXXXXOXXXXXXXOOXXXXXXOOOXXXXXOOOXXOO-OOOOO---OOOOOOO-OOOOOOO- X";
	//"OOOOOOOOXOOXXX--XXOOXOO-XOXOOO--XOOOOX--XOOXOO--XOOOOO--XXXX---- O";
	//"--OOOO--X-OOOOO-XXOOXOXXXOXOXXXXXXXOXXXX-XXOXOXX--OXXX-X----X--- O";
	//"--O-------OOX---OOOXXXO-OOOOXOXXXXXOOXOXXXXXXOOXX-XXXXOX--XXXX-- X";
	//"--O--O-----OOOX--X-XOXOO--XXXOOOXXXXOOOOXXXOXXOOXXXXXX--XOXX-O-- O";
	//"----X------XXXO--OOOXXXXXOOOOXXO-XXOOXXOOOXOXXXXOOOXX---X-XXXX-- O";
	//"-OOOOO----OXXO---OOOOXXO-OOOXOXX-OOXOOXX-XOXXOXX--O-XXXX--O----O X";
	//"--XO-O----OOOO--OOXOXXO-OOOOXXOOOOOXXOX-OXOXXXXX--XXXX----X-O-X- X";
	//"--O-------OOO--X-XOOOOXXXXXXOXOX-XXOXOOXXXOXOOXX-OOOOO-X---OOO-- X";
	//"-OXXXX----OXXO--XXOOXOOOXXXOOXOOXXOOXOOOXXXXOO-XX-XXO----------- X";
	//"-XXX----X-XOO---XXOXOO--XOXOXO--XOOXOXXXXOOXXOX---OOOOO--XXXXX-- X";
	//"-OOOOO----OOOO--OXXOOO---XXXOO--XXXXXXO-XXXOOO-OX-OOOO---OOOOO-- X";
	//"--XX----O-XXOX--OOXOO---OXOXOOO-OOXXOOOXOOXXXOOX--XXXXOX--X--X-X X";
	//"-XXXXXXX--XOOO----OXOOXX-OOXXOXX-OOOOOXX-X-XOOXX---O-X-X--OOOO-- X";
	//"-------------O-O-OOOOOOOOOOOOXOOOXXOOOXO-XXXOXOO--XXXOXO--OXXXXO X";
	//"--XXX-----XXXX-OOOXXOOOOOOOOOOXO-OOXXXXO-OOOXXXO---XOXX---X----- O";
	//"---X-O----XXXO-XXXXXXXXXXOOXXOOXXOXOOOXXXXOOOO-XX--OOOO--------- O";
	//"--OOOO--O-OOOO--OXXXOOO-OXXOXO--OOXXOXX-OOXXXX--O-XXX-----XX-O-- X";
	//"--OOOO----OOOO---XOXXOOXOOXOOOOX-OOOOOXXXOOXXXXX--X-X----------- X";
	//"O-OOOO--XOXXOX--XOOOXXX-XOOOXX--XOOXOX--XOXXX---X-XX------------ O";

	"O--OOOOX-OOOOOOXOOXXOOOXOOXOOOXXOOOOOOXX---OOOOX----O--X-------- X";
	//"-OOOOO----OOOOX--OOOOOO-XXXXXOO--XXOOX--OOXOXX----OXXO---OOO--O- X";
	//"--OOO-------XX-OOOOOOXOO-OOOOXOOX-OOOXXO---OOXOO---OOOXO--OOOO-- X";
	//"--XXXXX---XXXX---OOOXX---OOXXXX--OOXXXO-OOOOXOO----XOX----XXXXX- O";
	//"--O-X-O---O-XO-O-OOXXXOOOOOOXXXOOOOOXX--XXOOXO----XXXX-----XXX-- O";
	//"---XXXX-X-XXXO--XXOXOO--XXXOXO--XXOXXO---OXXXOO-O-OOOO------OO-- X";
	//"---XXX----OOOX----OOOXX--OOOOXXX--OOOOXX--OXOXXX--XXOO---XXXX-O- X";
	//"-OOOOO----OOOO---OOOOX--XXXXXX---OXOOX--OOOXOX----OOXX----XXXX-- O";
	//"-----X--X-XXX---XXXXOO--XOXOOXX-XOOXXX--XOOXX-----OOOX---XXXXXX- O";
	//"--OX-O----XXOO--OOOOOXX-OOOOOX--OOOXOXX-OOOOXX-----OOX----X-O--- X";
	//"----X-----XXX----OOOXOOO-OOOXOOO-OXOXOXO-OOXXOOO--OOXO----O--O-- X";
	//"----O-X------X-----XXXO-OXXXXXOO-XXOOXOOXXOXXXOO--OOOO-O----OO-- O";
	//"---X-------OX--X--XOOXXXXXXOXXXXXXXOOXXXXXXOOOXX--XO---X-------- O";
	//"----OO-----OOO---XXXXOOO--XXOOXO-XXXXXOO--OOOXOO--X-OX-O-----X-- X";
	//"--OOO---XXOO----XXXXOOOOXXXXOX--XXXOXX--XXOOO------OOO-----O---- X";
	//"--------X-X------XXXXOOOOOXOXX--OOOXXXX-OOXXXX--O-OOOX-----OO--- O";
	//"--XXXXX---XXXX---OOOXX---OOXOX---OXXXXX-OOOOOXO----OXX---------- O";
	//"-------------------XXOOO--XXXOOO--XXOXOO-OOOXXXO--OXOO-O-OOOOO-- X";
	//"--XOOO----OOO----OOOXOO--OOOOXO--OXOXXX-OOXXXX----X-XX---------- X";
	//"-----------------------O--OOOOO---OOOOOXOOOOXXXX--XXOOXX--XX-O-X X";



/**
"--ooooxo\
oooooxxo\
ooooxxxo\
oxooooxo\
oxxxoxxo\
oxxoxxoo\
o.oxooxo\
xxxxxxxx o";
/**/

	*ucEmpties = 0;
	/**/
	for (char j=0; j<64; j++) {
		u8 x = j&7, y = (j >> 3) & 7, k = BOARD_START + x+BOARD_LINE*y;
		if (s[j]=='-' || s[j]=='.' ) {
			taBoard[k] = EMPTY;
			(*ucEmpties)++;
		}
		else
			if (s[j]=='O' || s[j]=='o') taBoard[k] = WHITE;
			else taBoard[k] = BLACK;
	}
	*tColor = (s[65] == 'X' || s[65] == 'x' || s[65] == '*'? BLACK : WHITE);
	/**
	for (char j=0; j<64; j++) {
		u8 x = j&7, y = (j >> 3) & 7, k = BOARD_START + x+BOARD_LINE*y;
		if ((s[2*j] == '-') || (s[2*j] == '.')) {
			taBoard[k] = EMPTY;
			(*ucEmpties)++;
		}
		else
			if ((s[2*j] == 'O') || (s[2*j] == 'o') || (s[2*j] == '0')) 
				taBoard[k] = WHITE;
			else 
				taBoard[k] = BLACK;
	}
	*tColor = (s[128] == 'X' || s[128] == 'x' || s[128] == '*'? BLACK : WHITE);
	/**/
}

void vfAutoPlay(u8 ucPly, bool bBlack = true)
{
    TBoardCell tOpponentColor = (bBlack ? WHITE : BLACK), tColor = (bBlack ? BLACK : WHITE);
	TBoardCell tBest = 0;
	char cCurrentValue = 0;
	u8 ucEmpties = 60;
	char s[] = "D4";

	vfPrepareForGame();
	
	vfInitBoard(tGlobalSearchParameters.taBoard);

	vfLoadGame(tGame.taBoard,&tColor,&ucEmpties);

    tOpponentColor = (bBlack ? WHITE : BLACK);
	tColor = (bBlack ? BLACK : WHITE);
	
	vfShowBoard(tColor, dStartTime, dStartTime);
	tGame.tColor = tColor;
	tGame.ucPly = ucPly;
	
	dTimeLeft = dStartTime;
	
	for (char i=0, j=0; i<60; i++) {
		
		if (!bfCheckIfAnyAvailable(tColor))
			if (!bfCheckIfAnyAvailable(tOpponentColor))
				break;
			else
				vfDualPrintF("Computer passes...\n");

		else
			if (i > 0 || bBlack) {
				f64 dEval,dTime;
				int    iRow,iColumn;
				memcpy(tGame.taBoard,tGlobalSearchParameters.taBoard,BOARD_SIZE*sizeof(TBoardCell));
				vfGetBestMove(ucEmpties, dTimeLeft, &iRow,&iColumn,&dEval,&dTime);
				tBest = BOARD_START + iRow*BOARD_LINE + iColumn;
				
				ucEmpties--;
				
				if (tColor==WHITE) 
					cCurrentValue *= -1;
				
				bfMakeMove(tGlobalSearchParameters.taBoard,tColor,tBest,&cCurrentValue);
				caNotationBoard[tBest] = ++j;
				
				if (tColor == WHITE) 
					cCurrentValue *= -1;

				vfShowBoard(tOpponentColor, (tColor == BLACK ? dTimeLeft : dStartTime), (tColor == WHITE ? dTimeLeft : dStartTime));
			}
		
		if (!bfCheckIfAnyAvailable(tOpponentColor))
			if (!bfCheckIfAnyAvailable(tColor))
				break;
			else {
				vfDualPrintF("You have to pass...");
				while (!_kbhit()) ;
				vfDualPrintF("\n");
			}
		else
			do {
				vfDualPrintF("Virtual Othello> ");
				char caTestMoves[31][3] = {
				/**
					"f4",
					"c4",
					"d6",
					"c6",
					"g5",
					"e3",
					"g6",
					"g4",
					"f2",
					"h4",
					"d2",
					"b5",
					"b3",
					"b6",
					"a6",
					"a4",
					"c1",
					"d7",
					"c8",
					"c7",
					"h5",
					"g7",
					"e1",
					"h8",
					"--",
					"--",
					"--",
					"--",
					"--",
					"--",
					"--",
					};
				/**
					"f4",
					"c4",
					"d6",
					"c2",
					"d2",
					"b4",
					"e2",
					"f1",
					"f2",
					"a3",
					"g2",
					"c5",
					"d1",
					"a1",
					"b5",
					"g4",
					"a6",
					"b6",
					"d7",
					"a8",
					"b8",
					"e7",
					"c8",
					"e8",
					"--",
					"--",
					"--",
					"--",
					"--",
					"--",
					"--",
					};
				/**/
					"f4",
					"c4",
					"d6",
					"c6",
					"g5",
					"e3",
					"f7",
					"h4",
					"f3",
					"d2",
					"g3",
					"f8",
					"b4",
					"b3",
					"a4",
					"c1",
					"g1",
					"d7",
					"d8",
					"g7",
					"b6",
					"a8",
					"h7",
					"e1",
					"--",
					"--",
					"--",
					"--",
					"--",
					"--",
					"--",
					};
				s[0] = caTestMoves[i][0];
				s[1] = caTestMoves[i][1];

				vfDualPrintF("%s\n%d\n",s,ucEmpties - 1);
				
				if (bfCheckIfAvailable(tOpponentColor,tfMoveToIndex(s))) {
					
					if (tOpponentColor==WHITE) 
						cCurrentValue *= -1;
					
					bfMakeMove(tGlobalSearchParameters.taBoard,tOpponentColor,tfMoveToIndex(s),&cCurrentValue);
					
					if (tOpponentColor==WHITE) 
						cCurrentValue *= -1;
					
					caNotationBoard[tfMoveToIndex(s)] = ++j;
					vfShowBoard(tGame.tColor, (tColor == BLACK ? dTimeLeft : dStartTime), (tColor == WHITE ? dTimeLeft : dStartTime));
					ucEmpties--;
					break;
				}
				vfDualPrintF("Invalid move!\n");
				return;
			}
			while (true);
	}

	cCurrentValue = cfGetDiskDifference();
	cCurrentValue *= !bBlack ? -1 : 1;
	if (ucEmpties)
		if (cCurrentValue > 0)
			cCurrentValue += ucEmpties;
		else
			cCurrentValue -= ucEmpties;

	vfDualPrintF("Game is over : %3d\n",(cCurrentValue>0 ? cCurrentValue + ucEmpties : cCurrentValue - ucEmpties));
	while (!_kbhit()) ;
}

void vfPlayGame(bool bBlack = true)
{
	TBoardCell tBest = 0;
	char cCurrentValue = 0;
	u8 ucEmpties = 60;
	char s[] = "D4";

	vfPrepareForGame();
	
	vfInitBoard(tGlobalSearchParameters.taBoard);

    TBoardCell tOpponentColor = (bBlack ? WHITE : BLACK);
	TBoardCell tColor = (bBlack ? BLACK : WHITE);
	TBoardCell tPlayerColor = BLACK;
	
	//vfLoadGame(tGlobalSearchParameters.taBoard,&tPlayerColor,&ucEmpties);

	//FILE *fBug = fopen("hash.bug","rb");
	//fread(taHash,1,sizeof(taHash),fBug);
	//fclose(fBug);

	vfShowBoard(tPlayerColor, dStartTime, dStartTime);

	tGame.tColor = tColor;
	//tGame.ucPly = ucPly;
	
	dTimeLeft = dStartTime;
	
	for (char i=0, j=0; i<120; i++) {
		
		if (tPlayerColor == tColor) {
			if (!bfCheckIfAnyAvailable(tColor))
				if (!bfCheckIfAnyAvailable(tOpponentColor))
					break;
				else
					vfDualPrintF("Computer passes...\n");

			else {
				f64 dEval,dTime;
				int    iRow,iColumn;
				memcpy(tGame.taBoard,tGlobalSearchParameters.taBoard,BOARD_SIZE*sizeof(TBoardCell));
				vfGetBestMove(ucEmpties, dTimeLeft, &iRow,&iColumn,&dEval,&dTime);
				memcpy(tGlobalSearchParameters.taBoard,tGame.taBoard,BOARD_SIZE*sizeof(TBoardCell));
				tBest = BOARD_START + iRow*BOARD_LINE + iColumn;
				
				ucEmpties--;
				
				if (tColor==WHITE) 
					cCurrentValue *= -1;
				
				bfMakeMove(tGlobalSearchParameters.taBoard,tColor,tBest,&cCurrentValue);
				caNotationBoard[tBest] = ++j;
				
				if (tColor == WHITE) 
					cCurrentValue *= -1;

				vfShowBoard(tOpponentColor, (tColor == BLACK ? dTimeLeft : dStartTime), (tColor == WHITE ? dTimeLeft : dStartTime));
			}
			tPlayerColor = tOpponentColor;
		}
		else {
			if (!bfCheckIfAnyAvailable(tOpponentColor))
				if (!bfCheckIfAnyAvailable(tColor))
					break;
				else {
					vfDualPrintF("You have to pass...");
					while (!_kbhit()) ;
					vfDualPrintF("\n");
				}
			else
				do {
					vfDualPrintF("Virtual Othello> ");
					
					if (ucEmpties < 69) {
						scanf("%s",s);
					}
					else {
						switch (ucEmpties) {
							case 59 : 
								memcpy(s,"f4",2*sizeof(char));
								break;
							case 57 : 
								memcpy(s,"d6",2*sizeof(char));
								break;
							case 55 : 
								memcpy(s,"f3",2*sizeof(char));
								break;
							case 53 : 
								memcpy(s,"f6",2*sizeof(char));
								break;
							case 51 : 
								memcpy(s,"g4",2*sizeof(char));
								break;
							case 49 : 
								memcpy(s,"d3",2*sizeof(char));
								break;
						}
					}
#ifdef WRITE_LOG
					fprintf(fVOOutput,"%s\n",s);
#endif
					
					if (bfCheckIfAvailable(tOpponentColor,tfMoveToIndex(s))) {
						
						if (tOpponentColor==WHITE) 
							cCurrentValue *= -1;
						
						bfMakeMove(tGlobalSearchParameters.taBoard,tOpponentColor,tfMoveToIndex(s),&cCurrentValue);
						
						if (tOpponentColor==WHITE) 
							cCurrentValue *= -1;
						
						caNotationBoard[tfMoveToIndex(s)] = ++j;

						vfShowBoard(tColor, (tColor == BLACK ? dTimeLeft : dStartTime), (tColor == WHITE ? dTimeLeft : dStartTime));
						ucEmpties--;
						break;
					}
					vfDualPrintF("Invalid move!\n");
				}
				while (true);
				tPlayerColor = tColor;
		}
	}

	cCurrentValue = cfGetDiskDifference();
	cCurrentValue *= !bBlack ? -1 : 1;
	if (ucEmpties)
		if (cCurrentValue > 0)
			cCurrentValue += ucEmpties;
		else
			cCurrentValue -= ucEmpties;

	vfDualPrintF("Game is over : %3d\n",(cCurrentValue>0 ? cCurrentValue + ucEmpties : cCurrentValue - ucEmpties));
	while (!_kbhit()) ;
}

void vfLoadGame(TBoardCell *taBoard, TBoardCell *tColor, u8 *ucEmpties, char *s)
{
	vfPrepareForGame();
	
	vfInitBoard(taBoard);

	*ucEmpties = 0;
	for (char j=0; j<64; j++) {
		u8 x = j&7, y = (j >> 3) & 7, k = BOARD_START + x+BOARD_LINE*y;
		if (s[j]=='-' || s[j]=='.' ) {
			taBoard[k] = EMPTY;
			(*ucEmpties)++;
		}
		else
			if (s[j]=='O' || s[j]=='o') taBoard[k] = WHITE;
			else taBoard[k] = BLACK;
	}
	*tColor = (s[65] == 'X' || s[65] == 'x' || s[65] == '*'? BLACK : WHITE);
}

