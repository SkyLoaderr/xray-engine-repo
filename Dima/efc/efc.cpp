////////////////////////////////////////////////////////////////////////////
//	Module 		: efc.cpp
//	Created 	: 20.03.2002
//  Modified 	: 09.10.2002
//	Author		: Dmitriy Iassenev
//	Description : Evaluation Function Constructor
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "fitter.h"
#include "misc.h"	   

#include <string.h>	   

char caLogData[260];
char caTextData[260];
char caBinaryData[260];
char caConfigData[260];
char caPatternData[260];
char caCoreData[260];
char caParametersData[260];
char caEFData[260];

void vfShowHelpScreen()
{
	vfDualPrintF(" Available switches :\n");
	vfDualPrintF(" -p name  obligatory switch with project name\n");
	vfDualPrintF(" -pa      perform operations for all projects\n\n");
	vfDualPrintF(" -c[d]    convert text to binary data and generate initial patterns\n");
	vfDualPrintF("    d     show duplicates being found in text data\n\n");
	vfDualPrintF(" -g{c,p[s],b}\n");
	vfDualPrintF("   -gc    generate configurations from atomic features based on test data\n");
	vfDualPrintF("   -gp    generate patterns from configurations being generated\n");
	vfDualPrintF("   -gps   show all configurations during generation\n");
	vfDualPrintF("   -gb    generate pattern basis from patterns being generated\n\n");
	vfDualPrintF(" -f[p,u]  fit weights of pattern configurations\n");
	vfDualPrintF("    p     force to use probabilistic weight fitting algorithm\n");
	vfDualPrintF("    u     force to use previous parameters if exist\n\n");
	vfDualPrintF(" -l[a]    list stats on test data\n");
	vfDualPrintF("    a     list stats on all the valid data\n");
	vfDualPrintF(" -s       list sorted stats on test data\n");
	vfDualPrintF(" -w       list pattern configuration weights\n\n");
	vfDualPrintF(" -b[f]    build evaluation function\n");
	vfDualPrintF("    f     save parameters in float (default is double)\n\n");
	vfDualPrintF(" -v       validate evaluation function\n");
	vfDualPrintF(" -a       append log file\n\n");
	vfDualPrintF(" -h,\n -?,\n -i       this screen\n");
}

void vfPrintHeader(bool bShowHeader)
{
	char s[80];
	memset(s,0,sizeof(s));
	memset(s,'*',sizeof(s)-1);
	if (bShowHeader) {
		vfDualPrintF("%s\n",s);
		vfDualPrintF("*                    Evaluation function constructor v0.581                   *\n");
		vfDualPrintF("*                       for Windows 95,98,ME,NT,2000,XP                       *\n");
		vfDualPrintF("*                             by Dmitriy Iassenev                             *\n");
		vfDualPrintF("*                      Copyright(C) GSC Game World 2002                       *\n");
		vfDualPrintF("*                      Compiled on %s %s                       *\n",__DATE__,__TIME__);
		vfDualPrintF("%s\n",s);
	}
	else {
		fprintf(fOutput,"%s\n",s);
		fprintf(fOutput,"*                    Evaluation function constructor v0.581                   *\n");
		fprintf(fOutput,"*                       for Windows 95,98,ME,NT,2000,XP                       *\n");
		fprintf(fOutput,"*                             by Dmitriy Iassenev                             *\n");
		fprintf(fOutput,"*                      Copyright(C) GSC Game World 2002                       *\n");
		fprintf(fOutput,"*                      Compiled on %s %s                       *\n",__DATE__,__TIME__);
		fprintf(fOutput,"%s\n",s);
	}
}

