#include "stdafx.h"
#include "LuaEditor.h"


CString CLuaEditor::GetSelText()
{
  int nStartSel = GetSelectionStart();
  int nEndSel = GetSelectionEnd();
  char* pszSelText = new char[nEndSel - nStartSel + 1];
  GetSelText(pszSelText);
	CString sSelText(pszSelText);
  delete [] pszSelText;

  return sSelText;

}

int CLuaEditor::GetSelText(char* text, BOOL bDirect)
{
  return Sci(SCI_GETSELTEXT, 0, (LPARAM) text/*, bDirect*/);
}


long CLuaEditor::GetSelectionStart(BOOL bDirect)
{
  return Sci(SCI_GETSELECTIONSTART, 0, 0/*, bDirect*/);
}

long CLuaEditor::GetSelectionEnd(BOOL bDirect)
{
  return Sci(SCI_GETSELECTIONEND, 0, 0/*, bDirect*/);
}

int CLuaEditor::GetLength(BOOL bDirect)
{
  return Sci(SCI_GETLENGTH, 0, 0/*, bDirect*/);
}

void CLuaEditor::HideSelection(BOOL normal, BOOL bDirect)
{
  Sci(SCI_HIDESELECTION, (WPARAM) normal, 0/*, bDirect*/);
}

void CLuaEditor::ReplaceSel(const char* text, BOOL bDirect)
{
  Sci(SCI_REPLACESEL, 0, (LPARAM) text/*, bDirect*/);
}

long CLuaEditor::FindText(int flags, TextToFind* ft, BOOL bDirect)
{
  return Sci(SCI_FINDTEXT, (WPARAM) flags, (LPARAM) ft/*, bDirect*/);
}

long CLuaEditor::FindTextNext(int flags, TextToFind* ft, BOOL bDirect)
{
  return Sci(SCI_SEARCHNEXT, (WPARAM) flags, (LPARAM) ft/*, bDirect*/);
}

void CLuaEditor::SetSel(long start, long end, BOOL bDirect)
{
  Sci(SCI_SETSEL, (WPARAM) start, (LPARAM) end/*, bDirect*/);
}

int CLuaEditor::PointXFromPosition(long pos, BOOL bDirect)
{
  return Sci(SCI_POINTXFROMPOSITION, 0, (LPARAM) pos/*, bDirect*/);
}

int CLuaEditor::PointYFromPosition(long pos, BOOL bDirect)
{
  return Sci(SCI_POINTYFROMPOSITION, 0, (LPARAM) pos/*, bDirect*/);
}

void CLuaEditor::createFunctionList(CMenu& mnu)
{
	TextToFind ttf;
	ttf.chrg.cpMin=0;
	ttf.chrg.cpMax=GetLength();
	ttf.lpstrText = "function";
	int r;
	int line,line_length;
	char str_line[1024];
	char trim[] = "\t\n";

	while(-1 != (r=FindText(SCFIND_REGEXP,&ttf)) ){
		line = Sci(SCI_LINEFROMPOSITION,r);
		Sci(SCI_GETLINE,line,(int)&str_line[0]);
		line_length = Sci(SCI_LINELENGTH,line);
		if(str_line[line_length-1]=10) 
			--line_length;
		if(str_line[line_length-1]=13) 
			--line_length;

		str_line[line_length] = 0;
		StrTrim(str_line,trim);
		mnu.AppendMenu(MF_STRING,line+1,str_line);
		ttf.chrg.cpMin = r+1;
	}
}