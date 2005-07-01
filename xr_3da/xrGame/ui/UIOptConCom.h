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
	enum{
		flNetSrvDedicated				= 1,
		flNetConPublicServer			= 2,
		flNetConMaprotation				= 4,
		flNetConSpectatorOn				= 8,
		flNetConAllowVoting				= 16,
		flNetDamageBlockIndicator		= 32,
		flNetAutoTeamBalance			= 64,
		flNetFriendlyIndicators			= 128,
		flNetNoAnomalies				= 256
	};
    int			m_iMaxPlayers;
	Flags32		m_uNetFilters;
	Flags32		m_uNetSrvParams;
	u32			m_curGameMode;
	string64	m_playerName;
	string64	m_serverName;
	int			m_iNetConSpectator;
	int			m_iFriendlyFire;
	int			m_iArtefactsNum;
	int			m_iNetForceRespawn;
	int			m_iNetReinforcement;
	int			m_iNetFreezeTime;
	int			m_iNetDamageBlock;
	int			m_iNetFragLimit;
	int			m_iNetTimeLimit;
	int			m_iNetArtefactStay;
	int			m_iNetArtefactDelay;
	int			m_iNetAnomalyTime;
	int			m_iNetWeatherRate;
	int			m_iNetRespawnCur;


//	int m_iNetConPublicServer;
//	int m_iNetConMapRotation;
//	int m_iNetConSpectatorOn;
	

};