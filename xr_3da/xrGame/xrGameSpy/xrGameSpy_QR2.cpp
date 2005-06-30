#include "stdafx.h"
#include "windows.h"
#include "xrGameSpy_MainDefs.h"

#include "xrGameSpy_QR2.h"

XRGAMESPY_API const char* xrGS_RegisteredKey(DWORD KeyID)
{
	return qr2_registered_key_list[KeyID];
};

XRGAMESPY_API void xrGS_qr2_register_key(int keyid, const gsi_char *key)
{
	qr2_register_key(keyid, key);
}

XRGAMESPY_API void xrGS_qr2_think(qr2_t qrec)
{
	qr2_think(qrec);
};

XRGAMESPY_API void xrGS_qr2_shutdown (qr2_t qrec)
{
	qr2_shutdown(qrec);
};

XRGAMESPY_API void xrGS_qr2_buffer_add(qr2_buffer_t outbuf, const char *value)
{
	qr2_buffer_add(outbuf, value);
};

XRGAMESPY_API void xrGS_qr2_buffer_add_int(qr2_buffer_t outbuf, int value)
{
	qr2_buffer_add_int(outbuf, value);
}

XRGAMESPY_API void xrGS_qr2_keybuffer_add(qr2_keybuffer_t keybuffer, int keyid)
{
	qr2_keybuffer_add(keybuffer, keyid);
}

XRGAMESPY_API void xrGS_qr2_register_natneg_callback (qr2_t qrec, qr2_natnegcallback_t nncallback)
{
	qr2_register_natneg_callback (qrec, nncallback);
};
XRGAMESPY_API void xrGS_qr2_register_clientmessage_callback (qr2_t qrec, qr2_clientmessagecallback_t cmcallback)
{
	qr2_register_clientmessage_callback (qrec, cmcallback);
};
XRGAMESPY_API void xrGS_qr2_register_publicaddress_callback (qr2_t qrec, qr2_publicaddresscallback_t pacallback)
{
	qr2_register_publicaddress_callback (qrec, pacallback);
};

XRGAMESPY_API qr2_error_t xrGS_qr2_init(/*[out]*/qr2_t *qrec, const gsi_char *ip, int baseport, const gsi_char *gamename, const gsi_char *secret_key,
			   int ispublic, int natnegotiate,
			   qr2_serverkeycallback_t server_key_callback,
			   qr2_playerteamkeycallback_t player_key_callback,
			   qr2_playerteamkeycallback_t team_key_callback,
			   qr2_keylistcallback_t key_list_callback,
			   qr2_countcallback_t playerteam_count_callback,
			   qr2_adderrorcallback_t adderror_callback,
			   void *userdata)
{
	return qr2_init(qrec, ip, baseport, gamename, secret_key,
		ispublic, natnegotiate,
		server_key_callback,
		player_key_callback,
		team_key_callback,
		key_list_callback,
		playerteam_count_callback,
		adderror_callback,
		userdata);
}