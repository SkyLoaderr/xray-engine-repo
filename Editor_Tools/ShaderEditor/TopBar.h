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
#include "ExtBtn.hpp"
#include "mxPlacemnt.hpp"
//---------------------------------------------------------------------------
class TfraTopBar : public TFrame
{
__published:	// IDE-managed Components
    TPanel *paView;
    TExtBtn *ebViewFront;
    TExtBtn *ebViewLeft;
    TExtBtn *ebViewTop;
    TExtBtn *ebViewBack;
    TExtBtn *ebViewRight;
    TExtBtn *ebViewBottom;
	TPanel *Panel1;
	TExtBtn *ebZoomExtents;
	TFormStorage *fsStorage;
	TExtBtn *ebCameraPlane;
	TExtBtn *ebCameraArcBall;
	TExtBtn *ebCameraFly;
	TPanel *paTBAction;
	TExtBtn *ebActionMove;
	TExtBtn *ebActionRotate;
	TExtBtn *ebActionScale;
	TExtBtn *ebActionSelect;
	TExtBtn *ebActionAdd;
    void __fastcall ebViewClick(TObject *Sender);
	void __fastcall ebZoomExtentsClick(TObject *Sender);
	void __fastcall ebCameraClick(TObject *Sender);
	void __fastcall fsStorageRestorePlacement(TObject *Sender);
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
