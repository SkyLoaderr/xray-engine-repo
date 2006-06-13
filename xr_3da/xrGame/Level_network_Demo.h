private:
	string1024					m_sDemoName;

	//------------- Demo Play	---------------------------------------
	BOOL						m_bDemoPlayMode;
	BOOL						m_bDemoPlayByFrame;

	xr_string					m_sDemoFileName;
	long						m_lDemoOfs;

	enum	DEMO_CHUNK
	{
		DATA_FRAME		= u32(0),
		DATA_PACKET,

		DATA_DUMMY		= u32(-1),
	};

	struct DemoFrameTime {
		float									fTimeDelta;
		float									fTimeGlobal;
		u32										dwTimeDelta;
		u32										dwTimeGlobal;
		u32										dwTimeServer;
		u32										dwTimeServer_Delta;
	};

	struct DemoDataStruct {
		u32			m_dwDataType;
		u32			m_dwFrame;
		u32			m_dwTimeReceive;
		union		{
			NET_Packet	Packet;
			DemoFrameTime	FrameTime;
		};
	};

	DEF_DEQUE(DemoDeque, DemoDataStruct);
	DemoDeque					m_aDemoData;
	void						Demo_Load				(LPCSTR DemoName);
	void						Demo_Load_toFrame		(LPCSTR FileName, DWORD toFrame, long &ofs);
	BOOL						m_bDemoStarted;	
	u32							m_dwLastDemoFrame;
	void						Demo_Update				();


	//------------- Demo Store -----------------------------------------
	BOOL						m_bDemoSaveMode;

	xrCriticalSection			DemoCS;
	u32							m_dwStoredDemoDataSize;
	u8*							m_pStoredDemoData;
		
	void						Demo_PrepareToStore		();
	void						Demo_StoreData			(void* data, u32 size, DEMO_CHUNK DataType);
	void						Demo_DumpData			();
	void						Demo_Clear				();

	crashhandler*				m_pOldCrashHandler;
	bool						m_we_used_old_crach_handler;

	u32							m_dwCurDemoFrame;
	void						Demo_StartFrame			();
	void						Demo_EndFrame			();
	
public:
	void						CallOldCrashHandler		();
	void						WriteStoredDemo			();

	BOOL						IsDemoPlay				()	{return (m_bDemoPlayMode && !m_bDemoSaveMode);};
	BOOL						IsDemoSave				()	{return (m_bDemoSaveMode && !m_bDemoPlayMode && IsClient());};

	virtual	NET_Packet*			net_msg_Retreive		();
private: