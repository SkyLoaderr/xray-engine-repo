//////////////////////////////////////////////////////////////////////////////////
//	Module			:	evaluation function										//
//	File			:	eval.cpp												//
//	Creation date	:	04.04.2001												//
//	Author			:	Dmitriy Iassenev										//
//////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "syminv.h"
#include "eval.h"
#include "mobil.h"
#include "misc.h"

#define PAR_COUNT_X		113879

const u32	dwaPower[12]	= { 1, 3, 3*3, 3*3*3, 3*3*3*3, 3*3*3*3*3, 3*3*3*3*3*3, 3*3*3*3*3*3*3, 3*3*3*3*3*3*3*3, 3*3*3*3*3*3*3*3*3, 3*3*3*3*3*3*3*3*3*3, 3*3*3*3*3*3*3*3*3*3*3 };
const u32	dwaPatt	[12]	= { 
	0,
	dwaPower[4],
	dwaPower[4] + dwaPower[5],
	dwaPower[4] + dwaPower[5] + dwaPower[6],
	dwaPower[4] + dwaPower[5] + dwaPower[6] + dwaPower[7],
	dwaPower[4] + dwaPower[5] + dwaPower[6] + dwaPower[7] + dwaPower[8],
	dwaPower[4] + dwaPower[5] + dwaPower[6] + dwaPower[7] + 2*dwaPower[8],
	dwaPower[4] + dwaPower[5] + dwaPower[6] + dwaPower[7] + 3*dwaPower[8],
	dwaPower[4] + dwaPower[5] + dwaPower[6] + dwaPower[7] + 4*dwaPower[8],
	dwaPower[4] + dwaPower[5] + dwaPower[6] + dwaPower[7] + 4*dwaPower[8] + dwaPower[9],
	dwaPower[4] + dwaPower[5] + dwaPower[6] + dwaPower[7] + 4*dwaPower[8] + dwaPower[9] + dwaPower[10],
	dwaPower[4] + dwaPower[5] + dwaPower[6] + dwaPower[7] + 4*dwaPower[8] + dwaPower[9] + dwaPower[10] + dwaPower[10],
};

const u32	dwaCompressedPatt[12] = { 
	0,
	imPW_X(4),
	imPW_X(4) + imPW_X(5),
	imPW_X(4) + imPW_X(5) + imPW_X(6),
	imPW_X(4) + imPW_X(5) + imPW_X(6) + imPW_X(7),
	imPW_X(4) + imPW_X(5) + imPW_X(6) + imPW_X(7) + imPW_X(8),
	imPW_X(4) + imPW_X(5) + imPW_X(6) + imPW_X(7) + 2*imPW_X(8),
	imPW_X(4) + imPW_X(5) + imPW_X(6) + imPW_X(7) + 3*imPW_X(8),
	imPW_X(4) + imPW_X(5) + imPW_X(6) + imPW_X(7) + 4*imPW_X(8),
	imPW_X(4) + imPW_X(5) + imPW_X(6) + imPW_X(7) + 4*imPW_X(8) + imPW_X(90),
	imPW_X(4) + imPW_X(5) + imPW_X(6) + imPW_X(7) + 4*imPW_X(8) + imPW_X(90) + imPW_X(100),
	imPW_X(4) + imPW_X(5) + imPW_X(6) + imPW_X(7) + 4*imPW_X(8) + imPW_X(90) + imPW_X(100) + imPW_X(10)
};

SFeature taFullParameters[STAGE_COUNT][PAR_COUNT];
SFeature *taSymParameters;
SFeature *tpShortWay;

