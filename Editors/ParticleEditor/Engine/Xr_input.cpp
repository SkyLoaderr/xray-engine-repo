#include "stdafx.h"
#pragma hdrstop

#include "xr_input.h"
#include "fcontroller.h"

CInput *	pInput	= NULL;
CController dummyController;

ENGINE_API float	psMouseSens			= 1.f;
ENGINE_API float	psMouseSensScale	= 1.f;
ENGINE_API DWORD	psMouseInvert		= TRUE;

#define MOUSEBUFFERSIZE			64
#define KEYBOARDBUFFERSIZE		64
#define _KEYDOWN(name,key)		( name[key] & 0x80 )

CInput::CInput						( BOOL bExclusive, int deviceForInit)
{
	Log("Starting INPUT device...");

	pDI 								=	NULL;
	pMouse								=	NULL;
	pKeyboard							=	NULL;

	//=====================Mouse
	mouse_property.mouse_dt				=	25;

	ZeroMemory							( mouseState,	sizeof(mouseState) );
	ZeroMemory							( KBState,		sizeof(KBState) );
	ZeroMemory							( timeStamp,	sizeof(timeStamp) );
	ZeroMemory							( timeSave,		sizeof(timeStamp) );
	ZeroMemory							( offs,			sizeof(offs) );

	//===================== Dummy pack
	iCapture(&dummyController);

	if (!pDI) CHK_DX(DirectInputCreateEx( GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput7, (void**)&pDI, NULL ));

	// KEYBOARD
	if (deviceForInit & keyboard_device_key)
		CHK_DX(CreateInputDevice(
		&pKeyboard, 	GUID_SysKeyboard, 	&c_dfDIKeyboard,
		((bExclusive)?DISCL_EXCLUSIVE:DISCL_NONEXCLUSIVE) | DISCL_FOREGROUND,
		KEYBOARDBUFFERSIZE ));

	// MOUSE
	if (deviceForInit & mouse_device_key)
		CHK_DX(CreateInputDevice(
		&pMouse,		GUID_SysMouse,		&c_dfDIMouse,
		((bExclusive)?DISCL_EXCLUSIVE:DISCL_NONEXCLUSIVE) | DISCL_FOREGROUND | DISCL_NOWINKEY,
		MOUSEBUFFERSIZE ));

#ifdef ENGINE_BUILD 
	Device.seqAppActivate.Add		(this);
	Device.seqAppDeactivate.Add		(this);
	if (psDeviceFlags&mtInput)		Device.seqFrameMT.Add	(this, REG_PRIORITY_LOW);
	else							Device.seqFrame.Add		(this, REG_PRIORITY_HIGH);
#endif
}

CInput::~CInput(void)
{
#ifdef ENGINE_BUILD 
	Device.seqFrameMT.Remove		(this);
	Device.seqFrame.Remove			(this);
	Device.seqAppDeactivate.Remove	(this);
	Device.seqAppActivate.Remove	(this);
#endif
	//_______________________

	// Unacquire and release the device's interfaces
	if( pMouse ){
		pMouse->Unacquire();
		_RELEASE	(pMouse);
	}

	if( pKeyboard ){
		pKeyboard->Unacquire();
		_RELEASE	(pKeyboard);
	}

	_SHOW_REF	("Input: ",pDI);
	_RELEASE	(pDI);
}

