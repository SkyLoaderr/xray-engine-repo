//---------------------------------------------------------------------------
#ifndef ESound_EnvironmentH
#define ESound_EnvironmentH
//---------------------------------------------------------------------------

#include "CustomObject.h"
#include "EShape.h"
#include "xrServer_Objects_Alife_All.h"

class ESoundEnvironment: public CEditShape
{
	typedef CCustomObject inherited;

	friend class		CLevelSoundManager;
	// Env
	AnsiString			m_EnvInner;
	AnsiString			m_EnvOuter;

    void __fastcall		OnChangeEnvs	(PropValue* prop);
public:
    void				get_box			(Fmatrix& m);
public:
						ESoundEnvironment(LPVOID data, LPCSTR name);
	void 				Construct		(LPVOID data);
						~ESoundEnvironment();
    virtual bool		CanAttach		() {return true;}
                        
  	virtual bool 		Load			(IReader&);
	virtual void 		Save			(IWriter&);
	virtual void		FillProp		(LPCSTR pref, PropItemVec& values);
	virtual bool 		GetSummaryInfo	(SSceneSummary* inf);
	virtual void 		OnSceneUpdate	();
};

#endif
