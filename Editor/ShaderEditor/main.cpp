//---------------------------------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "main.h"
TfrmMain *frmMain;
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

#include "ui_main.h"
#include "topbar.h"
#include "leftbar.h"
#include "bottombar.h"

#define EulFrmS	     0
#define EulFrmR	     1
#define EulFrm(ord)  ((unsigned)(ord)&1)
#define EulRepNo     0
#define EulRepYes    1
#define EulRep(ord)  (((unsigned)(ord)>>1)&1)
#define EulParEven   0
#define EulParOdd    1
#define EulPar(ord)  (((unsigned)(ord)>>2)&1)
#define EulSafe	     "\000\001\002\000"
#define EulNext	     "\001\002\000\001"
#define EulAxI(ord)  ((int)(EulSafe[(((unsigned)(ord)>>3)&3)]))
#define EulAxJ(ord)  ((int)(EulNext[EulAxI(ord)+(EulPar(ord)==EulParOdd)]))
#define EulAxK(ord)  ((int)(EulNext[EulAxI(ord)+(EulPar(ord)!=EulParOdd)]))
#define EulAxH(ord)  ((EulRep(ord)==EulRepNo)?EulAxK(ord):EulAxI(ord))
    /* EulGetOrd unpacks all useful information about order simultaneously. */
#define EulGetOrd(ord,i,j,k,h,n,s,f) {unsigned o=ord;f=o&1;o>>=1;s=o&1;o>>=1;\
    n=o&1;o>>=1;i=EulSafe[o&3];j=EulNext[i+n];k=EulNext[i+1-n];h=s?k:i;}
    /* EulOrd creates an order value between 0 and 23 from 4-tuple choices. */
#define EulOrd(i,p,r,f)	   (((((((i)<<1)+(p))<<1)+(r))<<1)+(f))
    /* Static axes */
#define EulOrdXYZs    EulOrd(X,EulParEven,EulRepNo,EulFrmS)
#define EulOrdXYXs    EulOrd(X,EulParEven,EulRepYes,EulFrmS)
#define EulOrdXZYs    EulOrd(X,EulParOdd,EulRepNo,EulFrmS)
#define EulOrdXZXs    EulOrd(X,EulParOdd,EulRepYes,EulFrmS)
#define EulOrdYZXs    EulOrd(Y,EulParEven,EulRepNo,EulFrmS)
#define EulOrdYZYs    EulOrd(Y,EulParEven,EulRepYes,EulFrmS)
#define EulOrdYXZs    EulOrd(Y,EulParOdd,EulRepNo,EulFrmS)
#define EulOrdYXYs    EulOrd(Y,EulParOdd,EulRepYes,EulFrmS)
#define EulOrdZXYs    EulOrd(Z,EulParEven,EulRepNo,EulFrmS)
#define EulOrdZXZs    EulOrd(Z,EulParEven,EulRepYes,EulFrmS)
#define EulOrdZYXs    EulOrd(Z,EulParOdd,EulRepNo,EulFrmS)
#define EulOrdZYZs    EulOrd(Z,EulParOdd,EulRepYes,EulFrmS)
    /* Rotating axes */
#define EulOrdZYXr    EulOrd(X,EulParEven,EulRepNo,EulFrmR)
#define EulOrdXYXr    EulOrd(X,EulParEven,EulRepYes,EulFrmR)
#define EulOrdYZXr    EulOrd(X,EulParOdd,EulRepNo,EulFrmR)
#define EulOrdXZXr    EulOrd(X,EulParOdd,EulRepYes,EulFrmR)
#define EulOrdXZYr    EulOrd(Y,EulParEven,EulRepNo,EulFrmR)
#define EulOrdYZYr    EulOrd(Y,EulParEven,EulRepYes,EulFrmR)
#define EulOrdZXYr    EulOrd(Y,EulParOdd,EulRepNo,EulFrmR)
#define EulOrdYXYr    EulOrd(Y,EulParOdd,EulRepYes,EulFrmR)
#define EulOrdYXZr    EulOrd(Z,EulParEven,EulRepNo,EulFrmR)
#define EulOrdZXZr    EulOrd(Z,EulParEven,EulRepYes,EulFrmR)
#define EulOrdXYZr    EulOrd(Z,EulParOdd,EulRepNo,EulFrmR)
#define EulOrdZYZr    EulOrd(Z,EulParOdd,EulRepYes,EulFrmR)

static int X = 0;
static int Y = 1;
static int Z = 2;
static int W = 3;