#ifndef __MPI__
SFeature tfEvaluate(const u32 cParity, const u32 cGameStage)
{
	SFeature tResult = 0;

	tpShortWay = taFullParameters[cGameStage];
	tResult += tpShortWay[_B4(37,29,21,13)]; 
	tResult += tpShortWay[_B4(14,24,34,44)]; 
	tResult += tpShortWay[_B4(53,61,69,77)]; 
	tResult += tpShortWay[_B4(76,66,56,46)]; 
		
	tpShortWay += 81;
	tResult += tpShortWay[_B5(46,38,30,22,14)]; 
	tResult += tpShortWay[_B5(13,23,33,43,53)]; 
	tResult += tpShortWay[_B5(44,52,60,68,76)]; 
	tResult += tpShortWay[_B5(77,67,57,47,37)]; 
		
	tpShortWay += 243;
	tResult += tpShortWay[_B6(55,47,39,31,23,15)]; 
	tResult += tpShortWay[_B6(12,22,32,42,52,62)]; 
	tResult += tpShortWay[_B6(35,43,51,59,67,75)]; 
	tResult += tpShortWay[_B6(78,68,58,48,38,28)]; 
		
	tpShortWay += 729;
	tResult += tpShortWay[_B7(64,56,48,40,32,24,16)]; 
	tResult += tpShortWay[_B7(11,21,31,41,51,61,71)]; 
	tResult += tpShortWay[_B7(26,34,42,50,58,66,74)]; 
	tResult += tpShortWay[_B7(79,69,59,49,39,29,19)]; 
		
	tpShortWay += 2187;
	tResult += tpShortWay[_B8(73,65,57,49,41,33,25,17)]; 
	tResult += tpShortWay[_B8(80,70,60,50,40,30,20,10)]; 
			
	tpShortWay += 6561;
	tResult += tpShortWay[_B8(19,20,21,22,23,24,25,26)]; 
	tResult += tpShortWay[_B8(79,70,61,52,43,34,25,16)]; 
	tResult += tpShortWay[_B8(71,70,69,68,67,66,65,64)]; 
	tResult += tpShortWay[_B8(11,20,29,38,47,56,65,74)]; 
				
	tpShortWay += 6561;
	tResult += tpShortWay[_B8(28,29,30,31,32,33,34,35)]; 
	tResult += tpShortWay[_B8(78,69,60,51,42,33,24,15)]; 
	tResult += tpShortWay[_B8(62,61,60,59,58,57,56,55)]; 
	tResult += tpShortWay[_B8(12,21,30,39,48,57,66,75)]; 
				
	tpShortWay += 6561;
	tResult += tpShortWay[_B8(37,38,39,40,41,42,43,44)]; 
	tResult += tpShortWay[_B8(77,68,59,50,41,32,23,14)]; 
	tResult += tpShortWay[_B8(53,52,51,50,49,48,47,46)]; 
	tResult += tpShortWay[_B8(13,22,31,40,49,58,67,76)]; 
			
	tpShortWay += 6561;
	tResult += tpShortWay[_B9(10,11,12,19,20,21,28,29,30)]; 
	tResult += tpShortWay[_B9(17,16,15,26,25,24,35,34,33)]; 
	tResult += tpShortWay[_B9(80,79,78,71,70,69,62,61,60)]; 
	tResult += tpShortWay[_B9(73,74,75,64,65,66,55,56,57)]; 
		
	tpShortWay += 19683;
	tResult += tpShortWay[_B10(10,11,12,13,14,19,20,21,22,23)]; 
	tResult += tpShortWay[_B10(10,19,28,37,46,11,20,29,38,47)]; 
	tResult += tpShortWay[_B10(17,16,15,14,13,26,25,24,23,22)]; 
	tResult += tpShortWay[_B10(17,26,35,44,53,16,25,34,43,52)]; 
	tResult += tpShortWay[_B10(80,79,78,77,76,71,70,69,68,67)]; 
	tResult += tpShortWay[_B10(80,71,62,53,44,79,70,61,52,43)]; 
	tResult += tpShortWay[_B10(73,74,75,76,77,64,65,66,67,68)]; 
	tResult += tpShortWay[_B10(73,64,55,46,37,74,65,56,47,38)]; 

	tpShortWay += 59049;
	tResult += tpShortWay[_B10(10,11,12,13,20,25,14,15,16,17)]; 
	tResult += tpShortWay[_B10(80,71,62,53,70,25,44,35,26,17)]; 
	tResult += tpShortWay[_B10(80,79,78,77,70,65,76,75,74,73)]; 
	tResult += tpShortWay[_B10(10,19,28,37,20,65,46,55,64,73)]; 

	return(tResult + (tpShortWay + 59049)[cParity]);
}
#else
	#define A00 0
	#define A01 0
	#define A02 0
	#define A03 0
	#define A04 0
	#define A05 0
	#define A06 0
	#define A07 0
	#define A08 0
	#define A09 (3*3*3*3 + 3*3*3*3*3 + 3*3*3*3*3*3 + 3*3*3*3*3*3*3 + 4*3*3*3*3*3*3*3*3 + 3*3*3*3*3*3*3*3*3)
	#define A10 (3*3*3*3 + 3*3*3*3*3 + 3*3*3*3*3*3 + 3*3*3*3*3*3*3 + 4*3*3*3*3*3*3*3*3 + 3*3*3*3*3*3*3*3*3 + 3*3*3*3*3*3*3*3*3*3)
	#define A11 (3*3*3*3 + 3*3*3*3*3 + 3*3*3*3*3*3 + 3*3*3*3*3*3*3 + 4*3*3*3*3*3*3*3*3 + 3*3*3*3*3*3*3*3*3 + 3*3*3*3*3*3*3*3*3*3 + 3*3*3*3*3*3*3*3*3*3)

	#pragma warning(disable:4035)
	__declspec(naked) SFeature __fastcall tfEvaluate(const u32 dwParity, const u32 dwGameStage)
	{
		__asm {
			push	ebx;
			push	esi;
			push	edi;
			//mov		edx, [dwGameStage];
			//mov		ecx, [dwParity];
			imul	edx, PAR_COUNT*2;
			add		edx, OFFSET taFullParameters;
			xor		eax, eax;
			add		ax,  WORD PTR  [edx + A11*2ul + ecx*2ul];
			mov		ecx, OFFSET waMPIndexes;
			xor		esi, esi
			xor		ebx, ebx
			xor		edi, edi
				
			mov		si,  WORD PTR [ecx+ 0*2];
			mov		bx,  WORD PTR [ecx+ 1*2];
			mov		di,  WORD PTR [ecx+ 2*2];
			add		ax,  WORD PTR [edx + esi*2ul + A10*2ul];
			add		ax,  WORD PTR [edx + ebx*2ul + A05*2ul];
			add		ax,  WORD PTR [edx + edi*2ul + A06*2ul];

			mov		si,  WORD PTR [ecx+ 3*2];
			mov		bx,  WORD PTR [ecx+ 4*2];
			mov		di,  WORD PTR [ecx+ 5*2];
			add		ax,  WORD PTR [edx + esi*2ul + A07*2ul];
			add		ax,  WORD PTR [edx + ebx*2ul + A07*2ul];
			add		ax,  WORD PTR [edx + edi*2ul + A06*2ul];

			mov		si,  WORD PTR [ecx+ 6*2];
			mov		bx,  WORD PTR [ecx+ 7*2];
			mov		di,  WORD PTR [ecx+ 8*2];
			add		ax,  WORD PTR [edx + esi*2ul + A05*2ul];
			add		ax,  WORD PTR [edx + ebx*2ul + A10*2ul];
			add		ax,  WORD PTR [edx + edi*2ul + A10*2ul];

			mov		si,  WORD PTR [ecx+ 9*2];
			mov		bx,  WORD PTR [ecx+10*2];
			mov		di,  WORD PTR [ecx+11*2];
			add		ax,  WORD PTR [edx + esi*2ul + A05*2ul];
			add		ax,  WORD PTR [edx + ebx*2ul + A06*2ul];
			add		ax,  WORD PTR [edx + edi*2ul + A07*2ul];

			mov		si,  WORD PTR [ecx+12*2];
			mov		bx,  WORD PTR [ecx+13*2];
			mov		di,  WORD PTR [ecx+14*2];
			add		ax,  WORD PTR [edx + esi*2ul + A07*2ul];
			add		ax,  WORD PTR [edx + ebx*2ul + A06*2ul];
			add		ax,  WORD PTR [edx + edi*2ul + A05*2ul];

			mov		si,  WORD PTR [ecx+15*2];
			mov		bx,  WORD PTR [ecx+16*2];
			mov		di,  WORD PTR [ecx+17*2];
			add		ax,  WORD PTR [edx + esi*2ul + A10*2ul];
			add		ax,  WORD PTR [edx + ebx*2ul + A04*2ul];
			add		ax,  WORD PTR [edx + edi*2ul + A03*2ul];

			mov		si,  WORD PTR [ecx+18*2];
			mov		bx,  WORD PTR [ecx+19*2];
			mov		di,  WORD PTR [ecx+20*2];
			add		ax,  WORD PTR [edx + esi*2ul + A03*2ul];
			add		ax,  WORD PTR [edx + ebx*2ul + A02*2ul];
			add		ax,  WORD PTR [edx + edi*2ul + A02*2ul];

			mov		si,  WORD PTR [ecx+21*2];
			mov		bx,  WORD PTR [ecx+22*2];
			mov		di,  WORD PTR [ecx+23*2];
			add		ax,  WORD PTR [edx + esi*2ul + A01*2ul];
			add		ax,  WORD PTR [edx + ebx*2ul + A01*2ul];
			add		ax,  WORD PTR [edx + edi*2ul + A00*2ul];

			mov		si,  WORD PTR [ecx+24*2];
			mov		bx,  WORD PTR [ecx+25*2];
			mov		di,  WORD PTR [ecx+26*2];
			add		ax,  WORD PTR [edx + esi*2ul + A00*2ul];
			add		ax,  WORD PTR [edx + ebx*2ul + A04*2ul];
			add		ax,  WORD PTR [edx + edi*2ul + A03*2ul];

			mov		si,  WORD PTR [ecx+27*2];
			mov		bx,  WORD PTR [ecx+28*2];
			mov		di,  WORD PTR [ecx+29*2];
			add		ax,  WORD PTR [edx + esi*2ul + A03*2ul];
			add		ax,  WORD PTR [edx + ebx*2ul + A02*2ul];
			add		ax,  WORD PTR [edx + edi*2ul + A02*2ul];

			mov		si,  WORD PTR [ecx+30*2];
			mov		bx,  WORD PTR [ecx+31*2];
			mov		di,  WORD PTR [ecx+32*2];
			add		ax,  WORD PTR [edx + esi*2ul + A01*2ul];
			add		ax,  WORD PTR [edx + ebx*2ul + A01*2ul];
			add		ax,  WORD PTR [edx + edi*2ul + A00*2ul];

			mov		si,  WORD PTR [ecx+33*2];
			mov		bx,  WORD PTR [ecx+34*2];
			mov		di,  WORD PTR [ecx+35*2];
			add		ax,  WORD PTR [edx + esi*2ul + A00*2ul];
			add		ax,  WORD PTR [edx + ebx*2ul + A08*2ul];
			add		ax,  WORD PTR [edx + edi*2ul + A08*2ul];

			mov		si,  WORD PTR [ecx+36*2];
			mov		bx,  WORD PTR [ecx+37*2];
			mov		di,  WORD PTR [ecx+38*2];
			add		ax,  WORD PTR [edx + esi*2ul + A08*2ul];
			add		ax,  WORD PTR [edx + ebx*2ul + A08*2ul];
			add		ax,  WORD PTR [edx + edi*2ul + A09*2ul];

			mov		si,  WORD PTR [ecx+39*2];
			mov		bx,  WORD PTR [ecx+40*2];
			mov		di,  WORD PTR [ecx+41*2];
			add		ax,  WORD PTR [edx + esi*2ul + A09*2ul];
			add		ax,  WORD PTR [edx + ebx*2ul + A09*2ul];
			add		ax,  WORD PTR [edx + edi*2ul + A09*2ul];

			mov		si,  WORD PTR [ecx+42*2];
			mov		bx,  WORD PTR [ecx+43*2];
			mov		di,  WORD PTR [ecx+44*2];
			add		ax,  WORD PTR [edx + esi*2ul + A09*2ul];
			add		ax,  WORD PTR [edx + ebx*2ul + A09*2ul];
			add		ax,  WORD PTR [edx + edi*2ul + A09*2ul];

			mov		si,  WORD PTR [ecx+45*2];
			add		ax,  WORD PTR [edx + esi*2ul + A09*2ul];
			
			pop		edi;
			pop		esi;
			pop		ebx;

			ret		0;
		}
	}
	#pragma warning(default:4035)
