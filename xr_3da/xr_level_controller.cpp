#include "stdafx.h"
#include "x_ray.h"
#include "xr_creator.h"
#include "xr_level_controller.h"
#include "xr_ioconsole.h"
#include "cameramanager.h"
#include "xr_input.h"
#include "xr_ioc_cmd.h"
#include "render.h"
#include "xr_object.h"
#include "std_classes.h"
#include "CustomHUD.h"

//************************************************ Input
ENGINE_API int key_binding	[ 2048	];

_keybind  keybind[] = {
	{ "left",			kLEFT		},	
	{ "right",			kRIGHT		},
	{ "up",				kUP			},	
	{ "down",			kDOWN		},
	{ "jump",			kJUMP		},	
	{ "crouch",			kCROUCH		},
	{ "accel",			kACCEL		},

	{ "forward",		kFWD		},	
	{ "back",			kBACK		},
	{ "lstrafe",		kL_STRAFE	},	
	{ "rstrafe",		kR_STRAFE	},

	{ "cam_1",			kCAM_1		},	
	{ "cam_2",			kCAM_2		},
	{ "cam_3",			kCAM_3		},	
	{ "cam_4",			kCAM_4		},
	{ "cam_zoom_in",	kCAM_ZOOM_IN},	
	{ "cam_zoom_out",	kCAM_ZOOM_OUT},

	{ "binocular",		kBINOCULAR	},
	{ "wpn_1",			kWPN_1		},
	{ "wpn_2",			kWPN_2		},
	{ "wpn_3",			kWPN_3		},
	{ "wpn_4",			kWPN_4		},
	{ "wpn_5",			kWPN_5		},
	{ "wpn_6",			kWPN_6		},
	{ "wpn_7",			kWPN_7		},
	{ "wpn_8",			kWPN_8		},
	{ "wpn_9",			kWPN_9		},
	{ "wpn_next",		kWPN_NEXT	},
	{ "wpn_prev",		kWPN_PREV	},
	{ "wpn_fire",		kWPN_FIRE	},
	{ "wpn_zoom",		kWPN_ZOOM	},
	{ "wpn_reload",		kWPN_RELOAD	},
	
	{ "use",			kUSE		},	
	{ "scores",			kSCORES		},	
	{ "chat",			kCHAT		},	
	{ "screenshot",		kSCREENSHOT	},	
	{ "quit",			kQUIT		},

	{ NULL, 			0			}
};

