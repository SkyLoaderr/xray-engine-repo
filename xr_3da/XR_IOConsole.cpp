// XR_IOConsole.cpp: implementation of the CConsole class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "x_ray.h"
#include "xr_creator.h"
#include "xr_ioconsole.h"
#include "xr_input.h"
#include "xr_ioc_cmd.h"
#include "xr_sndman.h"
#include "xr_smallfont.h"
#include "xr_tokens.h"

#define  LDIST .05f

ENGINE_API CConsole		Console;
const char *			ioc_prompt=">>> ";
extern vector<char*>	LogFile;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
void CConsole::AddCommand(CConsoleCommand* C)
{
	Commands[LPSTR(C->Name())] = C;
}

void CConsole::Initialize()
{
	scroll_delta	= cmd_delta = old_cmd_delta = 0;
	editor[0]       = 0;
	bShift			= false;
	RecordCommands	= false;
	editor[0]       = 0;
	cur_time		= rep_time = 0;
	fAccel			= 1.0f;
	bVisible		= false;
	rep_time		= 0;

	pFont			= new CFontSmall();
	Device.seqDevDestroy.Add(this);

	// Commands
	extern void CCC_Register();
	CCC_Register();
}

void CConsole::OnDeviceDestroy()
{
}

void CConsole::Destroy	()
{
	Execute						("cfg_save");

	Device.seqDevDestroy.Remove	(this);
	_DELETE						(pFont);
}

void CConsole::OnFrame	()
{
	cur_time+=Device.fTimeDelta;
	rep_time+=Device.fTimeDelta*fAccel;
	if (cur_time>0.1f) { cur_time-=0.1f; bCursor=!bCursor;	}
	if (rep_time>0.2f) { rep_time-=0.2f; bRepeat=true;	fAccel+=0.2f;	}
}

void CConsole::OnRender	()
{
	float			fMaxY;
	BOOL			bGame;

	if (!bVisible) return;

	bGame=false; if (pCreator && pCreator->bReady) bGame=true;

	VERIFY(HW.pDevice);

	//*** Shadow
	D3DRECT R = { 0,0,Device.dwWidth,Device.dwHeight};
	if (bGame) R.y2 /= 2;

	CHK_DX(HW.pDevice->Clear(1,&R,D3DCLEAR_TARGET,D3DCOLOR_XRGB(32,32,32),1,0));

	float dwMaxY=float(Device.dwHeight);
	float dwMaxX=float(Device.dwWidth/2);
	if (bGame) { fMaxY=0.f; dwMaxY/=2; } else fMaxY=1.f;

	char buf[96];
	strcpy(buf,ioc_prompt);
	strcat(buf,editor);
	if (bCursor) strcat(buf,"|");

	pFont->Color( D3DCOLOR_RGBA(128  ,128  ,255, 255) );
	pFont->Size	( 0.019f );
	pFont->Out	( -1.f, fMaxY-LDIST, "%s", buf );

	float ypos=fMaxY-LDIST-LDIST;
	for (int i=LogFile.size()-1-scroll_delta; i>=0; i--) {
		ypos-=LDIST;
		if (ypos<-1.f) break;
		switch (LogFile[i][0]) {
		case '~':
			pFont->Color(D3DCOLOR_RGBA(0  ,0  ,255, 255));
			pFont->Out  (-1.f,ypos,"%s",&(LogFile[i][2]));
			break;
		case '!':
			pFont->Color(D3DCOLOR_RGBA(255,0  ,0  , 255));
			pFont->Out  (-1.f,ypos,"%s",&(LogFile[i][2]));
			break;
		case '*':
			pFont->Color(D3DCOLOR_RGBA(128,128,128, 255));
			pFont->Out  (-1.f,ypos,"%s",&(LogFile[i][2]));
			break;
		case '-':
			pFont->Color(D3DCOLOR_RGBA(0  ,255,0  , 255));
			pFont->Out  (-1.f,ypos,"%s",&(LogFile[i][2]));
			break;
		default:
			pFont->Color(D3DCOLOR_RGBA(255,255,255, 255));
			pFont->Out  (-1.f,ypos,"%s",LogFile[i]);
		}
	}
	pFont->OnRender();
}

