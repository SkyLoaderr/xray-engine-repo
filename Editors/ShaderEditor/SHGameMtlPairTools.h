//---------------------------------------------------------------------------
#ifndef SHGameMtlPairToolsH
#define SHGameMtlPairToolsH

#include "SHToolsInterface.h"
#include "GameMtlLib.h"

// refs
class PropValue;

class CSHGameMtlPairTools: public ISHTools
{
	ISHTools*				m_GameMtlTools;
    virtual LPCSTR			GenerateItemName	(LPSTR name, LPCSTR pref, LPCSTR source);
public:
    SGameMtlPair*			m_MtlPair;
    virtual LPCSTR			AppendItem			(LPCSTR folder_name, LPCSTR parent_name=0);
    virtual void			RemoveItem			(LPCSTR name);
	virtual void			RenameItem			(LPCSTR old_full_name, LPCSTR ren_part, int level){;}
	virtual void			RenameItem			(LPCSTR old_full_name, LPCSTR new_full_name){;}
	virtual void 			FillItemList		();
public:
							CSHGameMtlPairTools (ISHInit& init);
    virtual 				~CSHGameMtlPairTools();

    virtual LPCSTR			ToolsName			(){return "Game Material Pairs";}

	virtual void			Reload				();
	virtual void			Load				();
	virtual void			Save				();
    virtual void			ApplyChanges		(bool bForced=false);

    virtual bool			OnCreate			();
    virtual void			OnDestroy			();
	virtual void 			OnActivate			(){;}
	virtual void 			OnDeactivate		(){;}

    // misc
    virtual void			ResetCurrentItem	();
    virtual void			SetCurrentItem		(LPCSTR name);

	virtual void 			UpdateProperties	();

	virtual void 			OnFrame				();
    virtual void			OnRender			(){;}

    virtual void			OnDeviceCreate		(){;}
    virtual void			OnDeviceDestroy		(){;}
};
//---------------------------------------------------------------------------
#endif // SHGameMaterialToolsH
