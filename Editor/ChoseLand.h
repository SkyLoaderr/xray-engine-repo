//---------------------------------------------------------------------------

#ifndef ChoseLandH
#define ChoseLandH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <ImgList.hpp>
//---------------------------------------------------------------------------
class TfrmChoseLand : public TForm
{
__published:	// IDE-managed Components
    TTreeView *tvLands;
    TPanel *Panel1;
    TPanel *Panel2;
    TSpeedButton *sbSelect;
    TSpeedButton *sbCancel;
    TImageList *ImageList1;
    void __fastcall sbSelectClick(TObject *Sender);
    void __fastcall sbCancelClick(TObject *Sender);
private:	// User declarations
    TTreeNode* pCurNode;
public:		// User declarations
    __fastcall TfrmChoseLand(TComponent* Owner);
    __fastcall void AddFolder(char* name);
    __fastcall void AddSubItem(char* name, void* obj);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmChoseLand *frmChoseLand;
//---------------------------------------------------------------------------
#endif
