//---------------------------------------------------------------------------
#ifndef PropertiesPSDefH
#define PropertiesPSDefH
#include "CloseBtn.hpp"
#include <Classes.hpp>
#include <Controls.hpp>
#include <ExtCtrls.hpp>
#include "multi_color.hpp"
#include "multi_edit.hpp"
#include "RXCtrls.hpp"
#include <StdCtrls.hpp>
#include "Placemnt.hpp"
#include <Dialogs.hpp>
#include "Gradient.hpp"
#include "multi_check.hpp"
#include <ComCtrls.hpp>
#include "FrameEmitter.h"
#include <Forms.hpp>
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
	TRxLabel *RxLabel17;
	TRxLabel *RxLabel19;
	TRxLabel *lbCreator;
	TExtBtn *ebSelectShader;
	TRxLabel *lbShader;
	TRxLabel *lbTexture;
	TExtBtn *ebSelectTexture;
	TEdit *edName;
	TGroupBox *gbParams;
	TRxLabel *RxLabel31;
	TRxLabel *RxLabel1;
	TRxLabel *RxLabel5;
	TRxLabel *RxLabel6;
	TRxLabel *RxLabel7;
	TRxLabel *RxLabel8;
	TRxLabel *RxLabel9;
	TRxLabel *RxLabel10;
	TRxLabel *RxLabel11;
	TRxLabel *RxLabel12;
	TRxLabel *RxLabel13;
	TRxLabel *RxLabel14;
	TRxLabel *RxLabel15;
	TRxLabel *RxLabel21;
	TRxLabel *RxLabel34;
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
	TRxLabel *RxLabel30;
	TRxLabel *RxLabel32;
	TRxLabel *RxLabel33;
	TExtBtn *ExtBtn11;
	TExtBtn *ExtBtn12;
	TRxLabel *RxLabel4;
	TRxLabel *RxLabel20;
	TMultiObjCheck *cbMotionBlur;
	TMultiObjSpinEdit *seBlurTimeStart;
	TMultiObjSpinEdit *seBlurSubdivStart;
	TMultiObjSpinEdit *seBlurTimeEnd;
	TMultiObjSpinEdit *seBlurSubdivEnd;
	TTabSheet *tsFrame;
	TGroupBox *gbVariability;
	TRxLabel *RxLabel2;
	TRxLabel *RxLabel3;
	TRxLabel *RxLabel16;
	TRxLabel *RxLabel23;
	TRxLabel *RxLabel24;
	TRxLabel *RxLabel25;
	TRxLabel *RxLabel26;
	TRxLabel *RxLabel27;
	TRxLabel *RxLabel28;
	TMultiObjSpinEdit *seLifeVar;
	TMultiObjSpinEdit *seSizeVar;
	TMultiObjSpinEdit *seSpeedVar;
	TMultiObjSpinEdit *seColorVarR;
	TMultiObjSpinEdit *seColorVarG;
	TMultiObjSpinEdit *seColorVarB;
	TMultiObjSpinEdit *seColorVarA;
	TMultiObjSpinEdit *seASpeedVar;
	TGroupBox *GroupBox4;
	TRxLabel *RxLabel29;
	TMultiObjCheck *cbAnimEnabled;
	TGroupBox *GroupBox5;
	TRxLabel *RxLabel18;
	TMultiObjSpinEdit *seAnimFrameWidth;
	TRxLabel *RxLabel22;
	TMultiObjSpinEdit *seAnimFrameHeight;
	TRxLabel *RxLabel35;
	TMultiObjSpinEdit *seAnimFrameCount;
	TRxLabel *RxLabel36;
	TMultiObjSpinEdit *seAnimSpeed;
	TRxLabel *RxLabel37;
	TMultiObjSpinEdit *seAnimSpeedVar;
	TGroupBox *GroupBox6;
	TRxLabel *RxLabel38;
	TRxLabel *RxLabel39;
	TMultiObjSpinEdit *seAnimTexWidth;
	TMultiObjSpinEdit *seAnimTexHeight;
	TFormStorage *fsStorage;
	TRxLabel *RxLabel40;
	TMultiObjCheck *cbAlignToPath;
	TRxLabel *RxLabel41;
	TMultiObjCheck *cbRandomInitAngle;
	TRxLabel *RxLabel42;
	TMultiObjCheck *cbAnimAnimate;
	TRxLabel *RxLabel43;
	TMultiObjCheck *cbAnimRandomInitialFrame;
	TRxLabel *RxLabel44;
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
