// Sound.h: interface for the ESound class.
//
//////////////////////////////////////////////////////////////////////

#ifndef ESoundH
#define ESoundH
#pragma once

class CFrustum;

class ESound : public CCustomObject
{
	typedef CCustomObject inherited;
	friend class TfrmPropertiesSound;
	friend class SceneBuilder;
protected:
	sound			m_Source;
	AnsiString		m_WAVName;

    CSound_params	m_Params;
    void __fastcall	OnChangeWAV		(PropValue* prop);
    void __fastcall	OnChangeSource	(PropValue* prop);

    void			ResetSource		();
protected:
    virtual Fvector& GetPosition	()						{ return m_Params.position; 	}
    virtual void 	SetPosition		(const Fvector& pos)	{ m_Params.position.set(pos);	UpdateTransform();}
public:
					ESound			(LPVOID data, LPCSTR name);
	void 			Construct		(LPVOID data);
	virtual 		~ESound			();

    LPCSTR			GetSourceWAV	(){return m_WAVName.c_str();}
    void			SetSourceWAV	(LPCSTR fname);

    void			Play			(BOOL bLoop=FALSE);
    void			Stop			();
    
	virtual void 	Render			(int priority, bool strictB2F);

	virtual bool 	RayPick			(float& dist, const Fvector& S, const Fvector& D, SRayPickInfo* pinf = NULL);

    virtual bool 	FrustumPick		(const CFrustum& frustum);

    virtual	void	OnFrame			();
    
  	virtual bool 	Load			(IReader&);
	virtual void 	Save			(IWriter&);

	virtual bool 	GetBox			(Fbox& box);

    // change position/orientation methods
	virtual void 	Scale			(Fvector& amount){;}

	// editor integration
	virtual void	FillProp		(LPCSTR pref, PropItemVec& values);
	virtual bool 	GetSummaryInfo	(SSceneSummary* inf);
};

#endif // SoundH