void CConsole::OnPressKey(int dik, BOOL bHold)
{
	if (!bHold)	fAccel			= 1.0f;

	switch (dik) {
	case DIK_ESCAPE:
		if (!bHold) {
			if  (pCreator) Hide();
		}
		break;
	case DIK_GRAVE:
		if (pCreator && !bHold) Hide();
		break;
	case DIK_PRIOR:
		scroll_delta++;
		if (scroll_delta>int(LogFile.size())-1) scroll_delta=LogFile.size()-1;
		break;
	case DIK_NEXT:
		scroll_delta--;
		if (scroll_delta<0) scroll_delta=0;
		break;
	case DIK_TAB:
		{
			vecCMD_IT I = Commands.lower_bound(editor);
			if (I!=Commands.end()) {
				CConsoleCommand &O = *(I->second);
				strcpy(editor,O.Name());
				strcat(editor," ");
			}
		}
		break;
	case DIK_UP:
		cmd_delta--;
		SelectCommand();
		break;
	case DIK_DOWN:
		cmd_delta++;
		SelectCommand();
		break;
	case DIK_BACK:
		if (strlen(editor)>0) editor[strlen(editor)-1]=0;
		break;
	case DIK_LSHIFT:
	case DIK_RSHIFT:
		bShift = true;
		break;
	case DIK_0:	strcat(editor,"0");	break;
	case DIK_1:	strcat(editor,"1");	break;
	case DIK_2:	strcat(editor,"2");	break;
	case DIK_3:	strcat(editor,"3");	break;
	case DIK_4:	strcat(editor,"4");	break;
	case DIK_5:	strcat(editor,"5");	break;
	case DIK_6:	strcat(editor,"6");	break;
	case DIK_7:	strcat(editor,"7");	break;
	case DIK_8:	strcat(editor,"8");	break;
	case DIK_9:	strcat(editor,"9");	break;
	case DIK_A:	strcat(editor,"a");	break;
	case DIK_B:	strcat(editor,"b");	break;
	case DIK_C:	strcat(editor,"c");	break;
	case DIK_D:	strcat(editor,"d");	break;
	case DIK_E:	strcat(editor,"e");	break;
	case DIK_F:	strcat(editor,"f");	break;
	case DIK_G:	strcat(editor,"g");	break;
	case DIK_H:	strcat(editor,"h");	break;
	case DIK_I:	strcat(editor,"i");	break;
	case DIK_J:	strcat(editor,"j");	break;
	case DIK_K:	strcat(editor,"k");	break;
	case DIK_L:	strcat(editor,"l");	break;
	case DIK_M:	strcat(editor,"m");	break;
	case DIK_N:	strcat(editor,"n");	break;
	case DIK_O:	strcat(editor,"o");	break;
	case DIK_P:	strcat(editor,"p");	break;
	case DIK_Q:	strcat(editor,"q");	break;
	case DIK_R:	strcat(editor,"r");	break;
	case DIK_S:	strcat(editor,"s");	break;
	case DIK_T:	strcat(editor,"t");	break;
	case DIK_U:	strcat(editor,"u");	break;
	case DIK_V:	strcat(editor,"v");	break;
	case DIK_W:	strcat(editor,"w");	break;
	case DIK_X:	strcat(editor,"x");	break;
	case DIK_Y:	strcat(editor,"y");	break;
	case DIK_Z:	strcat(editor,"z");	break;
	case DIK_SPACE:	strcat(editor," "); break;
	case DIK_COMMA:
		if (bShift) strcat(editor,"<");
		else		strcat(editor,",");
		break;
	case DIK_PERIOD:
		if (bShift) strcat(editor,">");
		else		strcat(editor,".");
		break;
	case DIK_MINUS:
		if (bShift) strcat(editor,"_");
		else		strcat(editor,"-");
		break;
	case 0x27:
		if (bShift) strcat(editor,":");
		else		strcat(editor,";");
		break;
	case DIK_RETURN:
		ExecuteCommand();
		break;
	default:
		break;
	}
	if (strlen(editor)>64) editor[64]=0;
	bRepeat=false;
	rep_time=0;
}

void CConsole::OnKeyboardPress(int dik)
{
	OnPressKey(dik);
}

void CConsole::OnKeyboardRelease(int dik)
{
	fAccel=1.0f;
	rep_time=0;
	switch (dik) {
	case DIK_LSHIFT:
	case DIK_RSHIFT:
		bShift = false;
		break;
	}
}

void CConsole::OnKeyboardHold(int dik)
{
	float fRep = rep_time;
	if (bRepeat) { OnPressKey(dik, true); bRepeat=false; }
	rep_time=fRep;
}

void CConsole::ExecuteCommand()
{
	char	first_word[64];
	char	last_word [64];
	char	converted [256];
	int		i,j,len;

	scroll_delta	= 0;
	cmd_delta		= 0;
	old_cmd_delta	= 0;

	len = strlen(editor);
	for (i=0; i<len; i++) {
		if ((editor[i]=='\n')||(editor[i]=='\t')) editor[i]=' ';
	}
	j   = 0;
	for (i=0; i<len; i++) {
		switch (editor[i]) {
		case ' ':
			if (editor[i+1]==' ') continue;
			if (i==len-1) goto outloop;
			break;
		case ';':
			goto outloop;
		}
		converted[j++]=editor[i];
	}
outloop:
	converted[j]=0;
	if (converted[0]==' ')	strcpy(editor,&(converted[1]));
	else					strcpy(editor,converted);
	if (editor[0]==0)		return;
	if (RecordCommands)		Log("~",editor);
	
	editor[j++  ]=' ';
	editor[len=j]=0;
	for (i=0; i<len; i++) {
		if (editor[i]!=' ') first_word[i]=editor[i];
		else {
			strcpy(editor,&(editor[i+1]));
			break;
		}
	}
	first_word[i]=0;
	strcpy(last_word,editor);
	if (last_word[strlen(last_word)-1]==' ') last_word[strlen(last_word)-1]=0;
	
	vecCMD_IT I = Commands.find(first_word);
	if (I!=Commands.end()) {
		CConsoleCommand &C = *(I->second);
		if (C.bEnabled) {
			if (C.bLowerCaseArgs) strlwr(last_word);
			if (last_word[0]==0) {
				if (C.bEmptyArgsHandled) C.Execute(last_word);
				else {
					CConsoleCommand::TStatus S; C.Status(S);
					Msg("- %s %s",C.Name(),S);
				}
			} else C.Execute(last_word);
		} else {
			Log("! Command disabled.");
		}
	}
	else Log("! Unknown command: ",first_word);
	editor[0]=0;
}

