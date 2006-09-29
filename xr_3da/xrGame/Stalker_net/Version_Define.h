//#define TESTERS

//#define	GAME_NAME		"gmtest"//stalkersc"
#define	GAME_NAME		"stalkersc"
//#define	SECRET_KEY		"HA6zkS"//t9Fj3Mx"
#define	SECRET_KEY		"t9Fj3Mx"
#define MAX_UPDATES		10
//SB - timer ID and frequency
#define TIMER_ID         100
#define TIMER_FREQUENCY  10
//SB - starting and ending port for LAN game searches
#define START_PORT       5445
#define END_PORT         65535//(START_PORT + 200) //GameSpy only process 500 ports

#ifdef	TESTERS

#define	LTX			"-ltx user_testers.ltx"
#define	MAP_LIST	"mp\\map_list_testers.ltx"
#define ICON_ID		IDR_MAINFRAME1
#define WND_NAME	"Stalknet_Testers"

#undef	TESTERS
#else

#define	LTX			"-ltx user_net.ltx"
#define	MAP_LIST	"mp\\map_list.ltx"
#define ICON_ID		IDR_MAINFRAME
#define WND_NAME	"Stalker_Net"

#endif

//--------------- CD key -------------------------------
#define CHECK_CDKEY			1
#define	REGISTRY_CDKEY_STR	"Software\\S.T.A.L.K.E.R\\CDKey"