#include "GameSpy/QR2/qr2.h"
#include "GameSpy/CDKey/gcdkeys.h"

// Called when a server key needs to be reported
void callback_serverkey(int keyid, qr2_buffer_t outbuf, void *userdata);
// Called when a player key needs to be reported
void callback_playerkey(int keyid, int index, qr2_buffer_t outbuf, void *userdata);
// Called when a team key needs to be reported
void callback_teamkey(int keyid, int index, qr2_buffer_t outbuf, void *userdata);
// Called when we need to report the list of keys we report values for
void callback_keylist(qr2_key_type keytype, qr2_keybuffer_t keybuffer, void *userdata);
// Called when we need to report the number of players and teams
int  callback_count(qr2_key_type keytype, void *userdata);
// Called if our registration with the GameSpy master server failed
void callback_adderror(qr2_error_t error, gsi_char *errmsg, void *userdata);
// Called when a client wants to connect using nat negotiation
//   (Nat Negotiation must be enabled in qr2_init)
void callback_nn(int cookie, void *userdata);
// Called when a client sends a message to the server through qr2
//    (Not commonly used)
void callback_cm(gsi_char *data, int len, void *userdata);
