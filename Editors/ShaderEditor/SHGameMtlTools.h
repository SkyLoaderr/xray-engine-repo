//---------------------------------------------------------------------------
#ifndef SHGameMaterialToolsH
#define SHGameMaterialToolsH

#include "ElTree.hpp"
#include "GameMtlLib.h"

// refs
class PropValue;

class CSHGameMaterialTools: public ISHTools
{
	SGameMtl*			FindMaterial		(LPCSTR name);
    LPCSTR				GenerateMaterialName(LPSTR name, LPCSTR source);

    bool 				m_bUpdateCurrent;	// если менялся объект непосредственно  Update____From___()
    void				FillMtlsView		();
    void				FillMtlPairsView	();
public:
    SGameMtl*			m_Mtl;
    SGameMtl*			AppendMaterial		(LPCSTR folder_name, SGameMtl* parent);
    SGameMtl*			CloneMaterial		(LPCSTR name);
    void				RemoveMaterial		(LPCSTR name);
	void				RenameMaterial		(LPCSTR old_full_name, LPCSTR ren_part, int level);
	void				RenameMaterial		(LPCSTR old_full_name, LPCSTR new_full_name);
	void __fastcall 	OnMaterialNameChange(PropValue* sender);
public:
    SGameMtlPair*		m_MtlPair;
    SGameMtlPair*		AppendMaterialPair	(int m0, int m1, SGameMtlPair* parent);
    void				RemoveMaterialPair	(LPCSTR name);
public:
						CSHGameMaterialTools 	();
    virtual 			~CSHGameMaterialTools	();

	void				Reload				();
	void				Load				();
	void				Save				();
    void				ApplyChanges		();

    bool				IfModified			();
    bool				IsModified			(){return m_bModified;}
    void _fastcall		Modified			();

    void				OnCreate			();
    void				OnDestroy			();

    // misc
    void				ResetCurrentMaterial();
    void				SetCurrentMaterial	(SGameMtl* B);
    void				SetCurrentMaterial	(LPCSTR name);
    void				SetCurrentMaterialPair	(SGameMtlPair* B);
    void				SetCurrentMaterialPair	(LPCSTR name);

	void 				UpdateProperties	();
	void 				OnFrame				();
};
//---------------------------------------------------------------------------
#endif // SHGameMaterialToolsH
