#pragma once



	//структура данных по оружию
	struct		WeaponDataStruct
	{
		u16				SlotItem_ID		;    //SlotID << 8 | ItemID;
		std::string		WeaponName		;
		u16				Cost			;
		bool			operator	==		(s16 ID){return		(SlotItem_ID == ID);}
		bool			operator	==		(LPCSTR name){int res = xr_strcmp(WeaponName.c_str(), name);return	res	 == 0;}
	};

	DEF_VECTOR(TEAM_WPN_LIST, WeaponDataStruct);

	// ¬ектор имен скинов комманды
	DEF_VECTOR(TEAM_SKINS_NAMES, std::string);	

	// ¬ектор имен скинов комманды
	DEF_VECTOR(DEF_ITEMS_LIST, u16);	

	//структура данных по команде
	struct		TeamStruct
	{
		string256			caSection;		// им€ секции комманды
		TEAM_SKINS_NAMES	aSkins;			// список скинов дл€ команды
		TEAM_WPN_LIST		aWeapons;		// список оружи€ дл€ команды
		DEF_ITEMS_LIST		aDefaultItems;	// список предметов по умолчанию

		//---- Money -------------------
		s16					m_iM_Start			;
		s16					m_iM_OnRespawn		;
		s16					m_iM_Min			;
		
		s16					m_iM_KillRival		;
		s16					m_iM_KillSelf		;
		s16					m_iM_KillTeam		;

		s16					m_iM_TargetRival	;
		s16					m_iM_TargetTeam		;
		s16					m_iM_TargetSucceed	;
		s16					m_iM_TargetSucceedAll	;

		s16					m_iM_RoundWin		;
		s16					m_iM_RoundLoose		;
		s16					m_iM_RoundDraw		;		

		s16					m_iM_RoundWin_Minor		;
		s16					m_iM_RoundLoose_Minor	;
		s16					m_iM_RivalsWipedOut		;

	};

	//массив данных по командам
	DEF_DEQUE(TEAM_DATA_LIST, TeamStruct);



