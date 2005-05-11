#ifndef __X_RAY_H__
#define __X_RAY_H__

// refs
class ENGINE_API CGameFont;

// definition
class ENGINE_API CApplication	:
	public pureFrame,
	public IEventReceiver
{
	// levels
	struct					sLevelInfo
	{
		char*				folder;
		char*				name;
	};
	
private:
	ref_shader				ll_hLogo;
	ref_shader				ll_hLogo1;
	ref_shader				ll_hLogo2;
	ref_geom				ll_hGeom;
	u32						ll_dwReference;
private:
	EVENT					eQuit;
	EVENT					eStart;
	EVENT					eStartLoad;
	EVENT					eDisconnect;

	void					Level_Append		(LPCSTR lname);
public:
	CGameFont*				pFontSystem;

	// Levels
	xr_vector<sLevelInfo>	Levels;
	u32						Level_Current;
	void					Level_Scan			();
	int						Level_ID			(LPCSTR name);
	void					Level_Set			(u32 ID);

	// Loading
	void					LoadBegin			();
	void					LoadEnd				();
	void					LoadTitle			(char* S, char *S2=NULL);
	void					LoadSwitch			();

	virtual	void			OnEvent				(EVENT E, u64 P1, u64 P2);

	// Other
							CApplication	( );
							~CApplication	( );

	virtual void			OnFrame			( );
};

extern ENGINE_API	CApplication*	pApp;

#endif //__XR_BASE_H__
