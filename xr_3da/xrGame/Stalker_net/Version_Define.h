//#define TESTERS

//#define	GAME_NAME		"gmtest"//stalkersc"

#ifdef DEMO_BUILD
	#define	GAMESPY_GAMENAME		"stalkerscd"
	#define GAMESPY_GAMEID			1567
	#define GAMESPY_PRODUCTID		10954
#else
	#define	GAMESPY_GAMENAME		"stalkersc"
	#define GAMESPY_GAMEID			1067
	#define GAMESPY_PRODUCTID		10953
#endif

#define	SECRET_KEY		"t9Fj3Mx"
#define MAX_UPDATES		20
//SB - timer ID and frequency
#define TIMER_ID         100
#define TIMER_FREQUENCY  10
//SB - starting and ending port for LAN game searches
#define START_PORT       5445
#define END_PORT		START_PORT + 100 //GameSpy only process 500 ports

#define MAX_PLAYERNAME_LEN		17
#define MAX_SERVERNAME_LEN		24
#define MAX_SERVERPASSW_LEN		8

#ifdef	TESTERS

#define	LTX			"-ltx user_testers.ltx"
#define	MAP_LIST	"mp\\map_list_testers.ltx"
#define ICON_ID		IDR_MAINFRAME1
#define WND_NAME	"Stalknet_Testers"

#undef	TESTERS
#else

#define	LTX			"-ltx user.ltx"
#define	MAP_LIST	"mp\\map_list.ltx"
#define ICON_ID		IDR_MAINFRAME
#define WND_NAME	"Stalker_Net"

#endif

#define			MAPROT_LIST_NAME		"maprot_list.ltx"

//--------------- CD key -------------------------------
#define CHECK_CDKEY			1
#define	REGISTRY_CDKEY_STR	"Software\\S.T.A.L.K.E.R\\CDKey"