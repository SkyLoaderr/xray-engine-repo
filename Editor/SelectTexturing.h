//---------------------------------------------------------------------------


#ifndef SelectTexturingH
#define SelectTexturingH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include "CloseBtn.hpp"
#include "RXCtrls.hpp"
#include "multiobj.hpp"
#include "RxMenus.hpp"
#include <Menus.hpp>
//---------------------------------------------------------------------------
class TfraSelectTexturing : public TFrame
{
__published:	// IDE-managed Components
    TPanel *Panel4;
    TLabel *Label2;
    TExtBtn *SpeedButton19;
    TExtBtn *ebSelectObject;
    TExtBtn *ebSelectMesh;
    TExtBtn *ebSelectLayer;
    TLabel *lbObjectName;
    TLabel *lbMeshName;
    TLabel *lbLayerName;
    TRxPopupMenu *pmLayers;
    TMenuItem *miAddNewLayer;
    TMenuItem *miRemoveSelected;
    TMenuItem *N1;
    TMenuItem *miSelectLayer;
    TMenuItem *N2;
    TMenuItem *miProperties;
    TBevel *Bevel1;
    TExtBtn *ebPickTarget;
    TRxLabel *RxLabel2;
    TExtBtn *ebLayerAddFace;
    TExtBtn *ebLayerRemoveFace;
    TBevel *Bevel2;
    TRxLabel *RxLabel1;
    TRxLabel *RxLabel3;
    TExtBtn *ebGizmo;
    TRadioButton *RadioButton1;
    TRadioButton *RadioButton2;
    TRadioButton *RadioButton3;
    TMultiObjSpinEdit *MultiObjSpinEdit1;
    TMultiObjSpinEdit *MultiObjSpinEdit2;
    TExtBtn *ebFit;
    TExtBtn *ebCenter;
    TExtBtn *ebNormalAlign;
    TBevel *Bevel3;
    void __fastcall PanelMinClick(TObject *Sender);
    void __fastcall TopClick(TObject *Sender);
    void __fastcall ebSelectMeshClick(TObject *Sender);
    void __fastcall ebSelectObjectClick(TObject *Sender);
    void __fastcall ebSelectLayerClick(TObject *Sender);
    void __fastcall SceneUpdateClick(TObject *Sender);
    void __fastcall miSelectLayerClick(TObject *Sender);
    void __fastcall miRemoveSelectedClick(TObject *Sender);
    void __fastcall miAddNewLayerClick(TObject *Sender);
    void __fastcall miPropertiesClick(TObject *Sender);
    void __fastcall ebFitClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TfraSelectTexturing(TComponent* Owner);
    void UpdateSelection();
    void SelectLayer(int face_id);
};
//---------------------------------------------------------------------------
extern PACKAGE TfraSelectTexturing *fraSelectTexturing;
//---------------------------------------------------------------------------
#endif
