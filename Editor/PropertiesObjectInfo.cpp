#include "stdafx.h"
#pragma hdrstop

#include "PropertiesObject.h"
#include "SceneClassList.h"
#include "EditObject.h"
#include "EditMesh.h"
#include "Texture.h"
#include "xrShader.h"
#include "Shader.h"
#include "ui_main.h"
#include "scene.h"
#include "XRShaderDef.h"
#include "main.h"
#include "motion.h"
//---------------------------------------------------------------------------
// info
//---------------------------------------------------------------------------
void __fastcall TfrmPropertiesObject::tsInfoShow(TObject *Sender)
{
// Set up info
    stNumericSet->Enabled 	= !bMultiSelection;
    if (!bMultiSelection){
        paMatrix->Visible = true;
        paBB->Visible = true;
        if (m_EditObject->IsReference())
            stNumericSet->Enabled = false;

		AnsiString n;
        int i;
        const Fvector& P=m_EditObject->GetPosition();
        for (i=0; i<3; i++) sgTransform->Cells[i+1][0]=n.sprintf("%.3f", P[i]);
        const Fvector& R=m_EditObject->GetRotate();
        for (i=0; i<3; i++) sgTransform->Cells[i+1][1]=n.sprintf("%.1f", rad2deg(R[i]));
        const Fvector& S=m_EditObject->GetScale();
        for (i=0; i<3; i++) sgTransform->Cells[i+1][2]=n.sprintf("%.3f", S[i]);

        Fbox BB;
        m_EditObject->GetBox(BB);
        for (int col=1; col<5; col++){
            Fvector p;
            switch(col){
            case 1: p.set(BB.min);break;
            case 2: p.set(BB.max);break;
            case 3: BB.getsize(p);break;
            case 4: p.set(m_EditObject->GetCenter());break;
            }
            for (int row=0; row<3; row++){
                AnsiString n; n.sprintf("%.3f", p[row]);
                sgBB->Cells[col][row] = n;
            }
        }
    }else{
    	VERIFY(!m_Objects.empty());
        paMatrix->Enabled = false;
        paBB->Enabled = false;
        for (int col=1; col<4; col++) for (int row=0; row<3; row++) sgTransform->Cells[col][row] = "-";
        for (col=1; col<5; col++) for (row=0; row<3; row++) sgBB->Cells[col][row] = "-";
	    ObjectIt _F = m_Objects.begin();
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmPropertiesObject::ebNumericSetMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
    POINT pt;
    GetCursorPos(&pt);
    pmNumericSet->TrackButton = tbLeftButton;
	pmNumericSet->Popup(pt.x,pt.y);
    OnModified(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesObject::Position1Click(TObject *Sender){
    UI->Command(COMMAND_SET_NUMERIC_POSITION,(int)m_EditObject);
    tsInfoShow(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesObject::Rotation1Click(TObject *Sender){
	UI->Command(COMMAND_SET_NUMERIC_ROTATION,(int)m_EditObject);
    tsInfoShow(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesObject::Scale1Click(TObject *Sender){
	UI->Command(COMMAND_SET_NUMERIC_SCALE,(int)m_EditObject);
    tsInfoShow(Sender);
}
//---------------------------------------------------------------------------


