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

#include "ui_customtools.h"
#include "ExtBtn.hpp"
#include "mxPlacemnt.hpp"
#include "ElTree.hpp"
#include "ElXPThemedControl.hpp"
#include "ElPanel.hpp"
#include "ElSplit.hpp"
#include "multi_edit.hpp"
// refs
class CEditObject;
class ESceneAIMapTools;
//---------------------------------------------------------------------------
class TfraAIMap : public TFrame
{
__published:	// IDE-managed Components
	TFormStorage *fsStorage;
	TPanel *paObjectList;
	TLabel *Label2;
	TExtBtn *ExtBtn2;
	TPanel *paLink;
	TLabel *Label3;
	TExtBtn *ExtBtn4;
	TExtBtn *ebGenerateMap;
	TExtBtn *ebModeAppend;
	TExtBtn *ebModeRemove;
	TExtBtn *ebUp;
	TExtBtn *ebDown;
	TExtBtn *ebLeft;
	TExtBtn *ebRight;
	TExtBtn *ebFull;
	TExtBtn *ebModeInvert;
	TExtBtn *ebSmoothNodes;
	TLabel *Label5;
	TExtBtn *ebRemove0LNodes;
	TExtBtn *ebRemove1LNodes;
	TExtBtn *ebIgnoreConstraints;
	TExtBtn *ebAutoLink;
	TBevel *Bevel2;
    void __fastcall PanelMinClick(TObject *Sender);
    void __fastcall ExpandClick(TObject *Sender);
	void __fastcall ebGenerateMapClick(TObject *Sender);
	void __fastcall ebChangeCurrentClick(TObject *Sender);
	void __fastcall fsStorageRestorePlacement(TObject *Sender);
	void __fastcall ebDrawSnapObjectsClick(TObject *Sender);
	void __fastcall ebInvertLinkClick(TObject *Sender);
	void __fastcall ebSideClick(TObject *Sender);
	void __fastcall ebSmoothNodesClick(TObject *Sender);
	void __fastcall ebRemove0LNodesClick(TObject *Sender);
	void __fastcall ebRemove1LNodesClick(TObject *Sender);
private:	// User declarations
	ESceneAIMapTools* tools;
public:		// User declarations
        __fastcall 	TfraAIMap(TComponent* Owner, ESceneAIMapTools* _tools);
};
//---------------------------------------------------------------------------
#endif
