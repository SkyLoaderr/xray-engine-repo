private:
	string1024					m_sDemoName;

	//------------- Demo Play	---------------------------------------
	BOOL						m_bDemoPlayMode;	
	DEF_DEQUE(DemoDeque, NET_Packet);
	DemoDeque					m_aDemoData;
	void						Demo_Load				(LPCSTR DemoName);
	BOOL						m_bDemoStarted;	
	void						Demo_Update				();


	//------------- Demo Store -----------------------------------------
	BOOL						m_bDemoSaveMode;

	xrCriticalSection			DemoCS;
	u32							m_dwStoredDemoDataSize;
	u8*							m_pStoredDemoData;	
	
	void						Demo_PrepareToStore		();
	void						Demo_StoreData			(void* data, u32 size);
	void						Demo_DumpData			();
	void						Demo_Clear				();

	crashhandler*				m_pOldCrashHandler;
	
public:
	void						CallOldCrashHandler		();
	void						WriteStoredDemo			();

private: