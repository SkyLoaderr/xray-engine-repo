///////////////////////////////////////////////////////////////
//	Module 		: xr_misc.cpp
//	Created 	: 20.03.2002
//  Modified 	: 20.03.2002
//	Author		: Dmitriy Iassenev
//	Description : Miscellanious routines
///////////////////////////////////////////////////////////////

#include <stdio.h>

#include "xr_misc.h"
 
hdrNodes tNodeHeader;
NodeCompressed **tpaNodes;
uchar *taRawNodeData;

void vfFreeAll()
{
	free(tpaNodes);
	free(taRawNodeData);
}

void vfLoadLevelMap(char *caFileName)
{
	FILE *fLevel = fopen(caFileName,"rb");

	fread(&tNodeHeader,1,sizeof(tNodeHeader),fLevel);
	printf("Level   : 0\n");
	printf("Version : %d\n",tNodeHeader.version);
	printf("Nodes   : %d\n",tNodeHeader.count);

	tpaNodes = (NodeCompressed **)malloc(tNodeHeader.count*sizeof(NodeCompressed *));

	fseek(fLevel,0,SEEK_END);
	printf("Size    : %d\n",ftell(fLevel));
	taRawNodeData = (uchar *)malloc((ftell(fLevel) - sizeof(tNodeHeader))*sizeof(uchar));
	fseek(fLevel,sizeof(tNodeHeader),SEEK_SET);

	for (uint i=0, uiCurPos=0; i<tNodeHeader.count; i++) {

		tpaNodes[i] = (NodeCompressed *)(taRawNodeData + uiCurPos);
		
		NodeCompressed tNode;
		fread(&tNode,1,sizeof(tNode),fLevel);

		memcpy(taRawNodeData + uiCurPos, &tNode, sizeof(tNode));

		uiCurPos += sizeof(tNode);
		
		fread(taRawNodeData + uiCurPos,tNode.link_count,sizeof(NodeLink),fLevel);
		uiCurPos += tNode.link_count*sizeof(NodeLink);
	}

	fclose(fLevel);
}
