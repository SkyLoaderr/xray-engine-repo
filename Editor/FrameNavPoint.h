//---------------------------------------------------------------------------
#ifndef FrameNavPointH
#define FrameNavPointH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include "CloseBtn.hpp"

#include "multiobj.hpp"
#include "RXCtrls.hpp"
#include "APHeadLabel.hpp"

#include "SceneObject.h"
#include "ui_customtools.h"
//---------------------------------------------------------------------------
class TfraNavPoint : public TFrame
{
__published:	// IDE-managed Components
    void __fastcall PanelMinClick(TObject *Sender);
    void __fastcall Label4Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TfraNavPoint(TComponent* Owner);
    void RunEditor (ObjectList& objset);
    void Activate(){;}
    void Deactivate(){;}
};
//---------------------------------------------------------------------------
extern PACKAGE TfraNavPoint *fraNavPoint;
//---------------------------------------------------------------------------
#endif
