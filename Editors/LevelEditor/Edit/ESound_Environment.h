//---------------------------------------------------------------------------
#ifndef ESound_EnvironmentH
#define ESound_EnvironmentH
//---------------------------------------------------------------------------

#include "ESound_Custom.h"
#include "EShape.h"

class ESoundEnvironment: public ISoundCustom{
	// Env
	CEditShape*		m_Shape;
	AnsiString		m_EnvRef;
public:
    virtual Fvector& GetPosition	()						{ return m_Shape->PPosition; 	}
    virtual void 	SetPosition		(const Fvector& pos)	{ m_Shape->PPosition = pos;		}
    virtual Fvector& GetRotation	()						{ return m_Shape->PRotation;	}
	virtual void 	SetRotation		(const Fvector& rot)	{ m_Shape->PRotation = rot;		}
public:
					ESoundEnvironment();
					~ESoundEnvironment();
	virtual void 	Render			(int priority, bool strictB2F, bool bLocked, bool bSelected);
	virtual bool 	RayPick			(float& dist, const Fvector& S, const Fvector& D, SRayPickInfo* pinf = NULL);
    virtual bool 	FrustumPick		(const CFrustum& frustum);
    virtual	void	OnFrame			();
  	virtual bool 	Load			(IReader&);
	virtual void 	Save			(IWriter&);
	virtual bool 	GetBox			(Fbox& box);
	virtual void 	Scale			(Fvector& amount);
	virtual void	FillProp		(LPCSTR pref, PropItemVec& values);
	virtual bool 	GetSummaryInfo	(SSceneSummary* inf);
	virtual void 	InitDefault		();
    virtual bool	ExportGame		(SExportStreams& data);
};

#endif
