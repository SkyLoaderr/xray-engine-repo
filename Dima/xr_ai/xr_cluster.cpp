///////////////////////////////////////////////////////////////
//	Module 		: xr_cluster.cpp
//	Created 	: 22.03.2002
//  Modified 	: 22.03.2002
//	Author		: Dmitriy Iassenev
//	Description : Node Clusterisation Algorithm
///////////////////////////////////////////////////////////////

#include <stdio.h>

#include "xr_cluster.h"
#include "xr_emu.h"
#include "xr_misc.h"

uint   *uiaNodeClusters;
double *daNodesTSE;
uint   *uiaNodesInCluster;
uint    uiNodesInCluster;

double dDifference(NodeCompressed tNode0, NodeCompressed tNode1)
{
	double dResult = SQR((tNode0.light - tNode1.light)/255.0);
	dResult += SQR((tNode0.cover[0] - tNode1.cover[0])/255.0);
	dResult += SQR((tNode0.cover[1] - tNode1.cover[1])/255.0);
	dResult += SQR((tNode0.cover[2] - tNode1.cover[2])/255.0);
	dResult += SQR((tNode0.cover[3] - tNode1.cover[3])/255.0);
	return(dResult);
}

void vfJoinNodesToCluster(int iCurrentNode, int iClusterNumber, double dEpsilon)
{
	if (!(uiaNodeClusters[iCurrentNode])) {
		
		NodeCompressed tCurrentNode = *tpaNodes[iCurrentNode];
		daNodesTSE[uiNodesInCluster] = 0.0;
		
		for (uint i=0; i<uiNodesInCluster; i++) {
			double dTemp = dDifference(*tpaNodes[uiaNodesInCluster[i]],tCurrentNode);
			if (dTemp > dEpsilon)
				return;
			else 
				if (dTemp > daNodesTSE[uiNodesInCluster]) 
					daNodesTSE[uiNodesInCluster] = dTemp;
		}
		
		for ( i=0; i<uiNodesInCluster; i++) {
			double dTemp = dDifference(*tpaNodes[uiaNodesInCluster[i]],tCurrentNode);
			if (dTemp > daNodesTSE[i])
				daNodesTSE[i] = dTemp;
		}

		uiaNodeClusters[iCurrentNode] = iClusterNumber;
		uiaNodesInCluster[uiNodesInCluster] = iCurrentNode;
		uiNodesInCluster++;

		i = 0;
		NodeLink *taLinks = (NodeLink *)((uchar *)(tpaNodes[iCurrentNode]) + sizeof(NodeCompressed));
		for (uint iCount = tpaNodes[iCurrentNode]->link_count, iNodeIndex = ifConvertToDWord(taLinks[i]); i<iCount; i++, iNodeIndex = ifConvertToDWord(taLinks[i]))
			if (!(uiaNodeClusters[iNodeIndex]))
				vfJoinNodesToCluster(iNodeIndex,iClusterNumber,dEpsilon);
	}
}

void vfClusteriseNodes(double dEpsilon)
{
	// allocating memory for the cluster map
	uiaNodeClusters = (uint *)calloc(tNodeHeader.count,sizeof(uint));
	uiaNodesInCluster = (uint *)calloc(tNodeHeader.count,sizeof(uint));
	daNodesTSE = (double *)calloc(tNodeHeader.count,sizeof(double));
	
	// clasterising
	//for (uint i=0, uiClusterCount=0; i<tNodeHeader.count; i++) 
	for (int i=tNodeHeader.count-1, uiClusterCount=0; i>=0; i--) 
		if (!(uiaNodeClusters[i])) {
			uiNodesInCluster = 0;
			uiClusterCount++;
			vfJoinNodesToCluster(i,uiClusterCount,dEpsilon);
			printf("%6d %6d %6d\n",uiClusterCount,i,uiNodesInCluster);
			/**/
			for (uint j=0; j<uiNodesInCluster; j++) {
				if (uiaNodesInCluster[j] == 127280)
					for (uint k=0; k<uiNodesInCluster; k++) {
						NodeCompressed tNode = *tpaNodes[uiaNodesInCluster[k]];
						printf("# %4d %6d %6d %4.2f %4.2f %4.2f %4.2f %4.2f\n",uiClusterCount,k,
							uiaNodesInCluster[k],
							tNode.light/255.0,
							tNode.cover[0]/255.0,
							tNode.cover[1]/255.0,
							tNode.cover[2]/255.0,
							tNode.cover[3]/255.0
						);
					}
				/**
				NodeCompressed tNode = *tpaNodes[uiaNodesInCluster[j]];
				printf("# %4d %6d %6d %4.2f %4.2f %4.2f %4.2f %4.2f\n",uiClusterCount,j,
					uiaNodesInCluster[j],
					tNode.light/255.0,
					tNode.cover[0]/255.0,
					tNode.cover[1]/255.0,
					tNode.cover[2]/255.0,
					tNode.cover[3]/255.0
				);
				/**/
			}
			//getchar();
			/**/
		}
	// saving clusterised zones
	FILE *fClusterMap = fopen("data\\level_00.zon","wb");
	fwrite(uiaNodeClusters,sizeof(uint),tNodeHeader.count,fClusterMap);
	fclose(fClusterMap);
	
	// freeing resources
	free(daNodesTSE);
	free(uiaNodesInCluster);
	free(uiaNodeClusters);
}
