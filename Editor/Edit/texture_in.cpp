//---------------------------------------------------------------------------
#include "pch.h"
#pragma hdrstop

//---------------------------------------------------------------------------
bool FillDDSurfaceEx(LPDIRECTDRAWSURFACE7 m_Surface,
	DDSURFACEDESC2 m_Desc,
	int m_Width,
	int m_Height,
	FPcolor* m_Pixels){

	if( m_Surface == 0 )
		return false;

	if( DD_OK != m_Surface->Lock(0,&m_Desc,
		DDLOCK_SURFACEMEMORYPTR|DDLOCK_WAIT|DDLOCK_WRITEONLY,0 ) )
			return false;

	WORD off_r,off_g,off_b,off_a;
	DWORD mask_r,mask_g,mask_b,mask_a;
	DWORD loopW,loopH;

	_ASSERTE( (m_Desc.ddpfPixelFormat.dwRGBBitCount % 8)==0 );
	_ASSERTE( m_Desc.ddpfPixelFormat.dwRGBBitCount >= 8 );
	_ASSERTE( m_Desc.lpSurface );
	_ASSERTE( m_Desc.ddpfPixelFormat.dwRBitMask );
	_ASSERTE( m_Desc.ddpfPixelFormat.dwGBitMask );
	_ASSERTE( m_Desc.ddpfPixelFormat.dwBBitMask );

	mask_r = m_Desc.ddpfPixelFormat.dwRBitMask;
	mask_g = m_Desc.ddpfPixelFormat.dwGBitMask;
	mask_b = m_Desc.ddpfPixelFormat.dwBBitMask;

	DWORD bytepixsize = m_Desc.ddpfPixelFormat.dwRGBBitCount / 8;
	LPBYTE px = (LPBYTE) m_Desc.lpSurface;
	LPBYTE orgdata = (LPBYTE) m_Pixels;

	loopW = m_Width;
	loopH = m_Height;

	DWORD addptr =
		m_Desc.lPitch - loopW*bytepixsize;

	__asm{
			mov eax,mask_r
			bsr ebx,eax
			mov dx,31
			sub dx,bx
			mov off_r,dx

			mov eax,mask_g
			bsr ebx,eax
			mov dx,31
			sub dx,bx
			mov off_g,dx

			mov eax,mask_b
			bsr ebx,eax
			mov dx,31
			sub dx,bx
			mov off_b,dx
	}

	__asm{
			// WH loops prepare

			mov esi,orgdata
			mov edi,px
			mov ecx,loopH

scanloop_rgba_H:

			push ecx
			mov ecx,loopW

scanloop_rgba_W:

			mov edx,ecx

			mov ebx,0

			// B
			mov al, byte ptr [esi]
			shl eax, 24
			mov cx,  off_b
			shr eax, cl
			and eax, mask_b
			or  ebx, eax

			// G
			mov al, byte ptr [esi+1]
			shl eax,24
			mov cx,off_g
			shr eax,cl
			and eax,mask_g
			or  ebx,eax

			// R
			mov al, byte ptr [esi+2]
			shl eax,24
			mov cx,off_r
			shr eax,cl
			and eax,mask_r
			or  ebx,eax

			// store pixel

			mov ecx,bytepixsize
mvloop_rgba:
			mov byte ptr [edi], bl
			shr ebx,8
			inc edi
			loop mvloop_rgba

			add esi,4
			mov ecx,edx
			loop scanloop_rgba_W

			// adjust scanline
			add edi,addptr

			pop ecx
			loop scanloop_rgba_H
	}

	m_Surface->Unlock(0);
	return true;
}

#pragma package(smart_init)
