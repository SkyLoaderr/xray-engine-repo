// VBManager.h: interface for the CVBManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VBMANAGER_H__9660964B_B095_450B_873C_4824D7F2D406__INCLUDED_)
#define AFX_VBMANAGER_H__9660964B_B095_450B_873C_4824D7F2D406__INCLUDED_
#pragma once

#define TLB_SIZE	4096

// Vertex buffers
class ENGINE_API CVBManager
{
	// TLB - TL vertices write-only buffer for continous write
	LPDIRECT3DVERTEXBUFFER7	pTLB;
	FTLvertex*				pStart;

public:
	void	OnDeviceCreate	();
	void	OnDeviceDestroy	();

	LPDIRECT3DVERTEXBUFFER7 Create(
		DWORD dwVertType, DWORD dwVertCount, bool bSoftware=false);
	LPDIRECT3DVERTEXBUFFER7 CreateFromData(
		DWORD dwTypeDest, DWORD dwTypeSrc, DWORD dwVertCount, void *pSource, bool bSoftware=false);

	FTLvertex*	TLB_Start		(); // return the pointer to write-only memory
	void		TLB_End			(FTLvertex *pEnd); // ends appending and draw primitive as triangle-STRIP
	void		TLB_EndIndexed	(FTLvertex *pEnd, WORD *pIndices, DWORD dwCount); // ends process and draw primitive as triangle-LIST using indices specified

	CVBManager();
	~CVBManager();
};

extern CVBManager VB;

#endif // !defined(AFX_VBMANAGER_H__9660964B_B095_450B_873C_4824D7F2D406__INCLUDED_)
