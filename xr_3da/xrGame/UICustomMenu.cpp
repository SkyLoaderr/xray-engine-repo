#include "stdafx.h"
#include "UICustomMenu.h"
#include "HUDManager.h"
#include "..\xr_trims.h"
#include "..\xr_ioconsole.h"

void CUICustomMenuItem::Execute()			
{
	if (value0&&(value0[0]=='$'))		Console.Execute(value0+1);
	else if (value1&&(value1[0]=='$'))	Console.Execute(value1+1);
	else if (OnExecute)					OnExecute(this);
}
//--------------------------------------------------------------------
CUICustomMenuItem* UIParseMenu	(CUICustomMenu* owner, CInifile* ini, CUICustomMenuItem* root, LPCSTR sect, OnExecuteEvent exec, OnItemDrawEvent draw)
{
	CUICustomMenuItem* I=0;
	string256	buf,buf1;
	string64	buf2;
	int ln_cnt = ini->LineCount(sect);
	if (ln_cnt){
		if (!root)	root		= new CUICustomMenuItem(owner,0,"root",0);
		if (ini->LineExists(sect,"title"))	root->SetTitle(ini->ReadSTRING(sect,"title"));
		for (int i=1; i<=ln_cnt; i++){
			// append if exist menu item
			sprintf(buf,"menu_%d",i);
			if (ini->LineExists(sect,buf)){
				LPCSTR line		= ini->ReadSTRING(sect,buf);	R_ASSERT(_GetItemCount(line)==2);
				LPCSTR	name	= _GetItem(line,0,buf);
				LPCSTR	new_sect= strlwr(_GetItem(line,1,buf1));
				I				= new CUICustomMenuItem(owner,root,name,0,0,0,draw);
				UIParseMenu(owner,ini,I,new_sect,exec,draw);
				root->AppendItem(I);
			}
			// append if exist value item
			sprintf(buf,"item_%d",i);
			if (ini->LineExists(sect,buf)){
				LPCSTR line		= ini->ReadSTRING(sect,buf);	R_ASSERT(_GetItemCount(line)>=2);
				LPCSTR	name	= _GetItem(line,0,buf);
				LPCSTR	value0	= _GetItem(line,1,buf1);
				LPCSTR	value1	= (_GetItemCount(line)>2)?_GetItem(line,2,buf2):0;
				root->AppendItem(new CUICustomMenuItem(owner,root,name,value0,value1,exec,draw));
			}
		}
	}
	return root;
}
//--------------------------------------------------------------------

CUICustomMenuItem* UILoadMenu		(CUICustomMenu* owner, LPCSTR ini_name, LPCSTR sect, OnExecuteEvent exec, OnItemDrawEvent draw)
{
	// check ini exist
	CUICustomMenuItem* I=0;
	string256 fn;
	if (Engine.FS.Exist(fn,Path.GameData,ini_name)){
		CInifile* ini		= CInifile::Create(fn);
		I					= UIParseMenu(owner,ini,0,sect,exec,draw);
		CInifile::Destroy	(ini);
	}
	return I;
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
// Class CUICustomMenu
//--------------------------------------------------------------------
CUICustomMenu::CUICustomMenu()
{
	bVisible		= FALSE;
	menu_root		= 0;	
	menu_active		= 0;
	m_Owner			= 0;
}
CUICustomMenu::~CUICustomMenu()
{
	_DELETE(menu_root);
}

void CUICustomMenu::Load(LPCSTR ini, LPCSTR start_sect, LPVOID owner, OnExecuteEvent exec)
{
	m_Owner				= owner;
	menu_root			= UILoadMenu(this,ini,start_sect,exec);
	menu_active			= menu_root;
}
