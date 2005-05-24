struct BulletData 
{
	shared_str		FirerName;
	shared_str		WeaponName;

	u8				HitRefCount;
	bool			Removed;

	SBullet			Bullet;
	bool			operator	==		(u32 BulletID)	{return BulletID == Bullet.m_dwID;};
	bool			operator	!=		(u32 BulletID)	{return BulletID != Bullet.m_dwID;};

	BulletData		(shared_str FName, shared_str WName, SBullet* pBullet);
};

DEF_VECTOR		(ABULLETS, BulletData);

struct HitData
{
	Fvector Pos0;
	Fvector Pos1;
	
	s16		BoneID;
	shared_str	TargetName;
	u32		BulletID;
	bool	Deadly;

	bool	Completed;

	void			net_save			(NET_Packet* P);
	void			net_load			(NET_Packet* P);

	bool			operator	==		(u32 ID)	{return ID == BulletID;};
	bool			operator	!=		(u32 ID)	{return ID != BulletID;};
};

DEF_VECTOR	(HITS_VEC, HitData);

struct Weapon_Statistic
{
	shared_str		WName;
	shared_str		InvName;
	u32				NumBought;
	u32				RoundsFired;
	u32				BulletsFired;
	u32				HitsScored;
	u32				KillsScored;
	u32				m_Basket[34];

	u32				m_dwNumCompleted;
	HITS_VEC		m_Hits;
	

	Weapon_Statistic(LPCSTR Name);
	~Weapon_Statistic();

	void			net_save			(NET_Packet* P);
	void			net_load			(NET_Packet* P);

	bool			FindHit				(u32 BulletID, HITS_VEC_it& Hit_it);
	bool			operator	==		(LPCSTR name){int res = xr_strcmp(WName.c_str(), name);return	res	 == 0;}
};

DEF_VECTOR		(WEAPON_STATS, Weapon_Statistic);

struct Player_Statistic
{
	shared_str		PName;
	u32				TotalShots;

	WEAPON_STATS	aWeaponStats;

	u32				m_dwTotalAliveTime;
	s32				m_dwTotalMoneyRound;
	u32				m_dwNumRespawned;
	//-----------------------------------------------
	u32				m_dwCurMoneyRoundDelta;

	Player_Statistic(LPCSTR Name);
	~Player_Statistic();

	WEAPON_STATS_it	FindPlayersWeapon	(LPCSTR WeaponName);

	void			net_save			(NET_Packet* P);
	void			net_load			(NET_Packet* P);

	bool			operator	==		(LPCSTR name){int res = xr_strcmp(PName.c_str(), name);return	res	 == 0;}
};

DEF_VECTOR	(PLAYERS_STATS, Player_Statistic);

struct Bullet_Check_Request
{
	u32	BulletID;
	s16 BoneID;
	bool Result;
	bool Processed;
	Bullet_Check_Request() : BulletID(0), Result(false) {};
	Bullet_Check_Request(u32 ID, s16 BID) : BulletID(ID), BoneID(BID), Result(false), Processed(false) {};
};

DEF_VECTOR(BChR, Bullet_Check_Request);

struct Bullet_Check_Array
{
	u32	SenderID;	

	BChR	Requests;
	u8		NumTrue;
	u8		NumFalse;

	bool			operator	==		(u32 ID){return	ID == SenderID;}
	bool			operator	!=		(u32 ID){return	ID != SenderID;}
	Bullet_Check_Array(u32 ID) : SenderID(ID) {Requests.clear(); NumTrue = 0; NumFalse = 0;};
	~Bullet_Check_Array () {Requests.clear_and_free();};
};	

DEF_VECTOR(BChA, Bullet_Check_Array);

struct WeaponUsageStatistic {

	bool			m_bCollectStatistic;
	bool			CollectData				() {return m_bCollectStatistic;};
	void			SetCollectData			(bool Collect);
	//-----------------------------------------------
	ABULLETS		ActiveBullets;
	//-----------------------------------------------
	PLAYERS_STATS	aPlayersStatistic;
	//-----------------------------------------------
	u32				m_dwTotalPlayersAliveTime;
	s32				m_dwTotalPlayersMoneyRound;
	u32				m_dwTotalNumRespawns;
	//-----------------------------------------------
	u32				m_dwLastUpdateTime;
	u32				m_dwUpdateTimeDelta;
	//-----------------------------------------------
	WeaponUsageStatistic();
	~WeaponUsageStatistic();

	void				Clear			();

	PLAYERS_STATS_it					FindPlayer			(LPCSTR PlayerName);
	bool								GetPlayer			(LPCSTR PlayerName, PLAYERS_STATS_it& pPlayerI);	
	bool								FindBullet			(u32 BulletID, ABULLETS_it& Bullet_it);
	void								RemoveBullet		(ABULLETS_it& Bullet_it);
	//-----------------------------------------------
	void				OnWeaponBought			(game_PlayerState* ps, LPCSTR WeaponName);
	void				OnBullet_Fire			(SBullet* pBullet, const CCartridge& cartridge);
	void				OnBullet_Hit			(SBullet* pBullet, u16 TargetID, s16 element, Fvector HitLocation);
	void				OnBullet_Remove			(SBullet* pBullet);
	//-----------------------------------------------
	
	u32						m_dwLastRequestSenderID;

	BChA					m_Requests;

	void				OnBullet_Check_Request		(s16 iBoneID, NET_Packet* P);
	void				OnBullet_Check_Result		(bool Result);
	//-----------------------------------------------
	void				Send_Check_Respond			();
	void				On_Check_Respond			(NET_Packet* P);
	//-----------------------------------------------
	void				Draw						();
	//-----------------------------------------------
	void				OnPlayerKilled				(game_PlayerState* ps);
	void				OnPlayerSpawned				(game_PlayerState* ps);
	void				OnPlayerAddMoney			(game_PlayerState* ps, s32 MoneyAmount);
	//-----------------------------------------------
	void				Update						();
	void				OnUpdateRequest				(NET_Packet* P);
	void				OnUpdateRespond				(NET_Packet* P);
	//-----------------------------------------------
	void				SaveData					();
};

struct Bullet_Check_Respond_True
{
	u32	BulletID;
	s16 BoneID;
};