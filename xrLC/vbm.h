#pragma once

#include "fmesh.h"
#include "fs.h"

// Vertex containers
class VBContainer {
	vector<DWORD>			FVF;
	vector<vector<BYTE> >	vContainers;

	// Recording
	DWORD					R_FVF;
	vector<BYTE>			R_DATA;

public:
	// Constructor & destructor
	VBContainer()			{	R_FVF = 0;	}

	// Methods
	void	Begin			(DWORD	dwFVF)
	{
		VERIFY(R_FVF==0);
		VERIFY(R_DATA.empty());
		R_FVF = dwFVF;
	}
	void	Add				(VOID* PTR, DWORD cnt)
	{
		VERIFY(R_FVF);
		BYTE*	P = (BYTE*) PTR;
		R_DATA.insert(R_DATA.end(),P,P+cnt);
	}
	void	End				(DWORD* dwContainerID, DWORD *dwIndexStart)
	{
		VERIFY(R_FVF);
		VERIFY(! R_DATA.empty());
		
		DWORD	dwSize  = D3DXGetFVFVertexSize(R_FVF);
		VERIFY(R_DATA.size()%dwSize == 0);
		
		// Search for container capable of handling data
		DWORD bytes_collected	= R_DATA.size();
		DWORD vertices_collected= bytes_collected/dwSize;
		for (DWORD CID = 0; CID<FVF.size(); CID++)
		{
			if (FVF[CID]!=R_FVF) continue;
			
			DWORD bytes_already	= vContainers[CID].size();
			if ((bytes_already+bytes_collected)>g_params.m_VB_maxSize) continue;
			DWORD vertices_already = bytes_already/dwSize;
			if ((vertices_already+vertices_collected)>g_params.m_VB_maxVertices) continue;
			
			// If we get here - container CID can take the data
			*dwContainerID			= CID;
			*dwIndexStart			= vertices_already;
			vContainers[CID].insert(vContainers[CID].end(),R_DATA.begin(),R_DATA.end());
			R_FVF=0;
			R_DATA.clear();
			return;
		}
		
		// No such format found
		// Simple add it and register
		*dwContainerID			= FVF.size();
		*dwIndexStart			= 0;
		FVF.push_back			(R_FVF);	R_FVF=0;
		vContainers.push_back	(R_DATA);	R_DATA.clear();
	}
	void	Save	(CFS_Base &fs)
	{
		VERIFY(R_FVF==0);
		VERIFY(R_DATA.empty());
		fs.Wdword(FVF.size());
		for (DWORD i=0; i<FVF.size(); i++)
		{
			DWORD dwOneSize		= D3DXGetFVFVertexSize(FVF[i]);
			DWORD dwTotalSize	= vContainers[i].size();
			DWORD dwVertCount	= dwTotalSize/dwOneSize;

			VERIFY(dwVertCount*dwOneSize == dwTotalSize);

			fs.Wdword(FVF[i]);		// Vertex format
			fs.Wdword(dwVertCount);	// Number of vertices
			fs.write(vContainers[i].begin(),dwTotalSize);
		}
		FVF.clear();
		vContainers.clear();
	}
};

extern VBContainer g_VB;
