//---------------------------------------------------------------------------
#ifndef PropertiesPSDefH
#define PropertiesPSDefH
#include "FrameEmitter.h"
#include "ExtBtn.hpp"
#include "MXCtrls.hpp"
#include "mxPlacemnt.hpp"
#include "Gradient.hpp"
#include "multi_check.hpp"
#include "multi_edit.hpp"
#include <Classes.hpp>
#include <ComCtrls.hpp>
#include <Controls.hpp>
#include <ExtCtrls.hpp>
#include <Forms.hpp>
#include <StdCtrls.hpp>
//---------------------------------------------------------------------------

#include "particlesystem.h"

class TfrmPropertiesPSDef : public TForm
{
__published:	// IDE-managed Components
	TTimer *tmUpdate;
	TPageControl *pcPS;
	TTabSheet *tsBase;
	TTabSheet *tsParams;
	TTabSheet *tsVary;
	TTabSheet *tsBlur;
	TGroupBox *gbBase;
	TMxLabel *RxLabel17;
	TMxLabel *RxLabel19;
	TMxLabel *lbCreator;
	TExtBtn *ebSelectShader;
	TMxLabel *lbShader;
	TMxLabel *lbTexture;
	TExtBtn *ebSelectTexture;
	TEdit *edName;
	TGroupBox *gbParams;
	TMxLabel *RxLabel31;
	TMxLabel *RxLabel1;
	TMxLabel *RxLabel5;
	TMxLabel *RxLabel6;
	TMxLabel *RxLabel7;
	TMxLabel *RxLabel8;
	TMxLabel *RxLabel9;
	TMxLabel *RxLabel10;
	TMxLabel *RxLabel11;
	TMxLabel *RxLabel12;
	TMxLabel *RxLabel13;
	TMxLabel *RxLabel14;
	TMxLabel *RxLabel15;
	TMxLabel *RxLabel21;
	TMxLabel *RxLabel34;
	TExtBtn *ExtBtn2;
	TExtBtn *ExtBtn3;
	TExtBtn *ExtBtn4;
	TExtBtn *ExtBtn5;
	TExtBtn *ExtBtn6;
	TExtBtn *ExtBtn7;
	TExtBtn *ExtBtn8;
	TExtBtn *ExtBtn9;
	TExtBtn *ExtBtn10;
	TExtBtn *ExtBtn1;
	TMultiObjSpinEdit *seLife;
	TMultiObjSpinEdit *seSizeStart;
	TMultiObjSpinEdit *seGravityStartX;
	TMultiObjSpinEdit *seGravityEndX;
	TMultiObjSpinEdit *seGravityStartY;
	TMultiObjSpinEdit *seGravityEndY;
	TMultiObjSpinEdit *seGravityStartZ;
	TMultiObjSpinEdit *seGravityEndZ;
	TMultiObjSpinEdit *seColorStartR;
	TMultiObjSpinEdit *seColorEndR;
	TMultiObjSpinEdit *seColorStartG;
	TMultiObjSpinEdit *seColorEndG;
	TMultiObjSpinEdit *seColorStartB;
	TMultiObjSpinEdit *seColorEndB;
	TMultiObjSpinEdit *seColorStartA;
	TMultiObjSpinEdit *seColorEndA;
	TMultiObjSpinEdit *seSizeEnd;
	TMultiObjSpinEdit *seASpeedStart;
	TMultiObjSpinEdit *seASpeedEnd;
	TGradient *grColor;
	TMultiObjSpinEdit *seSpeedStart;
	TMultiObjSpinEdit *seSpeedEnd;
	TTabSheet *tsEmitter;
	TGroupBox *GroupBox2;
	TfraEmitter *fraEmitter;
	TGroupBox *GroupBox1;
	TMxLabel *RxLabel30;
	TMxLabel *RxLabel32;
	TMxLabel *RxLabel33;
	TExtBtn *ExtBtn11;
	TExtBtn *ExtBtn12;
	TMxLabel *RxLabel4;
	TMxLabel *RxLabel20;
	TMultiObjCheck *cbMotionBlur;
	TMultiObjSpinEdit *seBlurTimeStart;
	TMultiObjSpinEdit *seBlurSubdivStart;
	TMultiObjSpinEdit *seBlurTimeEnd;
	TMultiObjSpinEdit *seBlurSubdivEnd;
	TTabSheet *tsFrame;
	TGroupBox *gbVariability;
	TMxLabel *RxLabel2;
	TMxLabel *RxLabel3;
	TMxLabel *RxLabel16;
	TMxLabel *RxLabel23;
	TMxLabel *RxLabel24;
	TMxLabel *RxLabel25;
	TMxLabel *RxLabel26;
	TMxLabel *RxLabel27;
	TMxLabel *RxLabel28;
	TMultiObjSpinEdit *seLifeVar;
	TMultiObjSpinEdit *seSizeVar;
	TMultiObjSpinEdit *seSpeedVar;
	TMultiObjSpinEdit *seColorVarR;
	TMultiObjSpinEdit *seColorVarG;
	TMultiObjSpinEdit *seColorVarB;
	TMultiObjSpinEdit *seColorVarA;
	TMultiObjSpinEdit *seASpeedVar;
	TGroupBox *GroupBox4;
	TMxLabel *RxLabel29;
	TMultiObjCheck *cbAnimEnabled;
	TGroupBox *GroupBox5;
	TMxLabel *RxLabel18;
	TMultiObjSpinEdit *seAnimFrameWidth;
	TMxLabel *RxLabel22;
	TMultiObjSpinEdit *seAnimFrameHeight;
	TMxLabel *RxLabel35;
	TMultiObjSpinEdit *seAnimFrameCount;
	TMxLabel *RxLabel36;
	TMultiObjSpinEdit *seAnimSpeed;
	TMxLabel *RxLabel37;
	TMultiObjSpinEdit *seAnimSpeedVar;
	TGroupBox *GroupBox6;
	TMxLabel *RxLabel38;
	TMxLabel *RxLabel39;
	TMultiObjSpinEdit *seAnimTexWidth;
	TMultiObjSpinEdit *seAnimTexHeight;
	TFormStorage *fsStorage;
	TMxLabel *RxLabel40;
	TMultiObjCheck *cbAlignToPath;
	TMxLabel *RxLabel41;
	TMultiObjCheck *cbRandomInitAngle;
	TMxLabel *RxLabel42;
	TMultiObjCheck *cbAnimAnimate;
	TMxLabel *RxLabel43;
	TMultiObjCheck *cbAnimRandomInitialFrame;
	TMxLabel *RxLabel44;
	TMultiObjCheck *cbAnimRandomPlaybackDir;
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall FormShow(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall tmUpdateTimer(TObject *Sender);
	void __fastcall ebSelectShaderClick(TObject *Sender);
	void __fastcall ebSelectTextureClick(TObject *Sender);
	void __fastcall clColorMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall seLWChange(TObject *Sender, int Val);
	void __fastcall seExit(TObject *Sender);
	void __fastcall seKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
	void __fastcall ebBirthFuncMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall ExtBtn2Click(TObject *Sender);
	void __fastcall ExtBtn3Click(TObject *Sender);
	void __fastcall ExtBtn4Click(TObject *Sender);
	void __fastcall ExtBtn5Click(TObject *Sender);
	void __fastcall ExtBtn6Click(TObject *Sender);
	void __fastcall ExtBtn7Click(TObject *Sender);
	void __fastcall ExtBtn8Click(TObject *Sender);
	void __fastcall ExtBtn9Click(TObject *Sender);
	void __fastcall ExtBtn10Click(TObject *Sender);
	void __fastcall ebBirthFuncClick(TObject *Sender);
	void __fastcall ExtBtn1Click(TObject *Sender);
	void __fastcall seEmitterExit(TObject *Sender);
	void __fastcall seEmitterKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
	void __fastcall seEmitterLWChange(TObject *Sender, int Val);
	void __fastcall ExtBtn12Click(TObject *Sender);
	void __fastcall ExtBtn11Click(TObject *Sender);
private:	// User declarations
	static TfrmPropertiesPSDef* form;
    PS::SDef* m_PS;

    bool bSetMode;

    void GetObjectsInfo     ();
    bool ApplyObjectsInfo   ();
public:		// User declarations
    __fastcall TfrmPropertiesPSDef(TComponent* Owner);
    static void __fastcall ShowProperties();
    static void __fastcall HideProperties();
    static bool __fastcall Visible(){return !!form;}
    static TfrmPropertiesPSDef* GetForm(){VERIFY(form); return form;}
};
//---------------------------------------------------------------------------
#endif