void vfPerformOperations( int argc, char *argv[], char *caProjectFolder, bool bShowHeader = true)
{
	sprintf(caLogData		,caLogDataFormat	   ,caProjectFolder);
	sprintf(caTextData		,caTextDataFormat	   ,caProjectFolder);
	sprintf(caBinaryData	,caBinaryDataFormat	   ,caProjectFolder);
	sprintf(caConfigData	,caConfigDataFormat	   ,caProjectFolder);
	sprintf(caPatternData	,caPatternDataFormat   ,caProjectFolder);
	sprintf(caCoreData		,caCoreDataFormat	   ,caProjectFolder);
	sprintf(caParametersData,caParametersDataFormat,caProjectFolder);
	sprintf(caEFData		,caEFDataFormat		   ,caProjectFolder);
	
	if (bfCheckForSwitch(argc, argv,"a"))
		fOutput = fopen(caLogData,"at");
	else
		fOutput = fopen(caLogData,"wt");
	
	try {
		CHECK_FILE_IF_OPENED(fOutput,caLogData)
		
		vfPrintHeader(bShowHeader);
			
		if (bfCheckForSwitch(argc, argv,"c"))
			vfConvertTestData(caTextData,caBinaryData,caPatternData,bfCheckForSwitch(argc, argv,"cd"));
		if (bfCheckForSwitch(argc, argv,"gc"))
			vfGenerateComplexConfigurations(caBinaryData,caConfigData);
		if (bfCheckForSwitch(argc, argv,"gp"))
			vfGeneratePatterns(caBinaryData,caConfigData,caPatternData,bfCheckForSwitch(argc, argv,"gps"));
		if (bfCheckForSwitch(argc, argv,"gb"))
			vfGeneratePatternBasis(caBinaryData,caPatternData,caCoreData);
		if (bfCheckForSwitch(argc, argv,"f"))
			vfOptimizeParameters(caCoreData,caParametersData,bfCheckForSwitch(argc, argv,"fp"),bfCheckForSwitch(argc, argv,"fu"));
		if (bfCheckForSwitch(argc, argv,"l") || bfCheckForSwitch(argc, argv,"s") || bfCheckForSwitch(argc, argv,"w"))
			vfShowTestData(caCoreData,caParametersData,bfCheckForSwitch(argc, argv,"l"),bfCheckForSwitch(argc, argv,"s"),bfCheckForSwitch(argc, argv,"w"),bfCheckForSwitch(argc, argv,"la"));
		if (bfCheckForSwitch(argc, argv,"b"))
			vfBuildEvaluationFunction(caCoreData,caParametersData,caEFData,bfCheckForSwitch(argc, argv,"bf"));
		if (bShowHeader && bfCheckForSwitch(argc, argv,"v"))
			vfValidateTestData(caEFData);
		if ((bfCheckForSwitch(argc, argv,"h") || bfCheckForSwitch(argc, argv,"?") || bfCheckForSwitch(argc, argv,"i")) ||
			 !bfCheckForSwitch(argc, argv,"p") ||
			(!bfCheckForSwitch(argc, argv,"c") && 
 			 !bfCheckForSwitch(argc, argv,"gp") && 
 			 !bfCheckForSwitch(argc, argv,"gc") && 
 			 !bfCheckForSwitch(argc, argv,"gb") && 
			 !bfCheckForSwitch(argc, argv,"f") && 
			 !bfCheckForSwitch(argc, argv,"l") && 
			 !bfCheckForSwitch(argc, argv,"s") && 
			 !bfCheckForSwitch(argc, argv,"w") && 
			 !bfCheckForSwitch(argc, argv,"b") && 
			 !bfCheckForSwitch(argc, argv,"v") && 
			 !bfCheckForSwitch(argc, argv,"h") && 
			 !bfCheckForSwitch(argc, argv,"i") &&
			 !bfCheckForSwitch(argc, argv,"?")))
			vfShowHelpScreen();
		fclose(fOutput);
	}
	catch(char *fFileName) {
		if (strlen(fFileName))
			vfDualPrintF("\nCannot open file %s\n",fFileName);
	}
}