_keybind keynames[] = {
	{ "kESCAPE",	 	DIK_ESCAPE		},	{ "k1",				DIK_1			},
	{ "k2",				DIK_2			},	{ "k3",				DIK_3			},
	{ "k4",				DIK_4			},	{ "k5",				DIK_5			},
	{ "k6",				DIK_6			},	{ "k7",				DIK_7			},
	{ "k8",				DIK_8			},	{ "k9",				DIK_9			},
	{ "k0",				DIK_0			},	{ "kMINUS",			DIK_MINUS		},
	{ "kEQUALS",		DIK_EQUALS		},	{ "kBACK",			DIK_BACK		},
	{ "kTAB",			DIK_TAB 		},	{ "kQ",				DIK_Q			},
	{ "kW",				DIK_W			},	{ "kE",				DIK_E			},
	{ "kR",				DIK_R			},	{ "kT",				DIK_T			},
	{ "kY",				DIK_Y			},	{ "kU",				DIK_U			},
	{ "kI",				DIK_I			},	{ "kO",				DIK_O			},
	{ "kP",				DIK_P			},	{ "kLBRACKET",		DIK_LBRACKET	},
	{ "kRBRACKET",		DIK_RBRACKET	},	{ "kRETURN",		DIK_RETURN		},
	{ "kLCONTROL",		DIK_LCONTROL	},	{ "kA",				DIK_A			},
	{ "kS",				DIK_S			},	{ "kD",				DIK_D			},
	{ "kF",				DIK_F			},	{ "kG",				DIK_G			},
	{ "kH",				DIK_H			},	{ "kJ",				DIK_J			},
	{ "kK",				DIK_K			},	{ "kL",				DIK_L			},
	{ "kSEMICOLON",		DIK_SEMICOLON	},	{ "kAPOSTROPHE",	DIK_APOSTROPHE	},
	{ "kGRAVE",			DIK_GRAVE		},	{ "kLSHIFT",	 	DIK_LSHIFT		},
	{ "kBACKSLASH",		DIK_BACKSLASH	},	{ "kZ",				DIK_Z			},
	{ "kX",				DIK_X			},	{ "kC",				DIK_C			},
	{ "kV",				DIK_V			},	{ "kB",				DIK_B			},
	{ "kN",				DIK_N			},	{ "kM",				DIK_M			},
	{ "kCOMMA",			DIK_COMMA		},	{ "kPERIOD",		DIK_PERIOD		},
	{ "kSLASH",			DIK_SLASH		},	{ "kRSHIFT",		DIK_RSHIFT		},
	{ "kMULTIPLY",		DIK_MULTIPLY	},	{ "kLMENU",			DIK_LMENU		},
	{ "kSPACE",			DIK_SPACE		},	{ "kCAPITAL",		DIK_CAPITAL		},
	{ "kF1",			DIK_F1			},	{ "kF2",			DIK_F2			},
	{ "kF3",			DIK_F3			},	{ "kF4",			DIK_F4			},
	{ "kF5",			DIK_F5			},	{ "kF6",			DIK_F6			},
	{ "kF7",			DIK_F7			},	{ "kF8",			DIK_F8			},
	{ "kF9",			DIK_F9			},	{ "kF10",			DIK_F10			},
	{ "kNUMLOCK",		DIK_NUMLOCK		},	{ "kSCROLL",		DIK_SCROLL		},
	{ "kNUMPAD7",		DIK_NUMPAD7		},	{ "kNUMPAD8",		DIK_NUMPAD8		},
	{ "kNUMPAD9",		DIK_NUMPAD9		},	{ "kSUBTRACT",		DIK_SUBTRACT	},
	{ "kNUMPAD4",		DIK_NUMPAD4		},	{ "kNUMPAD5",		DIK_NUMPAD5		},
	{ "kNUMPAD6",		DIK_NUMPAD6		},	{ "kADD",			DIK_ADD			},
	{ "kNUMPAD1",		DIK_NUMPAD1		},	{ "kNUMPAD2",		DIK_NUMPAD2		},
	{ "kNUMPAD3",		DIK_NUMPAD3		},	{ "kNUMPAD0",		DIK_NUMPAD0		},
	{ "kDECIMAL",		DIK_DECIMAL		},	{ "kF11",			DIK_F11			},
	{ "kF12",			DIK_F12			},	{ "kF13",			DIK_F13			},
	{ "kF14",			DIK_F14			},	{ "kF15",			DIK_F15			},
	{ "kKANA",			DIK_KANA		},	{ "kCONVERT",		DIK_CONVERT		},
	{ "kNOCONVERT",		DIK_NOCONVERT	},	{ "kYEN",			DIK_YEN			},
	{ "kNUMPADEQUALS",	DIK_NUMPADEQUALS},	{ "kCIRCUMFLEX",	DIK_CIRCUMFLEX	},
	{ "kAT",			DIK_AT			},	{ "kCOLON",			DIK_COLON		},
	{ "kUNDERLINE",		DIK_UNDERLINE	},	{ "kKANJI",			DIK_KANJI		},
	{ "kSTOP",			DIK_STOP		},	{ "kAX",			DIK_AX			},
	{ "kUNLABELED",		DIK_UNLABELED	},	{ "kNUMPADENTER",	DIK_NUMPADENTER	},
	{ "kRCONTROL",		DIK_RCONTROL	},	{ "kNUMPADCOMMA",	DIK_NUMPADCOMMA	},
	{ "kDIVIDE",		DIK_DIVIDE		},	{ "kSYSRQ",			DIK_SYSRQ		},
	{ "kRMENU",			DIK_RMENU		},	{ "kHOME",			DIK_HOME		},
	{ "kUP",			DIK_UP			},	{ "kPRIOR",			DIK_PRIOR		},
	{ "kLEFT",			DIK_LEFT		},	{ "kRIGHT",			DIK_RIGHT		},
	{ "kEND",			DIK_END			},	{ "kDOWN",			DIK_DOWN		},
	{ "kNEXT",			DIK_NEXT		},	{ "kINSERT",		DIK_INSERT		},
	{ "kDELETE",		DIK_DELETE		},	{ "kLWIN",			DIK_LWIN		},
	{ "kRWIN",			DIK_RWIN		},	{ "kAPPS",			DIK_APPS		},
	{ "kPAUSE",			DIK_PAUSE		},	{ "mouse1",			MOUSE_1			},
	{ "mouse2",			MOUSE_2			},	{ "mouse3",			MOUSE_3			},
	{ NULL, 			0				}
};

