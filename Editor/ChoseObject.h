//---------------------------------------------------------------------------

#ifndef ChoseObjectH
#define ChoseObjectH
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
//---------------------------------------------------------------------------
class TObjectPreview;
class SObject2;

class TfrmChoseObject : public TForm
{
__published:	// IDE-managed Components
    TPanel *Panel1;
    TExtBtn *sbSelect;
    TExtBtn *sbCancel;
    TElTree *tvObjects;
    TPanel *paPreview;
    TExtBtn *ebAddFolder;
    TExtBtn *ebDeleteFolder;
    void __fastcall sbSelectClick(TObject *Sender);
    void __fastcall sbCancelClick(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall tvObjectsDblClick(TObject *Sender);
private:	// User declarations
    void InitObjectFolder( );
    TElTreeItem* AddObject(LPSTR name, void* obj);
    SObject2* cur_obj;
public:		// User declarations
    __fastcall TfrmChoseObject(TComponent* Owner);
    bool __fastcall SelectObject(SceneObject** obj);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmChoseObject *frmChoseObject;
//---------------------------------------------------------------------------
#endif
