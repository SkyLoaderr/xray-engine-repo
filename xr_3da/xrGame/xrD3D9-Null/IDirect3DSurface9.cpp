#include "stdafx.h"

#include "IDirect3DSurface9.h"

#include "xrD3D9-Null_OutProc.h"

const GUID DECLSPEC_SELECTANY IID_IDirect3DSurface9;

xrIDirect3DSurface9::xrIDirect3DSurface9	(IDirect3DDevice9* pIDirect3DDevice9) 
	: m_refCount(0)
{
	APIDEBUG("xrIDirect3DSurface9::xrIDirect3DSurface9");

	m_pIDirect3DDevice9 = pIDirect3DDevice9;
	//-----------------------------------------------
	Name = NULL;
	Width = 0;
	Height = 0;
	Usage = 0;
	Format = D3DFORMAT(0);
	Pool = D3DPOOL(0);
	MultiSampleType = D3DMULTISAMPLE_TYPE(0);
	MultiSampleQuality = 0;
	Priority = 0;
	LockCount = 0;
	DCCount = 0;
	CreationCallStack = NULL;
};

/*** IUnknown methods ***/
HRESULT			xrIDirect3DSurface9::QueryInterface( REFIID riid, void** ppvObj)
{
	APIDEBUG("xrIDirect3DSurface9::QueryInterface");
	if (riid == IID_IUnknown || riid == IID_IDirect3DSurface9)
	{
		*ppvObj = this;
		AddRef();
		return HRESULT_Proc(NOERROR);
	}
	return HRESULT_Proc(E_NOINTERFACE);
}

ULONG			xrIDirect3DSurface9::AddRef() 
{
	APIDEBUG("xrIDirect3DSurface9::AddRef");
	m_refCount++;
	return ULONG_Proc(m_refCount);
}

ULONG			xrIDirect3DSurface9::Release()
{
	APIDEBUG("xrIDirect3DSurface9::Release");
	m_refCount--;
	if (m_refCount < 0)
	{
		delete this;
	}
	return ULONG_Proc(m_refCount);
}

/*** IDirect3DResource9 methods ***/
HRESULT			xrIDirect3DSurface9::GetDevice		( IDirect3DDevice9** ppDevice)										
{
	APIDEBUG("xrIDirect3DSurface9::GetDevice		");
	m_pIDirect3DDevice9->AddRef();
	*ppDevice = m_pIDirect3DDevice9;
	return HRESULT_Proc(S_OK);
};

HRESULT			xrIDirect3DSurface9::SetPrivateData	( REFGUID refguid,CONST void* pData,DWORD SizeOfData,DWORD Flags)	{APIDEBUG("xrIDirect3DSurface9::SetPrivateData	");return HRESULT_Proc(S_OK);};
HRESULT			xrIDirect3DSurface9::GetPrivateData	( REFGUID refguid,void* pData,DWORD* pSizeOfData)					{APIDEBUG("xrIDirect3DSurface9::GetPrivateData	");return HRESULT_Proc(S_OK);};
HRESULT			xrIDirect3DSurface9::FreePrivateData( REFGUID refguid) 													{APIDEBUG("xrIDirect3DSurface9::FreePrivateData");return HRESULT_Proc(S_OK);};
DWORD			xrIDirect3DSurface9::SetPriority	( DWORD PriorityNew) 												{APIDEBUG("xrIDirect3DSurface9::SetPriority	"); DWORD old = Priority; Priority = PriorityNew;  return DWORD_Proc(old);};
DWORD			xrIDirect3DSurface9::GetPriority	() 																	{APIDEBUG("xrIDirect3DSurface9::GetPriority	");return DWORD_Proc(Priority);};
void			xrIDirect3DSurface9::PreLoad		() 																	{APIDEBUG("xrIDirect3DSurface9::PreLoad		"); VOID_proc();};
D3DRESOURCETYPE	xrIDirect3DSurface9::GetType		() 																	{APIDEBUG("xrIDirect3DSurface9::GetType		");return D3DRESOURCETYPE(0);};
HRESULT			xrIDirect3DSurface9::GetContainer	( REFIID riid,void** ppContainer) 									{APIDEBUG("xrIDirect3DSurface9::GetContainer	");return HRESULT_Proc(S_OK);};
HRESULT			xrIDirect3DSurface9::GetDesc		( D3DSURFACE_DESC *pDesc) 											{APIDEBUG("xrIDirect3DSurface9::GetDesc		");return HRESULT_Proc(S_OK);};
HRESULT			xrIDirect3DSurface9::LockRect		( D3DLOCKED_RECT* pLockedRect,CONST RECT* pRect,DWORD Flags) 		{APIDEBUG("xrIDirect3DSurface9::LockRect		");return HRESULT_Proc(S_OK);};
HRESULT			xrIDirect3DSurface9::UnlockRect		() 																	{APIDEBUG("xrIDirect3DSurface9::UnlockRect		");return HRESULT_Proc(S_OK);};
HRESULT			xrIDirect3DSurface9::GetDC			( HDC *phdc) 														{APIDEBUG("xrIDirect3DSurface9::GetDC			");return HRESULT_Proc(S_OK);};
HRESULT			xrIDirect3DSurface9::ReleaseDC		( HDC hdc) 															{APIDEBUG("xrIDirect3DSurface9::ReleaseDC		");return HRESULT_Proc(S_OK);};