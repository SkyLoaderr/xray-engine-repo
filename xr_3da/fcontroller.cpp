#include "stdafx.h"
#include "x_ray.h"
#include "xr_input.h"
#include "fcontroller.h"

void	CController::iCapture					(void)
{
	VERIFY(pInput);
	pInput->iCapture(this);
}

void	CController::iRelease					(void)
{
	VERIFY(pInput);
	pInput->iRelease(this);
}

void	CController::iGetLastMouseDelta			(Ipoint& p)
{
	VERIFY(pInput);
	pInput->iGetLastMouseDelta( p );
}

void CController::OnInputDeactivate			(void)
{
	int i;
	for (i = 0; i < COUNT_KB_BUTTONS; i++ )
		OnKeyboardRelease( i );
	for (i = 0; i < COUNT_MOUSE_BUTTONS; i++ )
		OnMouseRelease( i );
	OnMouseStop	( DIMOFS_X, 0 );
	OnMouseStop	( DIMOFS_Y, 0 );
}

BOOL CController::iGetKeyState(int dik)
{
	VERIFY(pInput);
	return pInput->iGetAsyncKeyState(dik);
}

BOOL CController::iGetBtnState(int btn)
{
	VERIFY(pInput);
	return pInput->iGetAsyncBtnState(btn);
}
