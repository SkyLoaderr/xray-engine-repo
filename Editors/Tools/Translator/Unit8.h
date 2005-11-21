//---------------------------------------------------------------------------

#ifndef Unit8H
#define Unit8H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Grids.hpp>
#include <vector>
using namespace std;
//---------------------------------------------------------------------------
class TRemovedStringsDialog : public TForm
{
__published:	// IDE-managed Components
    TStringGrid *StringsList;
    TButton *Button1;
    TLabel *Label1;
private:	// User declarations
public:		// User declarations
    __fastcall TRemovedStringsDialog(TComponent* Owner);
    void        Add     (string file, string strid);
    void        Clear   ();
};
//---------------------------------------------------------------------------
extern PACKAGE TRemovedStringsDialog *RemovedStringsDialog;
//---------------------------------------------------------------------------
#endif
