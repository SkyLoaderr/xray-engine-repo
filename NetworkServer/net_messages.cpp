#include "stdafx.h"
#include "net_messages.h"

MSG_DEF _messages[net_last] = {
	{ MSG_NONE,					"msg_none",					false	},
	{ SYS_CONNECT,				"sys_connect",				true	},
	{ SYS_DISCONNECT,			"sys_disconnect",			true	},
	{ M_CHAT,					"m_chat",					true	},
	{ M_ACTOR_UPDATE,			"m_actor_update",			true	},
	{ M_ACTOR_WEAPON_CHANGE,	"m_actor_weapon_change",	true	},
	{ M_ACTOR_FIRE_START,		"m_actor_fire_start",		true	},
	{ M_ACTOR_FIRE_STOP,		"m_actor_fire_stop",		true	},
	{ M_ACTOR_HIT_ANOTHER,		"m_actor_hit_another",		true	},
	{ M_ACTOR_RESPAWN,			"m_actor_respawn",			true	},
	{ M_ACTOR_TAKE_ITEM,		"m_actor_take_item",		true	}
};

vector<MSG_DEF*>		net_messages;

void net_RegisterMessages()
{
	MSG_DEF *pdef  = _messages;
	int		 count = net_last;
	MSG_DEF *plast = pdef+count;
	while (pdef<plast) {
		DWORD dwType = pdef->dwType;
		if (net_messages.size()<=dwType) {
			// resize net_messages
			net_messages.resize(dwType+2);
		}
		net_messages[dwType] = pdef;
		pdef++;
	}
}

void net_LogMessage(char *op, DWORD dwType) {
#ifdef DEBUG
	Msg("* NET [%8s] :: %s", op, net_messages[dwType]->pName);
#endif
}
