///////////////////////////////////////////////////////////////
//	Module 		: xr_emu.cpp
//	Created 	: 20.03.2002
//  Modified 	: 22.03.2002
//	Author		: Dmitriy Iassenev
//	Description : Emulation of the FPS X-Ray
///////////////////////////////////////////////////////////////

#include <stdio.h>
#include <math.h>

#include "xr_a_star.h"
#include "xr_emu.h"
#include "xr_misc.h"

const uchar ucaBits[8] = { 
	0x01, 
	0x02, 
	0x04, 
	0x08, 
	0x10, 
	0x20, 
	0x40, 
	0x80 
};

int ifConvertToDWord(NodeLink tLink)
{
	return(tLink[0] | (tLink[1] << 8) | (tLink[2] << 16));
}

double dfGetDistance(NodeCompressed tNode0, NodeCompressed tNode1)
{
	float X1 = float(tNode0.p0[0])*tNodeHeader.size;
	float Y1 = (float(tNode0.p0[1])/32767)*tNodeHeader.size_y;
	float Z1 = float(tNode0.p0[2])*tNodeHeader.size;

	float X2 = float(tNode1.p0[0])*tNodeHeader.size;
	float Y2 = (float(tNode1.p0[1])/32767)*tNodeHeader.size_y;
	float Z2 = float(tNode1.p0[2])*tNodeHeader.size;

	return(sqrt(SQR(X2 - X1) + SQR(Y2 - Y1) + SQR(Z2 - Z1)));
}

double dfNodeSize(NodeCompressed tNode)
{
	float X1 = float(tNode.p0[0])*tNodeHeader.size;
	float Y1 = (float(tNode.p0[1])/32767)*tNodeHeader.size_y;
	float Z1 = float(tNode.p0[2])*tNodeHeader.size;

	float X2 = float(tNode.p1[0])*tNodeHeader.size;
	float Y2 = (float(tNode.p1[1])/32767)*tNodeHeader.size_y;
	float Z2 = float(tNode.p1[2])*tNodeHeader.size;

	return(sqrt(SQR(X2 - X1) + SQR(Y2 - Y1) + SQR(Z2 - Z1)));
}

void vfFreePathList(TNode *tpList)
{
	while (tpList) {
		TNode *tpTemp = tpList->tpNext;
		free(tpList);
		tpList = tpTemp;
	}
}

double dfCriteria(NodeCompressed tCurrentNode, NodeCompressed tFinishNode)
{
	double dLight = tCurrentNode.light/255.0 + 1.0;
	double dCover = 1.0/((tCurrentNode.cover[0]/255.0 + 1)*(tCurrentNode.cover[1]/255.0 + 1)*(tCurrentNode.cover[2]/255.0 + 1)*(tCurrentNode.cover[3]/255.0 + 1));
	double dDistance = dfGetDistance(tCurrentNode,tFinishNode);
	//double dNodeSize = dfNodeSize(tCurrentNode);
	return(sqrt(dLight*dCover*dDistance));
	//return(dDistance);
}

void vfEmulateGamePlay(int iStartNode,int iFinishNode)
{
	TNode *tpPath = tpfFindTheShortestPath(iStartNode,iFinishNode), *tpSafe = tpPath;
	
	tpPath = tpPath->tpNext;

	printf(" ##    Node       Criteria (Lght  Dist  Size  Cover[4])\n");

	NodeCompressed tStartNode = *tpaNodes[iStartNode];
	NodeCompressed tFinishNode = *tpaNodes[iFinishNode];
	double dCurMax = dfCriteria(tStartNode,tFinishNode);

	for (int j = 0, iCurNode = iStartNode; tpPath; j++) {
		
		if (iCurNode == -1)
			break;
		
		printf("%4d %6d : %12.7f (%4d %5.2f %5.2f %5.2f %5.2f %5.2f %5.2f)\n",j,iCurNode,dCurMax,
			tpaNodes[iCurNode]->light,dfGetDistance(tFinishNode,*tpaNodes[iCurNode]),
			dfNodeSize(*tpaNodes[iCurNode]),
			tpaNodes[iCurNode]->cover[0]/255.0 + 1,tpaNodes[iCurNode]->cover[1]/255.0 + 1,
			tpaNodes[iCurNode]->cover[2]/255.0 + 1,tpaNodes[iCurNode]->cover[3]/255.0 + 1);

		NodeCompressed tNode = *(tpaNodes[iCurNode]);
		NodeLink *taLinks = (NodeLink *)((uchar *)(tpaNodes[iCurNode]) + sizeof(tNode));
		double dNodeCriteria = dfCriteria(tNode,tFinishNode);

		dCurMax = 0.0;
		for (int i = 0, iIndex = -1; i<tNode.link_count; i++) {
			
			int iNodeIndex = ifConvertToDWord(taLinks[i]);
			NodeCompressed tCurNode = *tpaNodes[iNodeIndex];
			double dCriteria = dfCriteria(tCurNode,tFinishNode);

			/**
			printf("%4d %6d : %12.7f (%4d %5.2f %5.2f %5.2f %5.2f %5.2f %5.2f)\n",j,
				iNodeIndex,dCriteria,
				tCurNode.light,dfGetDistance(tFinishNode,tCurNode),
				dfNodeSize(tCurNode),
				tCurNode.cover[0]/255.0 + 1,tCurNode.cover[1]/255.0 + 1,
				tCurNode.cover[2]/255.0 + 1,tCurNode.cover[3]/255.0 + 1);
			/**/

			if (iNodeIndex == tpPath->iIndex) {
				iIndex = iNodeIndex;
				dCurMax = dCriteria;
			}
		}

		//printf("\n");
		
		iCurNode = iIndex;
		
		tpPath = tpPath->tpNext;
	}
	printf("%7.2f\n",tpSafe->f);
	printf("Test was performed successfully\n");

	vfFreePathList(tpSafe);
}