#endif

void vfLoadEFParameters()
{
	vfDualPrintF("Loading evaluation function pattern configuration weights...");
	
	vfInitConversionArray();
	
	taSymParameters = (SFeature *)malloc(PAR_COUNT_X*sizeof(SFeature));
	
	FILE *fTmp = fopen("patterns\\patterns","rb");;
	
	for (int ii=0; ii<STAGE_COUNT; ii++) {
		
		if (!ii) {
			memset(taFullParameters[0],0,PAR_COUNT*sizeof(SFeature));
			continue;
		}
		fread(taSymParameters,sizeof(SFeature),PAR_COUNT_X,fTmp);
		for (int i=4; i<11; i++)
			for (int j=0; (u32)j<dwaPower[i]; j++)
				switch (i) {
					case 4  : {
						taFullParameters[ii][dwaPatt[0] + j] = taSymParameters[dwaCompressedPatt[0] + ipaConversionArray[0][j]];
						break;
					}
					case 5  : {
						taFullParameters[ii][dwaPatt[1] + j] = taSymParameters[dwaCompressedPatt[1] + ipaConversionArray[1][j]];
						break;
					}
					case 6  : {
						taFullParameters[ii][dwaPatt[2] + j] = taSymParameters[dwaCompressedPatt[2] + ipaConversionArray[2][j]];
						break;
					}
					case 7  : {
						taFullParameters[ii][dwaPatt[3] + j] = taSymParameters[dwaCompressedPatt[3] + ipaConversionArray[3][j]];
						break;
					}
					case 8  : {
						taFullParameters[ii][dwaPatt[4] + j] = taSymParameters[dwaCompressedPatt[4] + ipaConversionArray[4][j]];
						taFullParameters[ii][dwaPatt[5] + j] = taSymParameters[dwaCompressedPatt[5] + ipaConversionArray[4][j]];
						taFullParameters[ii][dwaPatt[6] + j] = taSymParameters[dwaCompressedPatt[6] + ipaConversionArray[4][j]];
						taFullParameters[ii][dwaPatt[7] + j] = taSymParameters[dwaCompressedPatt[7] + ipaConversionArray[4][j]];
						break;
					}
					case 9  : {
						taFullParameters[ii][dwaPatt[8]+j] = taSymParameters[dwaCompressedPatt[8] + ipaConversionArray[5][j]];
						break;
					}
					case 10 : {
						taFullParameters[ii][dwaPatt[9]  + j] = taSymParameters[dwaCompressedPatt[9]  + ipaConversionArray[6][j]];
						taFullParameters[ii][dwaPatt[10] + j] = taSymParameters[dwaCompressedPatt[10] + ipaConversionArray[7][j]];
						break;
					}
				}
		
		if (!(ii % 2))
			taFullParameters[ii][PAR_COUNT - 1] = taFullParameters[ii][PAR_COUNT - 2] = taSymParameters[PAR_COUNT_X - 2];
		else
			taFullParameters[ii][PAR_COUNT - 1] = taFullParameters[ii][PAR_COUNT - 2] = taSymParameters[PAR_COUNT_X - 1];
	}
	fclose(fTmp);
	free(taSymParameters);
	vfFreeConversionArray();
	vfDualPrintF("completed\n  61 stage x %d weights x symmetries = %.1fMb (%d bytes)\n",PAR_COUNT_X,sizeof(taFullParameters)/1048576.0,sizeof(taFullParameters));
}