//-----------------------------------------------------------------------------
// Name: CreateInputDevice()
// Desc: Create a DirectInput device.
//-----------------------------------------------------------------------------
HRESULT CInput::CreateInputDevice( LPDIRECTINPUTDEVICE7* device, GUID guidDevice, const DIDATAFORMAT* pdidDataFormat, DWORD dwFlags, DWORD buf_size )
{
	// Obtain an interface to the input device
	CHK_DX( pDI->CreateDeviceEx( guidDevice, IID_IDirectInputDevice7, (void**)device, NULL ) );

	// Set the device data format. Note: a data format specifies which
	// controls on a device we are interested in, and how they should be
	// reported.
	CHK_DX((*device)->SetDataFormat( pdidDataFormat ) );

	// Set the cooperativity level to let DirectInput know how this device
	// should interact with the system and with other DirectInput applications.
	HRESULT _hr = (*device)->SetCooperativeLevel( Device.m_hWnd, dwFlags );
	if (FAILED(_hr) && (_hr==E_NOTIMPL)) Msg("! INPUT: Can't set coop level. Emulation???");
	else R_CHK(_hr);

	// setup the buffer size for the keyboard data
	DIPROPDWORD				dipdw;
	dipdw.diph.dwSize		= sizeof(DIPROPDWORD);
	dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	dipdw.diph.dwObj		= 0;
	dipdw.diph.dwHow		= DIPH_DEVICE;
	dipdw.dwData			= buf_size;

	CHK_DX( (*device)->SetProperty( DIPROP_BUFFERSIZE, &dipdw.diph ) );

	return S_OK;
}

//-----------------------------------------------------------------------

void CInput::SetAllAcquire( BOOL bAcquire )
{
	if (pMouse)		bAcquire ? pMouse->Acquire() 	: pMouse->Unacquire();
	if (pKeyboard)	bAcquire ? pKeyboard->Acquire()	: pKeyboard->Unacquire();
}

void CInput::SetMouseAcquire( BOOL bAcquire )
{
	if (pMouse)		bAcquire ? pMouse->Acquire() : pMouse->Unacquire();
}
void CInput::SetKBDAcquire( BOOL bAcquire )
{
	if (pKeyboard)	bAcquire ? pKeyboard->Acquire()	: pKeyboard->Unacquire();
}
//-----------------------------------------------------------------------
void CInput::KeyUpdate	( )
{
	HRESULT						hr;
	DWORD dwElements			= KEYBOARDBUFFERSIZE;
	DIDEVICEOBJECTDATA			od[KEYBOARDBUFFERSIZE];
	DWORD key					= 0;

	VERIFY(pKeyboard);

	hr = pKeyboard->GetDeviceData( sizeof(DIDEVICEOBJECTDATA), &od[0], &dwElements, 0 );
	if (( hr == DIERR_INPUTLOST )||( hr == DIERR_NOTACQUIRED )){
		hr = pKeyboard->Acquire();
		if ( hr != S_OK ) return;
		hr = pKeyboard->GetDeviceData( sizeof(DIDEVICEOBJECTDATA), &od[0], &dwElements, 0 );
		if ( hr != S_OK ) return;
	}

	for (DWORD i = 0; i < dwElements; i++){
		key					= od[i].dwOfs;
		KBState[key]		= od[i].dwData & 0x80;
		if ( KBState[key])	cbStack.top()->OnKeyboardPress		( key );
		if (!KBState[key])	cbStack.top()->OnKeyboardRelease	( key );
	}
	for ( i = 0; i < COUNT_KB_BUTTONS; i++ )
		if (KBState[i]) cbStack.top()->OnKeyboardHold( i );
}

BOOL CInput::iGetAsyncKeyState( int dik )
{
	return !!KBState[dik];
}

BOOL CInput::iGetAsyncBtnState( int btn )
{
	return !!mouseState[btn];
}

