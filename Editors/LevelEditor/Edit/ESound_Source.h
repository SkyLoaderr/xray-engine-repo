//---------------------------------------------------------------------------
#ifndef ESound_SourceH
#define ESound_SourceH
//---------------------------------------------------------------------------

class ESoundSource: public CCustomObject
{
	typedef CCustomObject inherited;
	// static sound
	sound			m_Source;
	AnsiString		m_WAVName;
    CSound_params	m_Params;
    // edit events
    void __fastcall	OnChangeWAV		(PropValue* prop);
    void __fastcall	OnChangeSource	(PropValue* prop);
	void __fastcall OnControlClick	(PropValue* sender, bool& bModif);
    void			ResetSource		();
    enum{
    	flLooped		= (1<<0),
    	flPlaying		= (1<<1)
    };
    Flags32			m_Flags;
    enum ECommand{
    	stNothing,
    	stPlay,
        stStop,
        stPause
    };
    ECommand		m_Command;
public:
	enum ESoundType{
    	stStaticSource = 0,
    };
    ESoundType		m_Type;
public:
    virtual Fvector& GetPosition	()						{ return m_Params.position; 	}
    virtual void 	SetPosition		(const Fvector& pos)	{ m_Params.position.set(pos); m_Source.set_position(m_Params.position);	}
public:
					ESoundSource	(LPVOID data, LPCSTR name);
	void 			Construct		(LPVOID data);
					~ESoundSource	();

    LPCSTR			GetSourceWAV	(){return m_WAVName.c_str();}
    void			SetSourceWAV	(LPCSTR fname);
    void			Play			(){m_Command=stPlay;}
    void			Stop			(){m_Command=stStop;}

	virtual void 	Render			(int priority, bool strictB2F);
	virtual bool 	RayPick			(float& dist, const Fvector& S, const Fvector& D, SRayPickInfo* pinf = NULL);
    virtual bool 	FrustumPick		(const CFrustum& frustum);
    virtual	void	OnFrame			();
  	virtual bool 	Load			(IReader&);
	virtual void 	Save			(IWriter&);
	virtual bool 	GetBox			(Fbox& box);
	virtual void 	Scale			(Fvector& amount){;}
	virtual void	FillProp		(LPCSTR pref, PropItemVec& values);
	virtual bool 	GetSummaryInfo	(SSceneSummary* inf);
    virtual bool	ExportGame		(SExportStreams& data);
};

#endif
