//---------------------------------------------------------------------------

#ifndef PropertiesObjectH
#define PropertiesObjectH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Dialogs.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
#include "CSPIN.h"
#include "RXCtrls.hpp"
#include "RXSpin.hpp"

#include "SceneObject.h"
#include "multiobj.hpp"
#include "CloseBtn.hpp"
#include "ElHeader.hpp"
#include <Grids.hpp>
#include "ElTree.hpp"

class SObject2;
class ETexture;
class Mesh;
//---------------------------------------------------------------------------
enum EPropObjMode{
    pomChange,
    pomEdit,
    pomAdd
};

class TfrmPropertiesObject : public TForm
{
__published:	// IDE-managed Components
    TPanel *Panel1;
    TExtBtn *ebOk;
    TExtBtn *ebCancel;
    TOpenDialog *odMesh;
    TPageControl *pcObjects;
    TTabSheet *tsScript;
    TMemo *mmScript;
    TTabSheet *tsInfo;
    TRxLabel *RxLabel1;
    TRxLabel *RxLabel3;
    TRxLabel *RxLabel4;
    TRxLabel *RxLabel5;
    TRxLabel *lbVertices;
    TRxLabel *lbFaces;
    TTabSheet *tsTextures;
    TTabSheet *tsMeshes;
    TElTree *tvTextures;
    TPanel *paMatrix;
    TStringGrid *sgMatrix;
    TElHeader *ElHeader2;
    TPanel *paBB;
    TStringGrid *sgBB;
    TElHeader *ElHeader1;
    TTabSheet *tsMainOptions;
    TEdit *edName;
    TEdit *edClass;
    TMultiObjCheck *cbMakeUnique;
    TMultiObjCheck *cbMakeDynamic;
    TRxLabel *RxLabel2;
    TRxLabel *RxLabel6;
    TBevel *Bevel2;
    TBevel *Bevel3;
    TRxLabel *RxLabel7;
    TRxLabel *RxLabel8;
    TRxLabel *RxLabel9;
    TRxLabel *lbWidth;
    TRxLabel *lbHeight;
    TRxLabel *lbAlpha;
    TRxLabel *RxLabel10;
    TRxLabel *RxLabel11;
    TExtBtn *ebDeleteMesh;
    TExtBtn *ebEditMesh;
    TExtBtn *ebAddMesh;
    TPanel *paImage;
    TPaintBox *im;
    TPanel *paBottom;
    TRxLabel *lbLibRef;
    TElTree *tvMeshes;
    TExtBtn *ebMergeMesh;
    void __fastcall ebEditMeshClick(TObject *Sender);
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall ebCancelClick(TObject *Sender);
    void __fastcall ebOkClick(TObject *Sender);
    void __fastcall ebAddMeshClick(TObject *Sender);
    void __fastcall ebDeleteMeshClick(TObject *Sender);
    void __fastcall cbMakeDynamicClick(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall tsInfoShow(TObject *Sender);
    void __fastcall tsTexturesShow(TObject *Sender);
    void __fastcall tsMeshesShow(TObject *Sender);
    void __fastcall tvTexturesItemSelectedChange(TObject *Sender,
          TElTreeItem *Item);
    void __fastcall imPaint(TObject *Sender);
    void __fastcall tsMainOptionsShow(TObject *Sender);
    void __fastcall OnModified(TObject *Sender);
    void __fastcall tvMeshesDblClick(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall ebMergeMeshClick(TObject *Sender);
    void __fastcall tvMeshesItemSelectedChange(TObject *Sender,
          TElTreeItem *Item);
private:	// User declarations
    EPropObjMode mode;

	bool bMultiSelection;
    list<SceneObject*> m_Objects;
	SObject2 *m_EditObject;
	SObject2 *m_SourceObject;
    ETexture* tx_selected;
    void GetObjectsInfo     ();
    bool ApplyObjectsInfo   ();
public:		// User declarations
    __fastcall TfrmPropertiesObject(TComponent* Owner);
    int __fastcall ChangeProperties(ObjectList* pObjects);
    int __fastcall AddNewObject(SObject2* obj);
    int __fastcall EditObject(SObject2* obj);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmPropertiesObject *frmPropertiesObject;
//---------------------------------------------------------------------------
#endif
