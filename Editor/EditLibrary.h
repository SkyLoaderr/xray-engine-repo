//---------------------------------------------------------------------------

#ifndef EditLibraryH
#define EditLibraryH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <ImgList.hpp>

#include "SceneObject.h"
#include "Library.h"
#include <Dialogs.hpp>
#include "ElTree.hpp"
#include "CloseBtn.hpp"
#include "ElHeader.hpp"
#include "RXCtrls.hpp"
#include <Grids.hpp>
//---------------------------------------------------------------------------
// refs
class TObjectPreview;
class SObject2;
//---------------------------------------------------------------------------

class TfrmEditLibrary : public TForm
{
__published:	// IDE-managed Components
    TPanel *Panel1;
    TExtBtn *sbCancel;
    TExtBtn *sbNewObject;
    TExtBtn *sbDeleteObject;
    TElTree *tvObjects;
    TExtBtn *sbEdit;
    TExtBtn *sbSave;
    TPageControl *pcObjects;
    TTabSheet *tsImage;
    TPanel *paPreview;
    void __fastcall sbSaveClick(TObject *Sender);
    void __fastcall sbCancelClick(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall tvObjectsItemSelectedChange(TObject *Sender,
          TElTreeItem *Item);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall sbEditClick(TObject *Sender);
    void __fastcall sbNewObjectClick(TObject *Sender);
    void __fastcall sbDeleteObjectClick(TObject *Sender);
    void __fastcall tvObjectsDblClick(TObject *Sender);
private:	// User declarations
//    TObjectPreview* preview;
//    void InitObjectFolder( ObjFolder& folder, TElTreeItem* pParentNode, SceneObject *_Current );
    void InitObjectFolder( SObject2* obj );
    TElTreeItem* FindFolderByType(DWORD type);
    TElTreeItem* FindObjectByType(DWORD type, void *obj);
    TElTreeItem* AddFolder(DWORD type);
    TElTreeItem* AddObject(TElTreeItem* node, LPSTR name, void* obj);
    SObject2* cur_obj;
public:		// User declarations
    __fastcall TfrmEditLibrary(TComponent* Owner);
    void __fastcall EditLibrary();
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmEditLibrary *frmEditLibrary;
//---------------------------------------------------------------------------
#endif
