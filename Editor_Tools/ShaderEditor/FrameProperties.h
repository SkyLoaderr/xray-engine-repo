//---------------------------------------------------------------------------


#ifndef FramePropertiesH
#define FramePropertiesH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "ElTree.hpp"
#include "ElTreeBtnEdit.hpp"
#include "ElTreeModalEdit.hpp"
#include "ElTreeSpinEdit.hpp"
#include "ElTreeStdEditors.hpp"
#include "ElXPThemedControl.hpp"
#include "MxMenus.hpp"
#include <Menus.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
// C++ STL headers
#include <vector>
#include <list>
#include <deque>
#include <stack>
#include <map>
#include <set>
#include <algorithm>
#include <functional>
using namespace std;
//---------------------------------------------------------------------------
enum	BPID
{
	BPID_MARKER	= 0,
	BPID_MATRIX,
	BPID_TOKEN,
	BPID_COLOR,
	BPID_INTEGER,
	BPID_FLOAT,
	BPID_BOOL,
	BPID_STRING,
	BPID_FORCEDWORD=DWORD(-1)
};

struct SIntegerParam{
	int min;
    int max;
	int increment;
    SIntegerParam(int mn, int mx, int inc):min(mn),max(mx),increment(inc){;}
};

struct SFloatParam{
	float min;
    float max;
	float increment;
    SFloatParam(float mn, float mx, float inc):min(mn),max(mx),increment(inc){;}
};
typedef vector<AnsiString> StringVec;
//---------------------------------------------------------------------------

class TfraProperties : public TFrame
{
__published:	// IDE-managed Components
	TElTreeInplaceEdit *InplaceEdit;
	TElTreeInplaceButtonEdit *InplaceCustom;
	TElTreeInplaceFloatSpinEdit *InplaceFloat;
	TElTreeInplaceSpinEdit *InplaceNumber;
	TElTreeInplaceModalEdit *InplaceEnum;
	TMxPopupMenu *pmEnum;
	TPanel *paProperties;
	TElTree *tvProperties;
	void __fastcall InplaceEditAfterOperation(TObject *Sender, bool &Accepted,
          bool &DefaultConversion);
	void __fastcall InplaceFloatAfterOperation(TObject *Sender,
          bool &Accepted, bool &DefaultConversion);
	void __fastcall InplaceNumberAfterOperation(TObject *Sender,
          bool &Accepted, bool &DefaultConversion);
	void __fastcall InplaceFloatBeforeOperation(TObject *Sender,
          bool &DefaultConversion);
	void __fastcall InplaceNumberBeforeOperation(TObject *Sender,
          bool &DefaultConversion);
	void __fastcall tvPropertiesClick(TObject *Sender);
	void __fastcall tvPropertiesItemDraw(TObject *Sender, TElTreeItem *Item,
          TCanvas *Surface, TRect &R, int SectionIndex);
	void __fastcall tvPropertiesMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
private:	// User declarations
    void __fastcall PMItemClick(TObject *Sender);
	TElTreeItem* __fastcall AddItem(TElTreeItem* parent, BPID type, LPCSTR key, LPDWORD value=0, LPDWORD p=0);
public:		// User declarations
	__fastcall TfraProperties(TComponent* Owner);
    void __fastcall ShowProperties(LPDWORD data);
    void __fastcall HideProperties();
};
//---------------------------------------------------------------------------
extern PACKAGE TfraProperties *fraProperties;
//---------------------------------------------------------------------------
#endif
