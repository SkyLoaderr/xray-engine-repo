//---------------------------------------------------------------------------

#ifndef PropertiesGlowH
#define PropertiesGlowH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Dialogs.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
#include "ExtBtn.hpp"
#include "mxPlacemnt.hpp"
#include "ElTree.hpp"

#include "ExtBtn.hpp"
#include "multi_edit.hpp"
#include "MXCtrls.hpp"
#include "mxPlacemnt.hpp"

class CGlow;
class TProperties;
class EImageThumbnail;
//---------------------------------------------------------------------------
class TfrmPropertiesGlow : public TForm
{
__published:	// IDE-managed Components
    TPanel *Panel1;
    TPanel *paImage;
    TPaintBox *pbImage;
	TFormStorage *fsStorage;
	TPanel *Panel2;
	TExtBtn *ebOk;
	TExtBtn *ebCancel;
	TPanel *paProps;
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall ebCancelClick(TObject *Sender);
    void __fastcall ebOkClick(TObject *Sender);
    void __fastcall pbImagePaint(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall fsStorageRestorePlacement(TObject *Sender);
	void __fastcall fsStorageSavePlacement(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
private:	// User declarations
    ObjectList* 	m_Objects;
    void 			GetObjectsInfo();
    void			ApplyObjectsInfo();
    void 			CancelObjectsInfo();
    EImageThumbnail*m_Thumbnail;
    TProperties* 	m_Props;
    void __fastcall OnModified();
	void __fastcall	OnShaderChange(PropValue* prop);
    void __fastcall OnItemFocused(TElTreeItem* item);
public:		// User declarations
    __fastcall TfrmPropertiesGlow(TComponent* Owner);
    void __fastcall Run(ObjectList* pObjects, bool& bChange);
};
//---------------------------------------------------------------------------
extern void frmPropertiesGlowRun(ObjectList* pObjects, bool& bChange);
//---------------------------------------------------------------------------
#endif
