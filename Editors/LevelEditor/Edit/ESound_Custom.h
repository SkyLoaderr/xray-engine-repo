//---------------------------------------------------------------------------
#ifndef ESound_CustomH
#define ESound_CustomH
//---------------------------------------------------------------------------

class ISoundCustom{
public:
    virtual Fvector& GetPosition	()=0;
    virtual void 	SetPosition		(const Fvector& pos)=0;
    virtual Fvector& GetRotation	()=0;
	virtual void 	SetRotation		(const Fvector& rot)=0;
public:
	virtual void 	Render			(int priority, bool strictB2F, bool bLocked, bool bSelected)=0;
	virtual bool 	RayPick			(float& dist, const Fvector& S, const Fvector& D, SRayPickInfo* pinf = NULL)=0;
    virtual bool 	FrustumPick		(const CFrustum& frustum)=0;
    virtual	void	OnFrame			()=0;
  	virtual bool 	Load			(IReader&)=0;
	virtual void 	Save			(IWriter&)=0;
	virtual bool 	GetBox			(Fbox& box)=0;
	virtual void 	Scale			(Fvector& amount)=0;
	virtual void	FillProp		(LPCSTR pref, PropItemVec& values)=0;
	virtual bool 	GetSummaryInfo	(SSceneSummary* inf)=0;
	virtual void 	InitDefault		()=0;
    virtual bool	ExportGame		(SExportStreams& data)=0;
};

#endif
