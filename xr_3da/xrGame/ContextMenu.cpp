#include "stdafx.h"
#include "contextmenu.h"
#include "..\xr_ini.h"
#include "..\xr_gamefont.h"
  
const float fade_speed = 8.0f;

CContextMenu::~CContextMenu(){
	for (vector<MenuItem>::iterator I=Items.begin(); I!=Items.end(); I++){
		Engine.Event.Destroy(I->Event);
		_FREE(I->Name);
		_FREE(I->Param);
	}
	_FREE(Name);
}
void CContextMenu::Load(CInifile* INI, LPCSTR SECT){
	CInifile::Sect& S = INI->ReadSection(SECT);
	for (CInifile::SectIt I=S.begin(); I!=S.end(); I++){
		char	Event[128],Param[128];
		Event[0]=0; Param[0]=0;
		sscanf	(I->second,"%[^,],%s",Event,Param);
		MenuItem	Item;
		Item.Name	= strdup(I->first);
		Item.Event	= Engine.Event.Create(Event);
		Item.Param	= strdup(Param);
		Items.push_back(Item);
	}
}
void CContextMenu::Render(CFontBase* F, DWORD cT, DWORD cI, float s){
	F->Size		(0.05f);
	F->Color	(cT);
	F->OutNext	("%s",Name);
	F->Color	(cI);
	F->Size		(0.03f);
	for (DWORD i=0; i<Items.size(); i++)
		F->OutNext("%d. %s", i, (char*)Items[i].Name);
}
void CContextMenu::Select(int I){
	if (I>=0 && I<int(Items.size())){
		MenuItem& M = Items[I];
		Engine.Event.Signal(M.Event, DWORD(M.Param));
	}
}
