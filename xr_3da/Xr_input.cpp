#include "stdafx.h"
#pragma hdrstop

#include "xr_input.h"
#include "IInputReceiver.h"

CInput *	pInput	= NULL;
IInputReceiver		dummyController;

ENGINE_API float	psMouseSens			= 1.f;
ENGINE_API float	psMouseSensScale	= 1.f;
ENGINE_API Flags32	psMouseInvert		= {FALSE};

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
	iCapture	(&dummyController);

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
	Device.seqFrame.Add				(this, REG_PRIORITY_HIGH);
#endif
}

CInput::~CInput(void)
{
#ifdef ENGINE_BUILD
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
HRESULT CInput::CreateInputDevice( LPDIRECTINPUTDEVICE7* device, GUID guidDevice, const DIDATAFORMAT* pdidDataFormat, u32 dwFlags, u32 buf_size )
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

	for (u32 i = 0; i < dwElements; i++){
		key					= od[i].dwOfs;
		KBState[key]		= od[i].dwData & 0x80;
		if ( KBState[key])	cbStack.back()->IR_OnKeyboardPress	( key );
		if (!KBState[key])	cbStack.back()->IR_OnKeyboardRelease	( key );
	}
	for ( i = 0; i < COUNT_KB_BUTTONS; i++ )
		if (KBState[i]) cbStack.back()->IR_OnKeyboardHold( i );
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

	offs[0] = offs[1] = offs[2] = 0;
	for (u32 i = 0; i < dwElements; i++){
		switch (od[i].dwOfs){
		case DIMOFS_X:	offs[0]	+= od[i].dwData; timeStamp[0] = od[i].dwTimeStamp;	break;
		case DIMOFS_Y:	offs[1]	+= od[i].dwData; timeStamp[1] = od[i].dwTimeStamp;	break;
		case DIMOFS_Z:	offs[2]	+= od[i].dwData; timeStamp[2] = od[i].dwTimeStamp;	break;
		case DIMOFS_BUTTON0:
			if ( od[i].dwData & 0x80 )	{ mouseState[0] = TRUE;				cbStack.back()->IR_OnMousePress(0);		}
			if ( !(od[i].dwData & 0x80)){ mouseState[0] = FALSE;			cbStack.back()->IR_OnMouseRelease(0);	}
			break;
		case DIMOFS_BUTTON1:
			if ( od[i].dwData & 0x80 )	{ mouseState[1] = TRUE;				cbStack.back()->IR_OnMousePress(1);		}
			if ( !(od[i].dwData & 0x80)){ mouseState[1] = FALSE;			cbStack.back()->IR_OnMouseRelease(1);	}
			break;
		case DIMOFS_BUTTON2:
			if ( od[i].dwData & 0x80 )	{ mouseState[2] = TRUE;				cbStack.back()->IR_OnMousePress(2);		}
			if ( !(od[i].dwData & 0x80)){ mouseState[2] = FALSE;			cbStack.back()->IR_OnMouseRelease(2);	}
			break;
		}
	}

	if (mouseState[0]) 		cbStack.back()->IR_OnMouseHold(0);
	if (mouseState[1])		cbStack.back()->IR_OnMouseHold(1);
	if (mouseState[2])		cbStack.back()->IR_OnMouseHold(2);

	if ( dwElements ){
		if (offs[0] || offs[1]) cbStack.back()->IR_OnMouseMove	( offs[0], offs[1] );
		if (offs[2])			cbStack.back()->IR_OnMouseWheel	( offs[2] );
	} else {
		if (timeStamp[1] && ((dwCurTime-timeStamp[1])>=mouse_property.mouse_dt))	cbStack.back()->IR_OnMouseStop(DIMOFS_Y, timeStamp[1] = 0);
		if (timeStamp[0] && ((dwCurTime-timeStamp[0])>=mouse_property.mouse_dt))	cbStack.back()->IR_OnMouseStop(DIMOFS_X, timeStamp[0] = 0);
	}
}

//-------------------------------------------------------
void CInput::iCapture(IInputReceiver *p)
{
	VERIFY(p);
	if (pMouse) 	MouseUpdate();
    if (pKeyboard) 	KeyUpdate();

    // change focus
	if (!cbStack.empty())
		cbStack.back()->IR_OnDeactivate();
	cbStack.push_back(p);
	cbStack.back()->IR_OnActivate();

	// prepare for _new_ controller
	ZeroMemory			( timeStamp,	sizeof(timeStamp) );
	ZeroMemory			( timeSave,		sizeof(timeStamp) );
	ZeroMemory			( offs,			sizeof(offs) );
}

void CInput::iRelease(IInputReceiver *p)
{
	if (p == cbStack.back())
	{
		cbStack.back()->IR_OnDeactivate();
		cbStack.pop_back();
		IInputReceiver * ir = cbStack.back();
		ir->IR_OnActivate();
	}else{// we are not topmost receiver, so remove the nearest one
		u32 cnt = cbStack.size();
		for(;cnt>0;--cnt)
			if( cbStack[cnt-1] == p ){
				xr_vector<IInputReceiver*>::iterator it = cbStack.begin();
				std::advance	(it,cnt-1);
				cbStack.erase	(it);
				break;
			}
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
	Device.Statistic->Input.Begin			();
	dwCurTime		= Device.TimerAsync_MMT	();
	if (pKeyboard)	KeyUpdate				();
	if (pMouse)		MouseUpdate				();
	Device.Statistic->Input.End				();
}

IInputReceiver*	 CInput::CurrentIR()
{
	if(cbStack.size())
		return cbStack.back();
	else
		return NULL;
}