/* Construct matrix from Euler angles (in radians). */
void LWEUL_fromMatrix(Fmatrix& M, int order, Fvector4& ea)
{
    int i,j,k,h,n,s,f;
    EulGetOrd(order,i,j,k,h,n,s,f);
    if (s==EulRepYes) {
	float sy = sqrt(M.m[i][j]*M.m[i][j] + M.m[i][k]*M.m[i][k]);
	if (sy > 16.0f*FLT_EPSILON) {
	    ea[X] = atan2(M.m[i][j], M.m[i][k]);
	    ea[Y] = atan2(sy, M.m[i][i]);
	    ea[Z] = atan2(M.m[j][i], -M.m[k][i]);
	} else {
	    ea[X] = atan2(-M.m[j][k], M.m[j][j]);
	    ea[Y] = atan2(sy, M.m[i][i]);
	    ea[Z] = 0;
	}
    } else {
	float cy = sqrt(M.m[i][i]*M.m[i][i] + M.m[j][i]*M.m[j][i]);
	if (cy > 16.0f*FLT_EPSILON) {
	    ea[X] = atan2(M.m[k][j], M.m[k][k]);
	    ea[Y] = atan2(-M.m[k][i], cy);
	    ea[Z] = atan2(M.m[j][i], M.m[i][i]);
	} else {
	    ea[X] = atan2(-M.m[j][k], M.m[j][j]);
	    ea[Y] = atan2(-M.m[k][i], cy);
	    ea[Z] = 0;
	}
    }
    if (n==EulParOdd) {ea[X] = -ea[X]; ea[Y] = - ea[Y]; ea[Z] = -ea[Z];}
    if (f==EulFrmR) {float t = ea[X]; ea[X] = ea[Z]; ea[Z] = t;}
    ea[W] = order;
}
void LWEUL_toMatrix(Fvector4& ea, Fmatrix& M)
{
    float ti, tj, th, ci, cj, ch, si, sj, sh, cc, cs, sc, ss;
    int i,j,k,h,n,s,f;
    EulGetOrd(((unsigned)ea[W]),i,j,k,h,n,s,f);
    if (f==EulFrmR) {float t = ea[X]; ea[X] = ea[Z]; ea[Z] = t;}
    if (n==EulParOdd) {ea[X] = -ea[X]; ea[Y] = -ea[Y]; ea[Z] = -ea[Z];}
    ti = ea[X];	  tj = ea[Y];	th = ea[Z];
    ci = cos(ti); cj = cos(tj); ch = cos(th);
    si = sin(ti); sj = sin(tj); sh = sin(th);
    cc = ci*ch; cs = ci*sh; sc = si*ch; ss = si*sh;
    if (s==EulRepYes) {
	M.m[i][i] = cj;	  	M.m[i][j] =  sj*si;    	M.m[i][k] =  sj*ci;
	M.m[j][i] = sj*sh;  M.m[j][j] = -cj*ss+cc; 	M.m[j][k] = -cj*cs-sc;
	M.m[k][i] = -sj*ch; M.m[k][j] =  cj*sc+cs; 	M.m[k][k] =  cj*cc-ss;
    } else {
	M.m[i][i] = cj*ch; 	M.m[i][j] = sj*sc-cs; 	M.m[i][k] = sj*cc+ss;
	M.m[j][i] = cj*sh; 	M.m[j][j] = sj*ss+cc; 	M.m[j][k] = sj*cs-sc;
	M.m[k][i] = -sj;	M.m[k][j] = cj*si;    	M.m[k][k] = cj*ci;
    }
    M.m[W][X]=M.m[W][Y]=M.m[W][Z]=M.m[X][W]=M.m[Y][W]=M.m[Z][W]=0.0f; M.m[W][W]=1.0f;
}
/*
    float ch, cp, cb, sh, sp, sb, cc, cs, sc, ss;

    ch = cos(h);
    sh = sin(h);
    cp = cos(p);
    sp = sin(p);
    cb = cos(b);
    sb = sin(b);
    cc = ch*cb; cs = ch*sb; sc = sh*cb; ss = sh*sb;

    M.m[0][0] = sp*ss+cc;	M.m[0][1] = cp*sb;	M.m[0][2] = sp*cs-sc;
	M.m[1][0] = sp*sc-cs;	M.m[1][1] = cp*cb; 	M.m[1][2] = sp*cc+ss;
	M.m[2][0] = cp*sh;    	M.m[2][1] = -sp;	M.m[2][2] = cp*ch;

    M.m[3][0]=M.m[3][1]=M.m[3][2]=M.m[0][3]=M.m[1][3]=M.m[2][3]=0.0f; M.m[3][3]=1.0f;

*/
void LWEUL_toMatrix(Fmatrix& M, float& h, float& p, float& b)
{
    float ch, cp, cb, sh, sp, sb, cc, cs, sc, ss;

    _sincos(h,sh,ch);
    _sincos(p,sp,cp);
    _sincos(b,sb,cb);
    cc = ch*cb; cs = ch*sb; sc = sh*cb; ss = sh*sb;

    M.i.set(cc-sp*ss,	-cp*sb,	sp*cs+sc);	M._14_=0;
	M.j.set(sp*sc+cs,	cp*cb, 	ss-sp*cc);  M._24_=0;
	M.k.set(-cp*sh,    	sp,		cp*ch);     M._34_=0;
    M.c.set(0,			0,		0);         M._44_=1;
}

