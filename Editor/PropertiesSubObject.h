//---------------------------------------------------------------------------

#ifndef PropertiesSubObjectH
#define PropertiesSubObjectH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Dialogs.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
#include "CloseBtn.hpp"
#include "ElHeader.hpp"
#include "ElTree.hpp"
#include "multiobj.hpp"
#include "RXCtrls.hpp"
#include <Grids.hpp>
#include <ExtDlgs.hpp>
#include "ToolEdit.hpp"
#include <Mask.hpp>
#include "Placemnt.hpp"
#include "DualList.hpp"

class Mesh;
class CTextureLayer;
//---------------------------------------------------------------------------
class TfrmPropertiesSubObject : public TForm
{
__published:	// IDE-managed Components
    TExtBtn *ebOk;
    TExtBtn *ebCancel;
    TPageControl *pcMesh;
    TTabSheet *tsLayers;
    TTabSheet *tsInfo;
    TRxLabel *RxLabel1;
    TRxLabel *RxLabel3;
    TRxLabel *lbVertices;
    TRxLabel *lbFaces;
    TPanel *paBottom;
    TRxLabel *RxLabel4;
    TRxLabel *lbTLayers;
    TPanel *Panel1;
    TElTree *tvTLayers;
    TExtBtn *ebAddLayer;
    TExtBtn *ebDeleteLayer;
    TRxLabel *RxLabel2;
    TBevel *Bevel2;
    TEdit *edName;
    TRxLabel *RxLabel5;
    TBevel *Bevel1;
    TEdit *edScript;
    TRxLabel *RxLabel7;
    TBevel *Bevel5;
    TEdit *edTextureName;
    TRxLabel *RxLabel8;
    TPanel *paImage;
    TPaintBox *pbImage;
    TExtBtn *ebTexture;
    TRadioGroup *rbShader;
    TComboBox *cbClassID;
    TFormStorage *fsMeshProp;
    TExtBtn *ebDestroyTex;
    TDualListDialog *dldFaces;
    TExtBtn *ebFaceList;
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall tsInfoShow(TObject *Sender);
    void __fastcall tsLayersShow(TObject *Sender);
    void __fastcall ebCancelClick(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall ebOkClick(TObject *Sender);
    void __fastcall tvTLayersDragOver(TObject *Sender, TObject *Source,
          int X, int Y, TDragState State, bool &Accept);
    void __fastcall tvTLayersStartDrag(TObject *Sender,
          TDragObject *&DragObject);
    void __fastcall tvTLayersDragDrop(TObject *Sender, TObject *Source,
          int X, int Y);
    void __fastcall ebDeleteLayerClick(TObject *Sender);
    void __fastcall ebAddLayerClick(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall tvTLayersItemSelectedChange(TObject *Sender,
          TElTreeItem *Item);
    void __fastcall edNameChange(TObject *Sender);
    void __fastcall edScriptChange(TObject *Sender);
    void __fastcall ebTextureClick(TObject *Sender);
    void __fastcall pbImagePaint(TObject *Sender);
    void __fastcall rbShaderClick(TObject *Sender);
    void __fastcall cbClassIDChange(TObject *Sender);
    void __fastcall ebDestroyTexClick(TObject *Sender);
    void __fastcall ebFaceListClick(TObject *Sender);
    void __fastcall tvTLayersDblClick(TObject *Sender);
private:	// User declarations
    Mesh* m_SaveMesh;
    Mesh* m_EditMesh;
    TElTreeItem* FDragItem;
    CTextureLayer* FSelectedLayer;
    void __fastcall OnModified();
    void __fastcall Update();
    int __fastcall FindInCBClass(unsigned __int64 cls);
    void AddLayerToList(CTextureLayer* l);
    void RemoveLayerFromList(CTextureLayer* l);
    void DrawTexture();
public:		// User declarations
    __fastcall TfrmPropertiesSubObject(TComponent* Owner);
    int __fastcall Edit(Mesh* mesh);
    int __fastcall EditLayer(Mesh* mesh);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmPropertiesSubObject *frmPropertiesSubObject;
//---------------------------------------------------------------------------
#endif
