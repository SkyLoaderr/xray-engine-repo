//---------------------------------------------------------------------------

#ifndef Unit1H
#define Unit1H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ToolWin.hpp>
#include "RXCtrls.hpp"
#include <Menus.hpp>
#include <ExtCtrls.hpp>
#include <map>
#include <string>
#include <vector>
#include "tunyxml\tinyxml.h"
#include "config.h"
#include "Unit2.h"
#include "Unit3.h"
#include "Unit4.h"
#include "Unit5.h"
#include "Unit6.h"
#include "Unit7.h"
#include "Unit8.h"
#include "database.h"

using namespace std;
typedef map<AnsiString, HANDLE>             lock_list;
typedef map<AnsiString, HANDLE>::iterator   lock_list_i;


std::string _replace (std::string strin, std::string strfind, std::string strrep)
{
    char buf[16384], *out = buf;
    const char *src = strin.c_str (), *find = strfind.c_str (), *rep = strrep.c_str ();
    while (src)
          {
          char *founded = strstr ((char*)src, (char*)find);
          if (!founded)
             {
             strcpy (out, src);
             break;
             }
          int ilen = (int)(founded - src);
          CopyMemory (out, src, ilen);
          out += ilen;
          *out = 0;
          strcat (out, rep);
          out += strlen (rep);
          src = founded + strlen (find);
          }
    return std::string (buf);
}

//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
    TRichEdit *SrcText;
    TToolBar *ToolBar1;
    TToolButton *ToolButton5;
    TRichEdit *DestText;
    TStatusBar *StatusBar1;
    TListBox *FilesList;
    TListBox *StringsList;
    TRxLabel *RxLabel1;
    TRxLabel *RxLabel2;
    TToolButton *ToolButton1;
    TToolButton *DiffButton;
    TToolButton *ToolButton2;
    TToolButton *ToolButton6;
    TToolButton *ToolButton7;
    TToolButton *ToolButton8;
    TToolButton *ToolButton9;
    TToolButton *ToolButton10;
    TToolButton *ToolButton11;
    TToolButton *CheckButton;
    TToolButton *ToolButton12;
    TToolButton *NotransButton;
    TToolButton *ToolButton13;
    TToolButton *DumpButton;
    TToolButton *SaveAllButton;
    TToolButton *ToolButton15;
    TPopupMenu *LockMenu;
    TMenuItem *LockFileMenuItem;
    TMenuItem *UnlockFileMenuItem;
    TTimer *Timer;
    void __fastcall FormShow(TObject *Sender);
    void __fastcall FormDestroy(TObject *Sender);
    void __fastcall FilesListClick(TObject *Sender);
    void __fastcall StringsListClick(TObject *Sender);
    void __fastcall ToolButton5Click(TObject *Sender);
    void __fastcall DestTextExit(TObject *Sender);
    void __fastcall DiffButtonClick(TObject *Sender);
    void __fastcall ToolButton3Click(TObject *Sender);
    void __fastcall ToolButton6Click(TObject *Sender);
    void __fastcall ToolButton8Click(TObject *Sender);
    void __fastcall ToolButton10Click(TObject *Sender);
    void __fastcall CheckButtonClick(TObject *Sender);
    void __fastcall NotransButtonClick(TObject *Sender);
    void __fastcall DumpButtonClick(TObject *Sender);
    void __fastcall FormKeyUp(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall SaveAllButtonClick(TObject *Sender);
    void __fastcall DestTextEnter(TObject *Sender);
    void __fastcall DestTextChange(TObject *Sender);
    void __fastcall FilesListDrawItem(TWinControl *Control, int Index,
          TRect &Rect, TOwnerDrawState State);
    void __fastcall LockFileMenuItemClick(TObject *Sender);
    void __fastcall UnlockFileMenuItemClick(TObject *Sender);
    void __fastcall TimerTimer(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
private:	// User declarations
    bool            m_bChange;
    vsConfig       *m_pConfig;
    CDatabase      *m_SourceLangDB;
    CDatabase      *m_DestLangDB;
    CDatabase      *m_SourceLangI;
    //CDatabase      *m_DestLangI;
    string          m_UserName;

    void            CheckDatabase   (CDatabase *db, CDatabase *imp);
    void            ChangeLanguage  ();
    void            RemoveStrings   ();
public:		// User declarations
    __fastcall TForm1(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
 