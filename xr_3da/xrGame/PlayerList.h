#pragma once

class CPlayers
{
public:
	struct Item
	{
		string64	name;
		int			score;
	};
	map<DWORD,Item>	items;
public:
	void			add		(NET_Packet& P);
	void			remove	(NET_Packet& P);
	Item&			access	(DWORD ID);

	CPlayers(void);
	~CPlayers(void);
};
