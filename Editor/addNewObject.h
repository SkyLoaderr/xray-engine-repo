//---------------------------------------------------------------------------

#ifndef addNewObjectH
#define addNewObjectH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "CloseBtn.hpp"
#include "RXCtrls.hpp"
#include <Dialogs.hpp>
//---------------------------------------------------------------------------
class TfrmAddNewObject : public TForm
{
__published:	// IDE-managed Components
    TExtBtn *sbNewObject;
    TListBox *lbMeshes;
    TOpenDialog *odMesh;
    TExtBtn *ebAdd;
    TExtBtn *ExtBtn2;
    TEdit *edName;
    TRxLabel *RxLabel1;
    TExtBtn *ExtBtn1;
    void __fastcall ebAddClick(TObject *Sender);
    void __fastcall sbNewObjectClick(TObject *Sender);
    void __fastcall edNameChange(TObject *Sender);
    void __fastcall ExtBtn1Click(TObject *Sender);
    void __fastcall ExtBtn2Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
    __fastcall TfrmAddNewObject(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmAddNewObject *frmAddNewObject;
//---------------------------------------------------------------------------
#endif