/* Convert matrix to Euler angles (in radians). */
void LWEUL_fromMatrix(Fmatrix& M, float& h, float& p, float& b)
{
	float cy = sqrt(M.m[1][1]*M.m[1][1] + M.m[0][1]*M.m[0][1]);
	if (cy > 16.0f*FLT_EPSILON) {
	    h = -atan2(M.k.x, M.k.z);
	    p = -atan2(-M.k.y, cy);
	    b = -atan2(M.i.y, M.j.y);
	} else {
	    h = -atan2(-M.i.z, M.i.x);
	    p = -atan2(-M.k.y, cy);
	    b = 0;
	}
}

//---------------------------------------------------------------------------
__fastcall TfrmMain::TfrmMain(TComponent* Owner)
        : TForm(Owner)
{
	Device.SetHandle		(Handle,D3DWindow->Handle);
    if (!UI.Command(COMMAND_INITIALIZE)) TerminateProcess(GetCurrentProcess(),-1);
	fsStorage->RestoreFormPlacement();

    Fmatrix m;
    float h1,p1,b1;
    for (float h=-PI_DIV_2; h<PI_DIV_2; h+=0.1)
	    for (float p=-PI_DIV_2; p<PI_DIV_2; p+=0.1)
    		for (float b=-PI_DIV_2; b<PI_DIV_2; b+=0.1){
				m.setHPB(h,p,b);
				m.getHPB(h1,p1,b1);
                if (!fsimilar(h,h1)) Msg("h-[%2.2f,%2.2f]",h,h1);
                if (!fsimilar(p,p1)) Msg("p-[%2.2f,%2.2f]",p,p1);
                if (!fsimilar(b,b1)) Msg("b-[%2.2f,%2.2f]",b,b1);
            }
/*


    Fvector4 ea,ee;
    ea.x=1.25f;	// P
    ea.y=0.65f; // H
    ea.z=0.1f;  // B

    Fmatrix x,y,z;
    x.rotateX(-ea.x);
    y.rotateY(-ea.y);
    z.rotateZ(-ea.z);
    Fmatrix t,_m,__m;
    _m.mul	(y,x);
    _m.mulB (z);

    Fmatrix m,m1;
//    Fvector4 ee;
//    ee.set(ea.z,ea.x,ea.y,ea.w);
//	ee.w=EulOrdZXYr; LWEUL_toMatrix(ee,m);
    LWEUL_toMatrix(m1,ea.y,ea.x,ea.z);

//	LWEUL_fromMatrix(m,ee.w,ee);


	LWEUL_fromMatrix(m1,ee.y,ee.x,ee.z);


//	LWEUL_fromMatrix(m,ee.w,ee);
//    ea.w=EulOrdZXZr; LWEUL_toMatrix(ea,m);
//    ea.w=EulOrdZYXr; LWEUL_toMatrix(ea,m);
//    ea.w=EulOrdZYZr; LWEUL_toMatrix(ea,m);

    __m.setHPB(ea.y,ea.x,-ea.z);
    D3DXMATRIX _x,_y,_z,_r,__r;
	D3DXMatrixRotationX(&_x,-ea.x);
	D3DXMatrixRotationY(&_y,-ea.y);
	D3DXMatrixRotationZ(&_z,-ea.z);
	D3DXMatrixMultiply(&_r,&_x,&_y);
	D3DXMatrixMultiply(&__r,&_z,&_r);
*/    
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::FormShow(TObject *Sender)
{
    fraBottomBar->Parent    = paBottomBar;
    fraTopBar->Parent       = paTopBar;
    fraLeftBar->Parent      = paLeftBar;
    if (paLeftBar->Tag > 0) paLeftBar->Parent = paTopBar;
    else paLeftBar->Parent = frmMain;

    tmRefresh->Enabled = true; tmRefreshTimer(Sender);
    UI.Command				(COMMAND_UPDATE_GRID);
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::FormClose(TObject *Sender, TCloseAction &Action)
{
    Application->OnIdle     = 0;

    fraLeftBar->fsStorage->SaveFormPlacement();
    fraBottomBar->fsStorage->SaveFormPlacement();
    fraTopBar->fsStorage->SaveFormPlacement();

    fraTopBar->Parent       = 0;
    fraLeftBar->Parent      = 0;
    fraBottomBar->Parent    = 0;

    UI.Command(COMMAND_DESTROY);
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::FormCloseQuery(TObject *Sender, bool &CanClose)
{
    tmRefresh->Enabled = false;
    CanClose = UI.Command(COMMAND_EXIT);
    if (!CanClose) tmRefresh->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::FormCreate(TObject *Sender)
{
	DEFINE_INI(fsStorage);
    Application->OnIdle = IdleHandler;
}

//---------------------------------------------------------------------------

#define MIN_PANEL_HEIGHT 17
void __fastcall TfrmMain::sbToolsMinClick(TObject *Sender)
{
    if (paLeftBar->Tag > 0){
        paLeftBar->Parent = frmMain;
        paLeftBar->Tag    = 0;
    }else{
        paLeftBar->Parent = paTopBar;//D3DWindow;
        paLeftBar->Tag    = 1;//paLeftBar->Height;
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::TopClick(TObject *Sender)
{
    if (paLeftBar->Tag > 0){
        paLeftBar->Align  = alRight;
        paLeftBar->Parent = frmMain;
        paLeftBar->Height = paLeftBar->Tag;
        paLeftBar->Tag    = 0;
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::IdleHandler(TObject *Sender, bool &Done)
{
    Done = false;
    UI.Idle();
}
void __fastcall TfrmMain::D3DWindowResize(TObject *Sender)
{
    UI.Resize();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::D3DWindowKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
    ShiftKey = Shift;
    if (!UI.KeyDown(Key, Shift)){UI.ApplyShortCut(Key, Shift);}
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::D3DWindowKeyUp(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
    if (!UI.KeyUp(Key, Shift)){;}
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::D3DWindowKeyPress(TObject *Sender, char &Key)
{
    if (!UI.KeyPress(Key, ShiftKey)){;}
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
    if (!D3DWindow->Focused()) UI.ApplyGlobalShortCut(Key, Shift);
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::UpdateCaption()
{
    AnsiString name;
    name.sprintf("%s - [%s%s]",UI.GetTitle(),UI.GetCaption(),UI.IsModified()?"*":"");
    Caption = name;
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::tmRefreshTimer(TObject *Sender)
{
    int i;
    for (i=0; i<frmMain->ComponentCount; i++){
        TComponent* temp = frmMain->Components[i];
        if (dynamic_cast<TExtBtn *>(temp) != NULL)
            ((TExtBtn*)temp)->UpdateMouseInControl();
    }
    fraLeftBar->UpdateBar();
    fraTopBar->OnTimer();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::D3DWindowPaint(TObject *Sender)
{
    UI.RedrawScene();
}
//---------------------------------------------------------------------------


void __fastcall TfrmMain::fsStorageRestorePlacement(TObject *Sender)
{
    fraLeftBar->fsStorage->RestoreFormPlacement();
    fraBottomBar->fsStorage->RestoreFormPlacement();
    fraTopBar->fsStorage->RestoreFormPlacement();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::paWindowResize(TObject *Sender)
{
	D3DWindow->Left  	= 1;
	D3DWindow->Top  	= 1;
	D3DWindow->Width 	= paWindow->Width-2;
	D3DWindow->Height 	= paWindow->Height-2;
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::D3DWindowChangeFocus(TObject *Sender)
{
	if (!UI.m_bReady) return;
	if (D3DWindow->Focused()){
     	paWindow->Color=TColor(0x090FFFF);
		// если потеряли фокус, а до этого кликнули мышкой -> вызовим событие MouseUp
//        if (UI.IsMouseInUse())
//            UI.OnMouseRelease(0);
        UI.iCapture();
		UI.OnAppActivate();
    }else{
		UI.OnAppDeactivate();
        UI.iRelease();
    	paWindow->Color=(TColor)0x00202020;
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::FormActivate(TObject *Sender)
{
//	UI.OnAppActivate();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::FormDeactivate(TObject *Sender)
{
//	UI.OnAppDeactivate();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::D3DWindowMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
    UI.MousePress(Shift,X,Y);
    UI.RedrawScene();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::D3DWindowMouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
    UI.MouseRelease(Shift,X,Y);
    UI.RedrawScene();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::D3DWindowMouseMove(TObject *Sender,
      TShiftState Shift, int X, int Y)
{
    UI.MouseMove(Shift,X,Y);
}
//---------------------------------------------------------------------------

