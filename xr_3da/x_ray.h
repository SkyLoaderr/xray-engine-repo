#ifndef __X_RAY_H__
#define __X_RAY_H__

// refs
class	CFontSmall;

// definition
class ENGINE_API CApplication	:
	public pureFrame,
	public CEventBase
{
	// levels
	struct					sLevelInfo
	{
		char*				folder;
		char*				name;
	};
	
private:
	Shader*					ll_hLogo;
	CVertexStream*			ll_pStream;
	DWORD					ll_dwReference;
private:
	EVENT					eQuit;
	EVENT					eStartServer;
	EVENT					eStartClient;
	EVENT					eDisconnect;
public:
	CFontSmall*				pFont;

	// Levels
	vector<sLevelInfo>		Levels;
	DWORD					Level_Current;
	void					Level_Scan			();
	int						Level_ID			(LPCSTR name);
	void					Level_Set			(DWORD ID);

	// Loading
	void					LoadBegin			();
	void					LoadEnd				();
	void					LoadTitle			(char* S, char *S2=NULL);

	virtual	void			OnEvent				(EVENT E, DWORD P1, DWORD P2);

	// Other
							CApplication	( );
							~CApplication	( );

	virtual void			OnFrame			( );
};

extern ENGINE_API	CApplication*	pApp;

#endif //__XR_BASE_H__
