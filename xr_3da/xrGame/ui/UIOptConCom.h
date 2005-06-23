#pragma once

class CUIOptConCom {
public:
	void Init();
	CUIOptConCom();
protected:
	enum{
		flNetConnectionLan				= 1,
		flNetFilterEmpty				= 2,
		flNetFilterFull					= 4,
		flNetFilterWithPassword			= 8,
		flNetFilterWithoutPassword		= 16,
		flNetFilterWithoutFriendlyFire	= 32,
		flNetFilterWithoutPunkbusier	= 64,
		flNetFilterListenServers		= 128,
	};
	int	m_iMaxPlayers;
	Flags32 m_uNetFilters;

	u32			m_curGameMode;

	string64 m_playerName;

	int m_iNetConPublicServer;
	int m_iNetConMapRotation;
	int m_iNetConSpectatorOn;
	int m_iNetConSpectator;

};