//----------------------------------------------------
// file: Library.h
//----------------------------------------------------

#ifndef LibraryH
#define LibraryH

#include "pure.h"
//----------------------------------------------------
class CEditableObject;

DEFINE_MAP_PRED(AnsiString,CEditableObject*,EditObjMap,EditObjPairIt,astr_pred);
//----------------------------------------------------
class ELibrary:	public pureDeviceCreate, public pureDeviceDestroy
{
	bool				m_bReady;
	friend class TfrmChoseObject;
	EditObjMap			m_EditObjects;
    AnsiString			m_Current;

    CEditableObject*	LoadEditObject		(LPCSTR name, int age);
public:
						ELibrary			();
	virtual 			~ELibrary			();

	void 				OnCreate			();
	void 				OnDestroy			();
	void 				Save				();

    void 				ReloadObjects		();
    void 				RefreshLibrary		();
    void 				ReloadObject		(LPCSTR name);

    void				SetCurrentObject	(LPCSTR T);
    LPCSTR				GetCurrentObject	(){return m_Current.IsEmpty()?0:m_Current.c_str();}
    CEditableObject*	CreateEditObject	(LPCSTR name,int* age=0);
    void				RemoveEditObject	(CEditableObject*& object);

    int					GetObjects			(FS_QueryMap& files);

    void				EvictObjects		();

	virtual		void	OnDeviceCreate		();
	virtual		void	OnDeviceDestroy		();
};

extern ELibrary Lib;
//----------------------------------------------------
#endif /*_INCDEF_Library_H_*/

