#ifndef __XR_INPUT__
#define __XR_INPUT__

class CController;

#define	MouseHWScale			3/4

//------------------------------------------------------------------
//обработчики
//---------------------------------------------------------------------------------------
#define	COUNT_MOUSE_BUTTONS		3
#define	COUNT_MOUSE_AXIS		2
#define	COUNT_KB_BUTTONS		256

//\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//------------------------------------------------------------------
//описание класса
//------------------------------------------------------------------
const int mouse_device_key		= 1;
const int keyboard_device_key	= 2;
const int all_device_key		= mouse_device_key | keyboard_device_key;
const int default_key			= mouse_device_key | keyboard_device_key ;

struct sxr_mouse
{
	DIDEVCAPS					capabilities;
	DIDEVICEINSTANCE			deviceInfo;
	DIDEVICEOBJECTINSTANCE		objectInfo;
	DWORD						mouse_dt;
};

ENGINE_API extern float psMouseSens;
ENGINE_API extern float psMouseSensScale;
ENGINE_API extern DWORD psMouseInvert;

struct sxr_key
{
	DIDEVCAPS					capabilities;
	DIDEVICEINSTANCE			deviceInfo;
	DIDEVICEOBJECTINSTANCE		objectInfo;
};

class ENGINE_API CInput :
	public pureFrame,
	public pureAppActivate,
	public pureAppDeactivate
{
private:
	LPDIRECTINPUT7				pDI;			// The DInput object
	LPDIRECTINPUTDEVICE7		pMouse;			// The DIDevice2 interface
	LPDIRECTINPUTDEVICE7		pKeyboard;		// The DIDevice2 interface
	//----------------------
	DWORD						timeStamp	[COUNT_MOUSE_AXIS];
	DWORD						timeSave	[COUNT_MOUSE_AXIS];
	int 						offs		[COUNT_MOUSE_AXIS];
	BOOL						mouseState	[COUNT_MOUSE_BUTTONS];

	//----------------------
	BOOL						KBState		[COUNT_KB_BUTTONS];

	HRESULT						CreateInputDevice(	LPDIRECTINPUTDEVICE7* device, GUID guidDevice,
													const DIDATAFORMAT* pdidDataFormat, DWORD dwFlags,
													DWORD buf_size );

	stack<CController*>			cbStack;

	void						MouseUpdate					( );
	void						KeyUpdate					( );

public:
	sxr_mouse					mouse_property;
	sxr_key						key_property;
	DWORD						dwCurTime;

	void						SetAllAcquire				( BOOL bAcquire = TRUE );
	void						SetMouseAcquire				( BOOL bAcquire );
	void						SetKBDAcquire				( BOOL bAcquire );

	void						iCapture					( CController *pc );
	void						iRelease					( CController *pc );
	BOOL						iGetAsyncKeyState			( int dik );
	BOOL						iGetAsyncBtnState			( int btn );
	void						iGetLastMouseDelta			( Ipoint& p ){p.set(offs[0],offs[1]);}

	CInput						( BOOL bExclusive = true, int deviceForInit = default_key);
	~CInput						( );

	virtual void				OnFrame						(void);
	virtual void				OnAppActivate				(void);
	virtual void				OnAppDeactivate				(void);
};

extern ENGINE_API CInput *pInput;

#endif //__XR_INPUT__