void vfPerformFoldersRecursively(int argc, char *argv[], const char *caPath, uint &uiProjectProcessedCount, bool bFirst = false)
{
	struct _finddata_t tProject;
	uint		uiHandle;
	char		l_caPath[260];
	l_caPath[0] = 0;
	strcat		(l_caPath,caPath);
	strcat		(l_caPath,"\\*.*");
	uiHandle	= _findfirst(l_caPath,&tProject);
	if (uiHandle == 0xffffffff) {
		if (bFirst)
			vfDualPrintF("Projects not found\n");
		return;
	}

	while (true) {
		if (_findnext(uiHandle,&tProject) != 0) {
			_findclose(uiHandle);
			if (bFirst)
				if (!uiProjectProcessedCount)
					vfDualPrintF("Projects not found\n");
				else
					vfDualPrintF("\nTotal processed %d projects\n\n",uiProjectProcessedCount);
			break;
		}
		if ((tProject.attrib & _A_SUBDIR) && (strcmp(tProject.name,".")) && (strcmp(tProject.name,".."))) {
			char	l_caProjectFolder[260];
			strcpy	(l_caProjectFolder,caPath);
			strcat	(l_caProjectFolder,"\\");
			strcat	(l_caProjectFolder,tProject.name);
			sprintf	(caTextData,caTextDataFormat,l_caProjectFolder);
			FILE	*f = fopen(caTextData,"rt");
			if (f) {
				fclose(f);
				uiProjectProcessedCount++;
				vfDualPrintF("\nPROJECT BEING PROCESSED : %s\n",tProject.name);
				vfPerformOperations(argc,argv,l_caProjectFolder,false);
			}
			else
				vfPerformFoldersRecursively(argc,argv,l_caProjectFolder,uiProjectProcessedCount);
		}
	}
}

