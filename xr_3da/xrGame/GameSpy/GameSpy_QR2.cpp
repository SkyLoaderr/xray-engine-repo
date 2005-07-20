#include "StdAfx.h"
#include "GameSpy_QR2.h"
#include "GameSpy_Base_Defs.h"

#include "GameSpy_QR2_callbacks.h"

CGameSpy_QR2::CGameSpy_QR2()
{
	m_SecretKey[0] = 't';
	m_SecretKey[1] = '9';
	m_SecretKey[2] = 'F';
	m_SecretKey[3] = 'j';
	m_SecretKey[4] = '3';
	m_SecretKey[5] = 'M';
	m_SecretKey[6] = 'x';
	m_SecretKey[7] = '\0';
	//-------------------------------
	hGameSpyDLL = NULL;

	LoadGameSpy();
};

CGameSpy_QR2::~CGameSpy_QR2()
{
	if (hGameSpyDLL)
	{
		FreeLibrary(hGameSpyDLL);
		hGameSpyDLL = NULL;
	}
};

void	CGameSpy_QR2::LoadGameSpy()
{
	LPCSTR			g_name	= "xrGameSpy.dll";
	Log				("Loading DLL:",g_name);
	hGameSpyDLL			= LoadLibrary	(g_name);
	if (0==hGameSpyDLL)	R_CHK			(GetLastError());
	R_ASSERT2		(hGameSpyDLL,"GameSpy DLL raised exception during loading or there is no game DLL at all");

	GAMESPY_LOAD_FN(xrGS_RegisteredKey);
	GAMESPY_LOAD_FN(xrGS_qr2_register_key);
	GAMESPY_LOAD_FN(xrGS_qr2_think);
	GAMESPY_LOAD_FN(xrGS_qr2_shutdown);
	GAMESPY_LOAD_FN(xrGS_qr2_buffer_add);
	GAMESPY_LOAD_FN(xrGS_qr2_buffer_add_int);
	GAMESPY_LOAD_FN(xrGS_qr2_keybuffer_add);

	GAMESPY_LOAD_FN(xrGS_qr2_register_natneg_callback);
	GAMESPY_LOAD_FN(xrGS_qr2_register_clientmessage_callback);
	GAMESPY_LOAD_FN(xrGS_qr2_register_publicaddress_callback);

	GAMESPY_LOAD_FN(xrGS_qr2_init);
}

void	CGameSpy_QR2::Think	(void* qrec)
{
	xrGS_qr2_think(qrec);
}
void	CGameSpy_QR2::ShutDown	(void* qrec)
{
	xrGS_qr2_shutdown(qrec);	
}

void	CGameSpy_QR2::RegisterAdditionalKeys	()
{
	xrGS_qr2_register_key(DEDICATED_KEY,   ("dedicated")  );
	xrGS_qr2_register_key(FFIRE_KEY,   ("friendlyfire")  );
	xrGS_qr2_register_key(FFIREAMOUNT_KEY,   ("friendlyfireamount")  );
	xrGS_qr2_register_key(DAMAGEBLOCKIND_KEY,   ("dmgblockind")  );
};

bool	CGameSpy_QR2::Init		(int Public, void* instance)
{	
	//--------- QR2 Init -------------------------/
	//call qr_init with the query port number and gamename, default IP address, and no user data
	if (xrGS_qr2_init(NULL,NULL,GAMESPY_BASEPORT, GAMESPY_GAMENAME, m_SecretKey, Public, 1,
		callback_serverkey, callback_playerkey, callback_teamkey,
		callback_keylist, callback_count, callback_adderror, instance) != e_qrnoerror)
	{
		//		_tprintf(_T("Error starting query sockets\n"));
		Msg("GameSpy::QR2 : Failes to Initialize!");
		return false;
	}
	RegisterAdditionalKeys();

	// Set a function to be called when we receive a game specific message
	xrGS_qr2_register_clientmessage_callback(NULL, callback_cm);

	// Set a function to be called when we receive a nat negotiation request
	xrGS_qr2_register_natneg_callback(NULL, callback_nn);

	Msg("GameSpy::QR2 : Initialized");
	return true;
};

void	CGameSpy_QR2::BufferAdd		(void* outbuf, const char* value)
{
	xrGS_qr2_buffer_add(outbuf, value);
};

void	CGameSpy_QR2::BufferAdd_Int	(void* outbuf, int value)
{
	xrGS_qr2_buffer_add_int(outbuf, value);
};

void	CGameSpy_QR2::KeyBufferAdd	(void* keybuffer, int keyid)
{
	xrGS_qr2_keybuffer_add(keybuffer, keyid);
}