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