//#include <vector>
//#include <map>
//#include <algorithm>
//
//#define R_ASSERT2(a,b)	if (!(a)) printf("%s\n",(b));
//#define VERIFY(a)		if (!(a)) printf("VERIFY failed!\n");
//#define IC				__forceinline
//
//typedef	signed		__int8	s8;
//typedef	unsigned	__int8	u8;
//
//typedef	signed		__int16	s16;
//typedef	unsigned	__int16	u16;
//
//typedef	signed		__int32	s32;
//typedef	unsigned	__int32	u32;
//
//typedef	signed		__int64	s64;
//typedef	unsigned	__int64	u64;
//
//template<
//	typename TIME_ID, 
//	typename TYPE_ID, 
//	typename VALUE_ID, 
//	typename BLOCK_ID, 
//	typename CHUNK_ID,
//	VALUE_ID tMinValue, 
//	VALUE_ID tMaxValue, 
//	CHUNK_ID tBlockSize,
//	VALUE_ID tInvalidValueID = tMaxValue,
//	TIME_ID	 tStartTime = 0> 
//class CID_Generator 
//{
//	typedef std::multimap< TIME_ID , BLOCK_ID >		TIME_BLOCK_MAP;
//	typedef typename TIME_BLOCK_MAP::iterator		TIME_BLOCK_PAIR_IT;
//
//	struct SID_Block {
//		CHUNK_ID				m_tCount;
//		TIME_ID					m_tTimeID;
//		TYPE_ID					m_tpIDs[tBlockSize];
//		SID_Block				() : m_tCount(0) {}
//	};
//
//	enum {
//		m_tBlockCount = (u32(tMaxValue - tMinValue) + 1)/tBlockSize,
//	};
//
//	TIME_BLOCK_MAP				m_tpTimeMap;
//	SID_Block					m_tppBlocks			[m_tBlockCount];
//
//	IC		BLOCK_ID			tfGetBlockByValue(VALUE_ID tValueID)
//	{
//		BLOCK_ID				l_tBlockID = BLOCK_ID((tValueID - tMinValue)/tBlockSize);
//		R_ASSERT2				(l_tBlockID < m_tBlockCount,"Requesting ID is invalid!");
//		return					(l_tBlockID);
//	}
//
//			VALUE_ID			tfGetFromBlock	(TIME_BLOCK_PAIR_IT &tIterator, VALUE_ID tValueID)
//	{
//		BLOCK_ID				l_tBlockID = (*tIterator).second;
//		SID_Block				&l_tID_Block = m_tppBlocks[l_tBlockID];
//
//		VERIFY					(l_tID_Block.m_tCount);
//
//		if (l_tID_Block.m_tCount == 1)
//			m_tpTimeMap.erase	(tIterator);
//
//		if (tInvalidValueID == tValueID)
//			return				(VALUE_ID(l_tID_Block.m_tpIDs[--l_tID_Block.m_tCount]) + l_tBlockID*tBlockSize + tMinValue);
//
//		TYPE_ID					*l_tpBlockID = std::find(l_tID_Block.m_tpIDs, l_tID_Block.m_tpIDs + l_tID_Block.m_tCount, TYPE_ID((tValueID - tMinValue)%tBlockSize));	
//		R_ASSERT2				(l_tpBlockID != l_tID_Block.m_tpIDs + l_tID_Block.m_tCount,"Requesting ID has already been used!");
//		*l_tpBlockID			= *(l_tID_Block.m_tpIDs + --l_tID_Block.m_tCount);
//		return					(tValueID);
//	}
//
//public:
//	VALUE_ID					m_tInvalidValueID;
//
//								CID_Generator	()
//	{
//		m_tInvalidValueID		= tInvalidValueID;
//		for (VALUE_ID i=0; ; i++) {
//			vfFreeID			(i,tStartTime);
//			if (i >= tMaxValue)
//				break;
//		}
//		for (u32 j=0; j<m_tBlockCount; j++)
//			std::reverse		(m_tppBlocks[j].m_tpIDs,m_tppBlocks[j].m_tpIDs + m_tppBlocks[j].m_tCount);
//	}
//
//	virtual						~CID_Generator	()
//	{
//	}
//
//			VALUE_ID			tfGetID			(VALUE_ID tValueID = tInvalidValueID)
//	{
//		if (tInvalidValueID == tValueID) {
//			TIME_BLOCK_PAIR_IT	I = m_tpTimeMap.begin();
//			R_ASSERT2			(I != m_tpTimeMap.end(),"Not enough IDs");
//			return				(tfGetFromBlock(I,tValueID));
//		}
//		else {
//			BLOCK_ID			l_tBlockID = tfGetBlockByValue(tValueID);
//			TIME_ID				l_tTimeID = m_tppBlocks[l_tBlockID].m_tTimeID;
//			TIME_BLOCK_PAIR_IT	I = m_tpTimeMap.find(l_tTimeID);
//			TIME_BLOCK_PAIR_IT	E = m_tpTimeMap.end();
//			for ( ; I != E; I++)
//				if ((*I).second == l_tBlockID)
//					return		(tfGetFromBlock(I,tValueID));
//			R_ASSERT2			(false,"Requesting ID has already been used!");
//			return				(tInvalidValueID);
//		}
//	}
//
//			void				vfFreeID		(VALUE_ID tValueID, TIME_ID tTimeID)
//	{
//		BLOCK_ID				l_tBlockID = tfGetBlockByValue(tValueID);
//		SID_Block				&l_tID_Block = m_tppBlocks[l_tBlockID];
//
//		VERIFY					(l_tID_Block.m_tCount < tBlockSize);
//
//		if (!l_tID_Block.m_tCount)
//			m_tpTimeMap.insert	(std::make_pair(tTimeID,l_tBlockID));
//
//		l_tID_Block.m_tpIDs		[l_tID_Block.m_tCount++] = TYPE_ID((tValueID - tMinValue)%tBlockSize);
//		l_tID_Block.m_tTimeID	= tTimeID;
//	}
//};
//
//void vfTestID_Generator()
//{
//	CID_Generator<u32,u8,u16,u8,u16,0,u16(-1),256>	l_tGenerator;
//	for (int i=0; i<256; i++)
//		for (int j=0; j<256; j++)
//			printf("%d\n",l_tGenerator.tfGetID());
//	l_tGenerator.vfFreeID(7,100);
//	l_tGenerator.vfFreeID(800,200);
//	printf("%d\n",l_tGenerator.tfGetID());
//	printf("%d\n",l_tGenerator.tfGetID());
//	l_tGenerator.vfFreeID(7,400);
//	l_tGenerator.vfFreeID(800,300);
//	printf("%d\n",l_tGenerator.tfGetID());
//	printf("%d\n",l_tGenerator.tfGetID());
//}
//
int __cdecl main(int argc, char *argv[], char *envp[])
{
//	vfTestID_Generator();
	bfLoadIniFile(INI_FILE);
	if (bfCheckForSwitch(argc, argv,"pa")) {
		uint uiProjectProcessedCount = 0;
		vfPrintHeader(true);
		vfPerformFoldersRecursively(argc,argv,"data",uiProjectProcessedCount,true);
	}
	else {
		char *caProjectFolder = cafGetStringNextToSwitch(argc, argv,"p");
		if (!caProjectFolder) {
			vfPrintHeader(true);
			if (argc > 1)
				vfDualPrintF(" Project folder is not specified!\n\n");
			vfShowHelpScreen();
			return(-1);
		}
		char S[260];
		strcpy(S,"data\\");
		strcat(S,caProjectFolder);
		vfPerformOperations(argc,argv,S);
	}
	return(0);
}