//-----------------------------------------------------------------------
// Bind/Unbind/List
//-----------------------------------------------------------------------
class CCC_Bind : public CConsoleCommand
{
public:
	CCC_Bind(LPCSTR N) : CConsoleCommand(N) {};
	virtual void Execute(LPCSTR args) {
		BOOL	binded=false;
		char	action[256],key[256];

		sscanf(args,"%s %s",action,key);
		for (int i=0; keybind[i].name; i++) {
			if (_stricmp(keybind[i].name,action)==0) {
				// i = action num
				for (int j=0; keynames[j].name; j++) {
					if (_stricmp(keynames[j].name,key)==0) {
						// j = key
						binded=true;
						key_binding[keynames[j].DIK]=keybind[i].DIK;
					}
				}
			}
		}
		if (!binded) {
			Log("! Can't bind action: ", action);
			Log("! Syntax error. See 'list_actions'.");
		}
	}
	virtual void Save(FILE *F) {
		fprintf(F,"unbindall\n");
		for (int i=0; i<2048; i++) {
			if (key_binding[i]) {
				for (int j=0; keybind[j].name; j++) {
					if (keybind[j].DIK==key_binding[i]) {
						for (int k=0; keynames[k].name; k++) {
							if (keynames[k].DIK==i) {
								fprintf(F, "bind %s %s\n", keybind[j].name, keynames[k].name);
							}
						}
					}
				}
			}
		}
	}
};

class CCC_UnBind : public CConsoleCommand
{
public:
	CCC_UnBind(LPCSTR N) : CConsoleCommand(N) 
	{ bEmptyArgsHandled=TRUE; };
	virtual void Execute(LPCSTR args) {
		for (int i=0; keybind[i].name; i++) {
			if (_stricmp(keybind[i].name,args)==0) {
				// i = action num
				for (int j=0; j<2048; j++) {
					if (key_binding[j]==keybind[i].DIK) key_binding[j]=0;
				}
			}
		}
	}
};

class CCC_ListActions : public CConsoleCommand
{
public:
	CCC_ListActions(LPCSTR N) : CConsoleCommand(N)
	{ bEmptyArgsHandled=TRUE; };

	virtual void Execute(LPCSTR args) {
		Log("- --- Action list start ---");
		for (int i=0; keybind[i].name; i++) {
			Log("-",keybind[i].name);
		}
		Log("- --- Action list end   ---");
	}
};

class CCC_UnBindAll : public CConsoleCommand
{
public:
	CCC_UnBindAll(LPCSTR N) : CConsoleCommand(N)
	{ bEmptyArgsHandled=TRUE; };

	virtual void Execute(LPCSTR args) {
		ZeroMemory(key_binding,sizeof(key_binding));
	}
};

void CCC_RegisterInput()
{
	CMD1(CCC_Bind,		"bind"					);
	CMD1(CCC_UnBind,	"unbind"				);
	CMD1(CCC_UnBindAll,	"unbindall"				);
	CMD1(CCC_ListActions,"list_actions"			);
};