void CInput::MouseUpdate( )
{
	HRESULT hr;
	DWORD dwElements	= MOUSEBUFFERSIZE;
	DIDEVICEOBJECTDATA	od[MOUSEBUFFERSIZE];

	VERIFY(pMouse);

	hr = pMouse->GetDeviceData( sizeof(DIDEVICEOBJECTDATA), &od[0], &dwElements, 0 );
	if (( hr == DIERR_INPUTLOST )||( hr == DIERR_NOTACQUIRED )){
		hr = pMouse->Acquire();
		if ( hr != S_OK ) return;
		hr = pMouse->GetDeviceData( sizeof(DIDEVICEOBJECTDATA), &od[0], &dwElements, 0 );
		if ( hr != S_OK ) return;
	};

	offs[0] = offs[1] = 0;
	for (DWORD i = 0; i < dwElements; i++){
		switch (od[i].dwOfs){
		case DIMOFS_X:	offs[0]	+= od[i].dwData; timeStamp[0] = od[i].dwTimeStamp;	break;
		case DIMOFS_Y:	offs[1]	+= od[i].dwData; timeStamp[1] = od[i].dwTimeStamp;	break;
		case DIMOFS_BUTTON0:
			if ( od[i].dwData & 0x80 )	{ mouseState[0] = TRUE;				cbStack.top()->OnMousePress(0);		}
			if ( !(od[i].dwData & 0x80)){ mouseState[0] = FALSE;			cbStack.top()->OnMouseRelease(0);	}
			break;
		case DIMOFS_BUTTON1:
			if ( od[i].dwData & 0x80 )	{ mouseState[1] = TRUE;				cbStack.top()->OnMousePress(1);		}
			if ( !(od[i].dwData & 0x80)){ mouseState[1] = FALSE;			cbStack.top()->OnMouseRelease(1);	}
			break;
		}
	}

	if (mouseState[0]) 		cbStack.top()->OnMouseHold(0);
	if (mouseState[1])		cbStack.top()->OnMouseHold(1);

	if ( dwElements ){
		if (offs[0] || offs[1]) cbStack.top()->OnMouseMove( offs[0], offs[1] );
	} else {
		if (timeStamp[1] && ((dwCurTime-timeStamp[1])>=mouse_property.mouse_dt))	cbStack.top()->OnMouseStop(DIMOFS_Y, timeStamp[1] = 0);
		if (timeStamp[0] && ((dwCurTime-timeStamp[0])>=mouse_property.mouse_dt))	cbStack.top()->OnMouseStop(DIMOFS_X, timeStamp[0] = 0);
	}
}

//-------------------------------------------------------
void CInput::iCapture(CController *p)
{
	VERIFY(p);
	if (pMouse) 	MouseUpdate();
    if (pKeyboard) 	KeyUpdate();

    // change focus
	if (!cbStack.empty())
		cbStack.top()->OnInputDeactivate();
	cbStack.push(p);
	cbStack.top()->OnInputActivate();

	// prepare for new controller
	ZeroMemory			( timeStamp,	sizeof(timeStamp) );
	ZeroMemory			( timeSave,		sizeof(timeStamp) );
	ZeroMemory			( offs,			sizeof(offs) );
}

void CInput::iRelease(CController *p)
{
	if (p == cbStack.top())
	{
		cbStack.top()->OnInputDeactivate();
		cbStack.pop();
		cbStack.top()->OnInputActivate();
	}
}

void CInput::OnAppActivate		(void)
{
	SetAllAcquire	( true );
	ZeroMemory		( mouseState,	sizeof(mouseState) );
	ZeroMemory		( KBState,		sizeof(KBState) );
	ZeroMemory		( timeStamp,	sizeof(timeStamp) );
	ZeroMemory		( timeSave,		sizeof(timeStamp) );
	ZeroMemory		( offs,			sizeof(offs) );
}

void CInput::OnAppDeactivate	(void)
{
	SetAllAcquire	( false );
	ZeroMemory		( mouseState,	sizeof(mouseState) );
	ZeroMemory		( KBState,		sizeof(KBState) );
	ZeroMemory		( timeStamp,	sizeof(timeStamp) );
	ZeroMemory		( timeSave,		sizeof(timeStamp) );
	ZeroMemory		( offs,			sizeof(offs) );
}

void CInput::OnFrame			(void)
{
	Device.Statistic.Input.Begin	();
	dwCurTime = Device.TimerAsyncMM	();
	if (pKeyboard)	KeyUpdate		();
	if (pMouse)		MouseUpdate		();
	Device.Statistic.Input.End		();
}
