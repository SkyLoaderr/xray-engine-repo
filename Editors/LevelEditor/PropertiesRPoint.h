//---------------------------------------------------------------------------
#ifndef PropertiesRPointH
#define PropertiesRPointH
#include <Classes.hpp>
#include <Controls.hpp>
#include <ExtCtrls.hpp>
#include "multi_check.hpp"
#include <StdCtrls.hpp>
#include "multi_edit.hpp"
//---------------------------------------------------------------------------
#include <Forms.hpp>
#include "ExtBtn.hpp"
#include "MXCtrls.hpp"

#include "PropertiesList.h"
#include "SpawnPoint.h"
#include "mxPlacemnt.hpp"

// refs
class CCustomObject;

class TfrmPropertiesSpawnPoint : public TForm
{
__published:	// IDE-managed Components
    TExtBtn *ebOk;
    TExtBtn *ebCancel;
    TPanel *paBottom;
	TPanel *paProps;
	TFormStorage *fsStorage;
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall ebCancelClick(TObject *Sender);
    void __fastcall ebOkClick(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall fsStorageRestorePlacement(TObject *Sender);
	void __fastcall fsStorageSavePlacement(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
private:	// User declarations
	static TfrmPropertiesSpawnPoint* form;
    TProperties* 	m_Props;
    CFS_Memory 		m_SPData;
    CSpawnPoint* 	m_SPObject;
//	PropItemVec		m_Items;

    void GetObjectInfo	();
    bool ApplyObjectInfo();
    void __fastcall OnModified();
    void __fastcall UpdateProps();
public:		// User declarations
    __fastcall TfrmPropertiesSpawnPoint(TComponent* Owner);
    static int __fastcall Run(list<CCustomObject*>* pObjects, bool& bChange);
    static bool __fastcall Visible(){return !!form;}
    static TfrmPropertiesSpawnPoint* GetForm(){VERIFY(form); return form;}
};
//---------------------------------------------------------------------------
#endif
