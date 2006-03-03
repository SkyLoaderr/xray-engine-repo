#pragma once

class CUIOptConCom {
public:
	void Init();
	CUIOptConCom();
protected:
	enum{
		flNetSrvDedicated				= 1,
		flNetConPublicServer			= 2,
		flNetConMaprotation				= 4,
		flNetConSpectatorOn				= 8,
		flNetConAllowVoting				= 16,
		flNetDamageBlockIndicator		= 32,
		flNetAutoTeamBalance			= 64,
		flNetFriendlyIndicators			= 128,
		flNetFriendlyNames				= 256,
		flNetNoAnomalies				= 512,

		flNetSpecTeamOnly				= 1024,
		flNetSpecFreeFly				= 2048,
		flNetSpecFirstEye				= 4096,
		flNetSpecLookAt					= 8192,
		flNetSpecFreeLook				= 16384
	};
    int			m_iMaxPlayers;
	Flags32		m_uNetSrvParams;
	u32			m_curGameMode;
	string64	m_playerName;
	string64	m_serverName;
	string64	m_maprotlist;
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
	int			m_iNetWarmUpTime;
	int			m_iNetWeatherRate;
	int			m_iNetRespawnCur;
};