#pragma once

#include "fmesh.h"
#include "fs.h"

// Vertex containers
class VBContainer 
{
	xr_vector<VDeclarator>		vDcl;
	xr_vector<xr_vector<BYTE> >	vContainers;

	// Recording
	VDeclarator				R_DCL;
	xr_vector<BYTE>			R_DATA;

public:
	// Constructor & destructor
	VBContainer()			{	R_DCL.clear();	}

	// Methods
	void	Begin			(u32	dwFVF)
	{
		R_ASSERT		(R_DCL.empty());
		R_ASSERT		(R_DATA.empty());
		R_DCL.set		(dwFVF);
	}
	void	Begin			(const VDeclarator& D)
	{
		R_ASSERT		(R_DCL.empty());
		R_ASSERT		(R_DATA.empty());
		R_DCL.set		(D);
	}
	void	Add				(VOID* PTR, u32 cnt)
	{
		R_ASSERT		(R_DCL.size());
		BYTE*	P		= (BYTE*) PTR;
		R_DATA.insert	(R_DATA.end(),P,P+cnt);
	}
	void	End				(u32* dwContainerID, u32 *dwIndexStart)
	{
		R_ASSERT		(! R_DCL.empty()	);
		R_ASSERT		(! R_DATA.empty()	);
		
		u32		dwSize  = R_DCL.vertex	();
		R_ASSERT		(R_DATA.size()%dwSize == 0);
		
		// Search for container capable of handling data
		u32 bytes_collected	= R_DATA.size();
		u32 vertices_collected= bytes_collected/dwSize;
		for (u32 CID = 0; CID<vDcl.size(); CID++)
		{
			if (!vDcl[CID].equal(R_DCL))	continue;
			
			u32 bytes_already	= vContainers[CID].size();
			if ((bytes_already+bytes_collected)>g_params.m_VB_maxSize) continue;
			u32 vertices_already = bytes_already/dwSize;
			if ((vertices_already+vertices_collected)>g_params.m_VB_maxVertices) continue;
			
			// If we get here - container CID can take the data
			*dwContainerID			= CID;
			*dwIndexStart			= vertices_already;
			vContainers[CID].insert	(vContainers[CID].end(),R_DATA.begin(),R_DATA.end());
			R_DCL.clear	();
			R_DATA.clear();
			return;
		}
		
		// No such format found
		// Simple add it and register
		*dwContainerID			= vDcl.size();
		*dwIndexStart			= 0;
		vDcl.push_back			(R_DCL);	R_DCL.clear();
		vContainers.push_back	(R_DATA);	R_DATA.clear();
	}
	void	Save	(IWriter &fs)
	{
		R_ASSERT		(R_DCL.empty());
		R_ASSERT		(R_DATA.empty());
		fs.w_u32		(vDcl.size());
		for (u32 i=0; i<vDcl.size(); i++)
		{
			u32 dwOneSize		= vDcl[i].vertex();
			u32 dwTotalSize	= vContainers[i].size();
			u32 dwVertCount	= dwTotalSize/dwOneSize;

			R_ASSERT	(dwVertCount*dwOneSize == dwTotalSize);
			
			fs.w		(vDcl[i].begin(), vDcl[i].size()*sizeof(D3DVERTEXELEMENT9));	// Vertex format
			fs.w_u32	(dwVertCount);													// Number of vertices
			fs.w		(&*vContainers[i].begin(),dwTotalSize);
		}
		vDcl.clear			();
		vContainers.clear	();
	}
};

class IBContainer
{
	xr_vector<xr_vector<u16> >	data;
	enum {
		LIMIT = 1024ul * 1024ul
	};
public:
	void	Register		(u16* begin, u16* end, u32* dwContainerID, u32 *dwStart)
	{
		u32 size				= end-begin;

		// 
		for	(u32 ID=0; ID<data.size(); ID++)
		{
			if ((data[ID].size()+size) < LIMIT)	
			{
				*dwContainerID	= ID;
				*dwStart		= data[ID].size();
				data[ID].insert	(data[ID].end(),begin,end);
				return;
			}
		}

		// Can't find suitable container - register new
		*dwContainerID		= data.size();
		*dwStart			= 0;
		data.push_back		(xr_vector<u16> ());
		data.back().assign	(begin,end);
	}
	void	Save	(IWriter &fs)
	{
		fs.w_u32	(data.size());
		for (u32 i=0; i<data.size(); i++)
		{
			fs.w_u32	(data[i].size());
			fs.w		(&*data[i].begin(),data[i].size()*2);
		}
		data.clear	();
	}
};

extern VBContainer g_VB;
