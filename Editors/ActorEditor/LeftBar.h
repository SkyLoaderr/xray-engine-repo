//---------------------------------------------------------------------------
#ifndef LeftBarH
#define LeftBarH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include "ExtBtn.hpp"
#include <Menus.hpp>
#include "ExtBtn.hpp"
#include "MxMenus.hpp"
#include "mxPlacemnt.hpp"
#include "ElTree.hpp"
#include "ElXPThemedControl.hpp"

#include "UI_Tools.h"
#include "ElTreeAdvEdit.hpp"
#include "ElPgCtl.hpp"
#include "MXCtrls.hpp"

//---------------------------------------------------------------------------
class TfraLeftBar : public TFrame
{
__published:	// IDE-managed Components
    TPanel *paLeftBar;
	TFormStorage *fsStorage;
	TMxPopupMenu *pmSceneFile;
	TMxPopupMenu *pmShaderList;
	TMenuItem *ExpandAll1;
	TMenuItem *CollapseAll1;
	TMenuItem *N1;
	TMenuItem *CreateFolder1;
	TMenuItem *Load1;
	TMenuItem *Save2;
	TMenuItem *miExportOGF;
	TMenuItem *SaevAs1;
	TMenuItem *N5;
	TMenuItem *Import1;
	TMenuItem *N2;
	TMenuItem *miRecentFiles;
	TMxPopupMenu *pmPreviewObject;
	TMenuItem *Custom1;
	TMenuItem *N3;
	TMenuItem *none1;
	TMenuItem *Preferences1;
	TMenuItem *Clear1;
	TMenuItem *N4;
	TMxPopupMenu *pmImages;
	TMenuItem *Refresh1;
	TMenuItem *Checknewtextures1;
	TMenuItem *ImageEditor1;
	TMenuItem *N6;
	TPanel *paBasic;
	TPanel *paModel;
	TLabel *Label4;
	TExtBtn *ExtBtn2;
	TExtBtn *ebRenderEditorStyle;
	TExtBtn *ebRenderEngineStyle;
	TLabel *Label5;
	TExtBtn *ebBonePart;
	TPanel *paObjectProperties;
	TLabel *Label6;
	TBevel *Bevel6;
	TPanel *paObjectProps;
	TPanel *paScene;
	TLabel *APHeadLabel2;
	TExtBtn *ebSceneMin;
	TExtBtn *ebSceneFile;
	TExtBtn *ebPreferences;
	TExtBtn *ebPreviewObjectClick;
	TExtBtn *ebSceneCommands1;
	TMenuItem *N7;
	TMenuItem *N8;
	TMenuItem *N9;
	TMenuItem *N11;
	TPanel *paCurrentMotion;
	TLabel *Label1;
	TExtBtn *ExtBtn10;
	TPanel *paPSList;
	TBevel *Bevel1;
	TPanel *paItemProps;
	TSplitter *Splitter1;
    void __fastcall ebSaveClick(TObject *Sender);
    void __fastcall PanelMimimizeClick(TObject *Sender);
    void __fastcall PanelMaximizeClick(TObject *Sender);
    void __fastcall ebEditorPreferencesClick(TObject *Sender);
	void __fastcall ebResetAnimationClick(TObject *Sender);
	void __fastcall ebSceneFileMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall tvMotionsMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall CreateFolder1Click(TObject *Sender);
	void __fastcall tvMotionsStartDrag(TObject *Sender,
          TDragObject *&DragObject);
	void __fastcall tvMotionsDragOver(TObject *Sender, TObject *Source, int X,
          int Y, TDragState State, bool &Accept);
	void __fastcall tvMotionsDragDrop(TObject *Sender, TObject *Source, int X,
          int Y);
	void __fastcall Import1Click(TObject *Sender);
	void __fastcall Load1Click(TObject *Sender);
	void __fastcall Save2Click(TObject *Sender);
	void __fastcall SaevAs1Click(TObject *Sender);
	void __fastcall fsStorageSavePlacement(TObject *Sender);
	void __fastcall fsStorageRestorePlacement(TObject *Sender);
	void __fastcall miRecentFilesClick(TObject *Sender);
	void __fastcall ebBonePartClick(TObject *Sender);
	void __fastcall miExportOGFClick(TObject *Sender);
	void __fastcall ebMakePreviewClick(TObject *Sender);
	void __fastcall ebRenderStyleClick(TObject *Sender);
	void __fastcall Custom1Click(TObject *Sender);
	void __fastcall none1Click(TObject *Sender);
	void __fastcall Preferences1Click(TObject *Sender);
	void __fastcall ebPreviewObjectClickMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall Clear1Click(TObject *Sender);
	void __fastcall Refresh1Click(TObject *Sender);
	void __fastcall Checknewtextures1Click(TObject *Sender);
	void __fastcall ebSceneCommands1MouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall ImageEditor1Click(TObject *Sender);
private:	// User declarations
	void __fastcall ShowPPMenu		(TMxPopupMenu* M, TObject* btn);
	void __fastcall RenameItem		(LPCSTR p0, LPCSTR p1);
public:		// User declarations
        __fastcall TfraLeftBar		(TComponent* Owner);
    void 			UpdateBar		();
	void 			AddMotion		(LPCSTR full_name, bool bLoadMode);
    void			SetRenderStyle	(bool bEngineStyle);
    void 			MinimizeAllFrames();
    void 			MaximizeAllFrames();
};
//---------------------------------------------------------------------------
extern PACKAGE TfraLeftBar *fraLeftBar;
//---------------------------------------------------------------------------
#endif
