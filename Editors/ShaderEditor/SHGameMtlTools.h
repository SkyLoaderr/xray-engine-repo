//---------------------------------------------------------------------------
#ifndef SHGameMtlToolsH
#define SHGameMtlToolsH

#include "SHToolsInterface.h"
#include "GameMtlLib.h"

// refs
class PropValue;

class CSHGameMtlTools: public ISHTools
{
	ISHTools*				m_GameMtlPairTools;
	SGameMtl*				FindItem			(LPCSTR name);
    virtual LPCSTR			GenerateItemName	(LPSTR name, LPCSTR pref, LPCSTR source);
public:
    SGameMtl*				m_Mtl;
    virtual LPCSTR			AppendItem			(LPCSTR folder_name, LPCSTR parent_name);
    virtual void			RemoveItem			(LPCSTR name);
	virtual void			RenameItem			(LPCSTR old_full_name, LPCSTR ren_part, int level);
	virtual void			RenameItem			(LPCSTR old_full_name, LPCSTR new_full_name);
	void __fastcall 		OnMaterialNameChange(PropValue* sender);

    virtual void			FillItemList		();
public:
							CSHGameMtlTools 	(EToolsID id, TElTree* tv, TMxPopupMenu* mn, TElTabSheet* sheet, TProperties* props);
    virtual 				~CSHGameMtlTools	();

	virtual void			Reload				();
	virtual void			Load				();
	virtual void			Save				();
    virtual void			ApplyChanges		();

    virtual bool			IfModified			();
    virtual void __fastcall	Modified			();

    virtual bool			OnCreate			();
    virtual void			OnDestroy			();
	virtual void 			OnActivate			(){;}
	virtual void 			OnDeactivate		(){;}

    // misc
    virtual void			ResetCurrentItem	();
    virtual void			SetCurrentItem		(LPCSTR name);

	virtual void 			UpdateProperties	();

	virtual void 			OnFrame				();
	virtual void 			OnRender			(){;}

    virtual void			OnDeviceCreate		(){;}
    virtual void			OnDeviceDestroy		(){;}
};
//---------------------------------------------------------------------------
#endif // SHGameMtlToolsH
