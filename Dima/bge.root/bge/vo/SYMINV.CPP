//////////////////////////////////////////////////////////////////////////////////
//	Module			:	evaluation function features symmetries and inversions	//
//	File			:	syminv.cpp												//
//	Creation date	:	26.07.2001												//
//	Author			:	Dmitriy Iassenev										//
//////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "board.h"
#include "eval.h"
#include "syminv.h"

#define MAX_CHARS 100

int *ipaConversionArray[8], *ipInversions, iVariableCount = 0, iSymInvCount = 0;

char *cpfMakeSymmetricReflection(char *s1, int n, int symCenter)
{
	static char s[MAX_CHARS];
	for (int i=0; i<n; i++)
		if (!symCenter || ((i<symCenter) || (i > n - symCenter - 1)))
			s[i] = s1[n-i-1];
		else
			s[i] = s1[i];
	return(s);
}

char *cpfMakeInversion(char *s1, int n)
{
	static char s[MAX_CHARS];
	for (int i=0; i<n; i++)
		s[i] = (s1[i] == BLACK ? WHITE : (s1[i] == WHITE ? BLACK : s1[i]));
	return(s);
}

int ifConvertStringToNumber(char *s, int n)
{
	int result = 0;
	for (int i=0; i<n; i++)
		result = result*3 + s[i];
	return(result);
}

int ifCheckForPreviousSymmetries(char *s,int n, bool sym, char symCenter)
{
	int nn = ifConvertStringToNumber(s,n), n1 = ifConvertStringToNumber(cpfMakeSymmetricReflection(s,n,symCenter),n);
	if (sym && (n1 < nn))
		return(n1);
	else
		return(iVariableCount);
}

void vfGenerateNonSymmetricIndexes(char *s, int n, int nn, int j, bool sym, char symCenter)
{
	for (char i=0; i<3; i++) {
		s[j] = i;
		if (j<n-1)
			vfGenerateNonSymmetricIndexes(s,n,nn,j+1,sym,symCenter);
		else {
			int a = ifCheckForPreviousSymmetries(s,n,sym,symCenter);
			ipaConversionArray[nn][iVariableCount] = (a < iVariableCount ? ipaConversionArray[nn][a] : iSymInvCount);
			iSymInvCount += (a < iVariableCount ? 0 : 1);
			iVariableCount++;
		}
	}
}

int ifCheckForPreviousInversions(char *s, int n, int _n)
{
	int nn = ipaConversionArray[_n][ifConvertStringToNumber(s,n)], n1 = ipaConversionArray[_n][ifConvertStringToNumber(cpfMakeInversion(s,n),n)];
	if (n1<nn)
		return(n1);
	else
		return(iSymInvCount);
}

void vfGenerateNonInversionIndexes(char *s, int n, int j, int _n, int nn)
{
	for (char i=0; i<3; i++) {
		s[j] = i;
		if (j<n-1)
			vfGenerateNonInversionIndexes(s,n,j+1,_n,nn);
		else {
			if (ipaConversionArray[_n][iVariableCount] == iSymInvCount) {
				int a = ifCheckForPreviousInversions(s,n,_n);
				ipInversions[nn + iSymInvCount] = (a < iSymInvCount ? nn + a : nn + iSymInvCount);
				iSymInvCount++;
			}
			iVariableCount++;
		}
	}
}

void vfInitConversionArray()
{
	for (int i=0; i<7; i++)
		ipaConversionArray[i] = (int *)calloc(dwaPower[i+4],sizeof(int));
	
	ipaConversionArray[7] = (int *)calloc(dwaPower[10],sizeof(int));

	char s[MAX_CHARS];

	for ( i=4; i<11; i++) {
		memset(s,0,100);
		iVariableCount = 0;
		iSymInvCount = 0;
		if (i<9)
			vfGenerateNonSymmetricIndexes(s,i,i-4,0,true,0);
		else
			if (i==9)
				vfGenerateNonSymmetricIndexes(s,i,i-4,0,true,3);
			else {
				vfGenerateNonSymmetricIndexes(s,i,i-4,0,false,0);
				memset(s,0,100);
				iVariableCount = 0;
				iSymInvCount = 0;
				vfGenerateNonSymmetricIndexes(s,i,i-3,0,true,0);
			}
	}
	
	ipInversions = (int *)calloc(PAR_COUNT,sizeof(int));
	for ( i=0; i<PAR_COUNT; i++)
		ipInversions[i] = i;
	
	iVariableCount = 0; 
	iSymInvCount = 0;
	memset(s,0,100);
	vfGenerateNonInversionIndexes(s,4 ,0,0,0);
	iVariableCount = 0; 
	iSymInvCount = 0;
	memset(s,0,100);
	vfGenerateNonInversionIndexes(s,5 ,0,1,imPW_X(4));
	iVariableCount = 0; 
	iSymInvCount = 0;
	memset(s,0,100);
	vfGenerateNonInversionIndexes(s,6 ,0,2,imPW_X(4) + imPW_X(5));
	iVariableCount = 0; 
	iSymInvCount = 0;
	memset(s,0,100);
	vfGenerateNonInversionIndexes(s,7 ,0,3,imPW_X(4) + imPW_X(5) + imPW_X(6));
	iVariableCount = 0; 
	iSymInvCount = 0;
	memset(s,0,100);
	vfGenerateNonInversionIndexes(s,8 ,0,4,imPW_X(4) + imPW_X(5) + imPW_X(6) + imPW_X(7));
	iVariableCount = 0; 
	iSymInvCount = 0;
	memset(s,0,100);
	vfGenerateNonInversionIndexes(s,8 ,0,4,imPW_X(4) + imPW_X(5) + imPW_X(6) + imPW_X(7) + 1*imPW_X(8));
	iVariableCount = 0; 
	iSymInvCount = 0;
	memset(s,0,100);
	vfGenerateNonInversionIndexes(s,8 ,0,4,imPW_X(4) + imPW_X(5) + imPW_X(6) + imPW_X(7) + 2*imPW_X(8));
	iVariableCount = 0; 
	iSymInvCount = 0;
	memset(s,0,100);
	vfGenerateNonInversionIndexes(s,8 ,0,4,imPW_X(4) + imPW_X(5) + imPW_X(6) + imPW_X(7) + 3*imPW_X(8));
	iVariableCount = 0; 
	iSymInvCount = 0;
	memset(s,0,100);
	vfGenerateNonInversionIndexes(s,9 ,0,5,imPW_X(4) + imPW_X(5) + imPW_X(6) + imPW_X(7) + 4*imPW_X(8));
	iVariableCount = 0; 
	iSymInvCount = 0;
	memset(s,0,100);
	vfGenerateNonInversionIndexes(s,10,0,6,imPW_X(4) + imPW_X(5) + imPW_X(6) + imPW_X(7) + 4*imPW_X(8) + imPW_X(90));
	iVariableCount = 0; 
	iSymInvCount = 0;
	memset(s,0,100);
	vfGenerateNonInversionIndexes(s,10,0,7,imPW_X(4) + imPW_X(5) + imPW_X(6) + imPW_X(7) + 4*imPW_X(8) + imPW_X(90) + imPW_X(100));
}

void vfFreeConversionArray()
{
	for (int i=0; i<8; i++)
		free(ipaConversionArray[i]);
	free(ipInversions);
}