//---------------------------------------------------------------------------


#ifndef TopBarH
#define TopBarH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include "CloseBtn.hpp"
#include "Placemnt.hpp"
//---------------------------------------------------------------------------
class TfraTopBar : public TFrame
{
__published:	// IDE-managed Components
    TPanel *paTBEdit;
    TExtBtn *ebEditUndo;
    TExtBtn *ebEditRedo;
    TPanel *paTBAction;
    TExtBtn *ebActionMove;
    TExtBtn *ebActionRotate;
    TExtBtn *ebActionScale;
    TExtBtn *ebActionSelect;
    TExtBtn *ebActionAdd;
    TPanel *paSnap;
    TExtBtn *ebMSnap;
    TExtBtn *ebASnap;
    TExtBtn *ebCSLocal;
    TPanel *paAxis;
    TExtBtn *ebAxisX;
    TExtBtn *ebAxisY;
    TExtBtn *ebAxisZ;
    TExtBtn *ebAxisZX;
    TExtBtn *ebGSnap;
    TExtBtn *ebOSnap;
    TPanel *paView;
    TExtBtn *ebViewFront;
    TExtBtn *ebViewLeft;
    TExtBtn *ebViewTop;
    TExtBtn *ebViewBack;
    TExtBtn *ebViewRight;
    TExtBtn *ebViewBottom;
	TExtBtn *ebNonUniformScale;
	TPanel *Panel1;
	TExtBtn *ebZoomExtents;
	TExtBtn *ebZoomExtentsSelected;
	TExtBtn *ebVSnap;
	TFormStorage *fsStorage;
    void __fastcall ebEditUndoClick(TObject *Sender);
    void __fastcall ebEditRedoClick(TObject *Sender);
    void __fastcall ActionClick(TObject *Sender);
    void __fastcall ebViewClick(TObject *Sender);
	void __fastcall ebZoomExtentsClick(TObject *Sender);
	void __fastcall ebZoomExtentsSelectedClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TfraTopBar(TComponent* Owner);
    void OnTimer();
    void __fastcall ChangeAction(int act);
    void __fastcall ChangeAxis(int ax);
};
//---------------------------------------------------------------------------
extern PACKAGE TfraTopBar *fraTopBar;
//---------------------------------------------------------------------------
#endif
