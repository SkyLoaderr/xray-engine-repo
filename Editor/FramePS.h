//---------------------------------------------------------------------------
#ifndef FramePSH
#define FramePSH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include "CloseBtn.hpp"
#include "RXCtrls.hpp"

#include "SceneObject.h"
#include "ui_customtools.h"
// refs
class CEditObject;
//---------------------------------------------------------------------------
class TfraPS : public TFrame
{
__published:	// IDE-managed Components
    TPanel *paSelectObject;
    TLabel *Label5;
	TExtBtn *ebSelectByRefs;
	TExtBtn *ebDeselectByRefs;
    TPanel *paAddObject;
    TExtBtn *ebSelect;
	TLabel *label2;
	TExtBtn *ExtBtn1;
	TLabel *Label;
	TExtBtn *ExtBtn3;
	TLabel *lbSelectObjectName;
    void __fastcall PaneMinClick(TObject *Sender);
    void __fastcall ebSelectByRefsClick(TObject *Sender);
    void __fastcall ebDeselectByRefsClick(TObject *Sender);
    void __fastcall ExpandClick(TObject *Sender);
    void __fastcall ebSelectClick(TObject *Sender);
private:	// User declarations
    void __fastcall SelByRef (bool flag);
    void __fastcall OutCurrentName();
public:		// User declarations
        __fastcall TfraPS(TComponent* Owner);
};
//---------------------------------------------------------------------------
#endif
