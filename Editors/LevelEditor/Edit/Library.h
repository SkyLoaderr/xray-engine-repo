//----------------------------------------------------
// file: Library.h
//----------------------------------------------------

#ifndef LibraryH
#define LibraryH

#include "pure.h"
#include "FolderLib.h"              
//----------------------------------------------------
class CEditableObject;

DEFINE_MAP_PRED(AnsiString,CEditableObject*,EditObjMap,EditObjPairIt,astr_pred);
//----------------------------------------------------
class ELibrary:	public pureDeviceCreate, public pureDeviceDestroy
{
	bool				m_bReady;
	friend class TfrmChoseObject;
	EditObjMap			m_EditObjects;

    CEditableObject*	LoadEditObject		(LPCSTR full_name);
    void				UnloadEditObject	(LPCSTR full_name);
public:
						ELibrary			();
	virtual 			~ELibrary			();

    BOOL __fastcall		RemoveObject		(LPCSTR fname, EItemType type);
    void __fastcall		RenameObject		(LPCSTR fn0, LPCSTR fn1, EItemType type);

	void 				OnCreate			();
	void 				OnDestroy			();
	void 				Save				();

    void 				ReloadObjects		();
    void 				RefreshLibrary		();
    void 				ReloadObject		(LPCSTR name);

    CEditableObject*	CreateEditObject	(LPCSTR name);
    void				RemoveEditObject	(CEditableObject*& object);

    int					GetObjects			(FS_QueryMap& files);

    void				EvictObjects		();

	virtual		void	OnDeviceCreate		();
	virtual		void	OnDeviceDestroy		();
};

extern ELibrary Lib;
//----------------------------------------------------
#endif /*_INCDEF_Library_H_*/