void CConsole::Show()
{
	if (bVisible) return;
	bVisible = true;

	editor[0]       = 0;
	rep_time		= 0;
	fAccel			= 1.0f;

	iCapture				( );
	Device.seqRender.Add	(this, REG_PRIORITY_NORMAL-5);
	Device.seqFrame.Add		(this);
}

void CConsole::Hide()
{
	if (!bVisible) return;
	bVisible = false;

	Device.seqFrame.Remove	(this);
	Device.seqRender.Remove	(this);
	iRelease				( );
}

void CConsole::SelectCommand()
{
	int		p,k;
	BOOL	found=false;
	for (p=LogFile.size()-1, k=0; p>=0; p--) {
		if (LogFile[p][0]=='~') {
			k--;
			if (k==cmd_delta) {
				strcpy(editor,&(LogFile[p][2]));
				found=true;
			}
		}
	}
	if (!found) {
		if (cmd_delta>old_cmd_delta) editor[0]=0;
		cmd_delta=old_cmd_delta;

	} else {
		old_cmd_delta=cmd_delta;
	}
}

void CConsole::Execute(char *cmd)
{
	strncpy(editor,cmd,63); editor[63]=0;
	RecordCommands	= false;
	ExecuteCommand();
	RecordCommands	= true;
}

/*
BOOL CConsole::GetBool(char *name)
{
	ioc_command *cmd = (ioc_command *)bsearch(name, ioc_cmd_array,ioc_num_cmd,sizeof(ioc_command),ioc_compare_search_cmd);
	if (cmd!=NULL && cmd->type==cmdMASK) {
		DWORD *v  = (DWORD *) cmd->ptr;
		DWORD mask= cmd->mask;
		return BOOL(*v & mask);
	}
	return FALSE;
}

FLOAT CConsole::GetFloat(char *name)
{
	ioc_command *cmd = (ioc_command *)bsearch(name, ioc_cmd_array,ioc_num_cmd,sizeof(ioc_command),ioc_compare_search_cmd);
	if (cmd!=NULL && cmd->type==cmdFLOAT) {
		float *v = (float *) cmd->ptr;
		return *v;
	}
	return 0.f;
}

char * CConsole::GetValue(char *name)
{
	ioc_command *cmd = (ioc_command *)bsearch(name, ioc_cmd_array,ioc_num_cmd,sizeof(ioc_command),ioc_compare_search_cmd);
	if (cmd!=NULL && cmd->type==cmdVALUE) {
		DWORD *v = (DWORD *) cmd->ptr; // pointer to value
		xr_token *tok=cmd->tok;
		while (tok->name) {
			if (tok->id==(int)(*v)) {
				return (char *)tok->name;
			}
			tok++;
		}
	}
	return " ";
}

char * CConsole::GetNextValue(char *name)
{
	ioc_command *cmd = (ioc_command *)bsearch(name, ioc_cmd_array,ioc_num_cmd,sizeof(ioc_command),ioc_compare_search_cmd);
	if (cmd!=NULL && cmd->type==cmdVALUE) {
		DWORD *v = (DWORD *) cmd->ptr; // pointer to value
		xr_token *tok=cmd->tok;
		while (tok->name) {
			if (tok->id==(int)(*v)) {
				char *save = (char *)tok->name;
				tok++;
				if (tok->name!=0) return (char *)tok->name;
				else				 return save;
			}
			tok++;
		}
	}
	return GetValue(name);
}

char * CConsole::GetPrevValue(char *name)
{
	ioc_command *cmd = (ioc_command *)bsearch(name, ioc_cmd_array,ioc_num_cmd,sizeof(ioc_command),ioc_compare_search_cmd);
	if (cmd!=NULL && cmd->type==cmdVALUE) {
		DWORD *v = (DWORD *) cmd->ptr; // pointer to value
		xr_token *tok=cmd->tok;
		while (tok->name) {
			if (tok->id==(int)(*v)) {
				if (tok!=cmd->tok) tok--;
				return (char *)tok->name;
			}
			tok++;
		}
	}
	return GetValue(name);
}
*/
