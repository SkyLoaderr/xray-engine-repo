#include "stdafx.h"
#include <dinput.h>
#include "HUDmanager.h"
#include "../xr_ioconsole.h"
#include "entity_alive.h"
#include "game_sv_single.h"
#include "alife_simulator.h"
#include "alife_simulator_header.h"
#include "level_graph.h"
#include "../fdemorecord.h"
#include "level.h"
#include "xr_level_controller.h"
#include "game_cl_base.h"
#include "stalker_movement_manager.h"
#include "Inventory.h"
#include "WeaponHUD.h"
#include "xrServer.h"
#include "autosave_manager.h"

#include "actor.h"
#include "huditem.h"
#include "ui/UIDialogWnd.h"
#include "clsid_game.h"

#ifdef DEBUG
#	include "ai/monsters/BaseMonster/base_monster.h"
#endif

#ifdef DEBUG
#	include "../xr_input.h"
	extern void try_change_current_entity();
	extern void restore_actor();
#endif

bool g_bDisableAllInput = false;
extern	float	g_fTimeFactor;

void CLevel::IR_OnMouseWheel( int direction )
{
	if( Device.Pause()		) return;
	if(	g_bDisableAllInput	) return;

	if (HUD().GetUI()->IR_OnMouseWheel(direction)) return;
	if ( Game().IR_OnMouseWheel(direction) ) return;

	if( HUD().GetUI()->MainInputReceiver() )return;
	if (CurrentEntity())		{
			IInputReceiver*		IR	= smart_cast<IInputReceiver*>	(smart_cast<CGameObject*>(CurrentEntity()));
			if (IR)				IR->IR_OnMouseWheel(direction);
		}

}

// ��������� ������� ������
extern	BOOL	g_bEnableMPL;	//.
void CLevel::IR_OnKeyboardPress	(int key)
{
	//. dbgmp
	if (DIK_NUMPADENTER==key)
	{
		if (g_bEnableMPL)	{
			// turn off
//			Console->Execute("r1_dlights_clip 30");
//			g_bEnableMPL	= FALSE	;
		} else {
			// turn on
//			Console->Execute("r1_dlights_clip 150");
//			g_bEnableMPL	= TRUE	;
		}
	}
	//. dbgmp end

	if (DIK_F12 == key)
	{
		Render->Screenshot();
		return;
	}

	switch (key_binding[key]) {
	case kCONSOLE:
		Console->Show				();
		return;
		break;

	case kQUIT:	{
		if( HUD().GetUI()->MainInputReceiver() ){
				if(HUD().GetUI()->MainInputReceiver()->IR_OnKeyboardPress(key))	return;//special case for mp and main_menu
				HUD().GetUI()->StartStopMenu( HUD().GetUI()->MainInputReceiver(), true);
		}else
			Console->Execute			("main_menu");
		return;
		}break;

	case kPAUSE:
		if (GameID() == GAME_SINGLE)
		{
			Device.Pause(!Device.Pause());
		}
		else
		if (OnServer())
		{
			NET_Packet					net_packet;
			net_packet.w_begin			(M_PAUSE_GAME);
			net_packet.w_u8				(u8(!Device.Pause()));
			Send						(net_packet,net_flags(TRUE));
		}
		
		return;
		break;
	};

	if( Device.Pause()		) return;
	if(	g_bDisableAllInput	) return;

	if (pHUD->GetUI()->IR_OnKeyboardPress(key)) return;
	if ( Game().IR_OnKeyboardPress(key) ) return;

	switch (key) {
#ifdef DEBUG
	case DIK_RETURN:
//		if (GameID() == GAME_SINGLE)
			bDebug	= !bDebug;
		return;
	case DIK_BACK:
		if (GameID() == GAME_SINGLE)
			HW.Caps.SceneMode			= (HW.Caps.SceneMode+1)%3;
		return;
#endif
//	case DIK_F12:
//		Render->Screenshot			();
//		return;
	case DIK_F6: {
		if (GameID() != GAME_SINGLE) return;
//		if (!autosave_manager().ready_for_autosave()) {
//			Msg("! Cannot save the game right now!");
//			return;
//		}
		Console->Execute			("save");
		return;
	}
	case DIK_F7: {
		if (GameID() != GAME_SINGLE) return;
		
		FS.get_path					("$game_config$")->m_Flags.set(FS_Path::flNeedRescan, TRUE);
		FS.get_path					("$game_scripts$")->m_Flags.set(FS_Path::flNeedRescan, TRUE);
		FS.rescan_pathes			();
		NET_Packet					net_packet;
		net_packet.w_begin			(M_RELOAD_GAME);
		Send						(net_packet,net_flags(TRUE));
		return;
	}
#ifdef DEBUG
				 /**/
	case DIK_F4: {
		if (GameID() != GAME_SINGLE) return;
		bool bOk = false;
		u32 i=0, j, n=Objects.o_count();
		if (pCurrentEntity)
			for ( ; i<n; ++i)
				if (Objects.o_get_by_iterator(i) == pCurrentEntity)
					break;
		if (i < n) {
			j = i;
			bOk = false;
			for (++i; i <n; ++i) {
				CEntityAlive* tpEntityAlive = smart_cast<CEntityAlive*>(Objects.o_get_by_iterator(i));
				if (tpEntityAlive) {
					bOk = true;
					break;
				}
			}
			if (!bOk)
				for (i = 0; i <j; ++i) {
					CEntityAlive* tpEntityAlive = smart_cast<CEntityAlive*>(Objects.o_get_by_iterator(i));
					if (tpEntityAlive) {
						bOk = true;
						break;
					}
				}
			if (bOk) {
				CObject *tpObject = CurrentEntity();
				CObject *__I = Objects.o_get_by_iterator(i);
				CObject **I = &__I;
				
				SetEntity(*I);
				if (tpObject != *I)
				{
					CActor* pActor = smart_cast<CActor*> (tpObject);
					if (pActor)
						pActor->inventory().Items_SetCurrentEntityHud(false);
				}
				if (tpObject)
				{
					Engine.Sheduler.Unregister	(tpObject);
					Engine.Sheduler.Register	(tpObject);
				};
				Engine.Sheduler.Unregister	(*I);
				Engine.Sheduler.Register	(*I, TRUE);

				CActor* pActor = smart_cast<CActor*> (*I);
				if (pActor)
				{
					pActor->inventory().Items_SetCurrentEntityHud(true);

					CHudItem* pHudItem = smart_cast<CHudItem*>(pActor->inventory().ActiveItem());
					if (pHudItem) 
					{
						pHudItem->OnStateSwitch(pHudItem->State());
					}
				}
			}
		}
		return;
	}
	case MOUSE_1: {
		if (GameID() != GAME_SINGLE)
			break;
		if (pInput->iGetAsyncKeyState(DIK_LALT)) {
			if (CurrentEntity()->CLS_ID == CLSID_OBJECT_ACTOR)
				try_change_current_entity	();
			else
				restore_actor				();
			return;
		}
		break;
	}
	/**/
#endif


	case DIK_DIVIDE:
		if( OnServer() ){
//			float NewTimeFactor				= pSettings->r_float("alife","time_factor");
			
			if (GameID() == GAME_SINGLE)
				Server->game->SetGameTimeFactor(g_fTimeFactor);
			else
			{
				Server->game->SetEnvironmentGameTimeFactor(g_fTimeFactor);
				Server->game->SetGameTimeFactor(g_fTimeFactor);
			};
		}
		break;	
	case DIK_MULTIPLY:
		if( OnServer() ){
			float NewTimeFactor				= 1000.f;
			if (GameID() == GAME_SINGLE)
				Server->game->SetGameTimeFactor(NewTimeFactor);
			else
			{
				Server->game->SetEnvironmentGameTimeFactor(NewTimeFactor);
//				Server->game->SetGameTimeFactor(NewTimeFactor);
			};
		}
		break;
#ifdef DEBUG
	case DIK_NUMPAD5: 
		{
			if (GameID() != GAME_SINGLE) 
			{
				Msg("For this game type Demo Record is disabled.");
				return;
			};
			Console->Hide	();
			char fn[256];
			FS.update_path(fn,"$game_saves$","1.xrdemo");
//			 strcpy(fn,"x:\\1.xrdemo");
			g_pGameLevel->Cameras.AddEffector(xr_new<CDemoRecord> (fn));
		}
		break;

	case DIK_F9:{
//		if (!ai().get_alife())
//			break;
//		const_cast<CALifeSimulatorHeader&>(ai().alife().header()).set_state(ALife::eZoneStateSurge);
		break;
	}
		return;
	case DIK_F10:{
//		ai().level_graph().set_dest_point();
//		ai().level_graph().build_detail_path();
//		if (!Objects.FindObjectByName("m_stalker_e0000") || !Objects.FindObjectByName("localhost/dima"))
//			return;
//		if (!m_bSynchronization) {
//			m_bSynchronization	= true;
//			ai().level_graph().set_start_point();
//			m_bSynchronization	= false;
//		}
//		luabind::functor<void>	functor;
//		ai().script_engine().functor("alife_test.set_switch_online",functor);
//		functor(0,false);
	}
		return;
	case DIK_F11:
//		ai().level_graph().build_detail_path();
//		if (!Objects.FindObjectByName("m_stalker_e0000") || !Objects.FindObjectByName("localhost/dima"))
//			return;
//		if (!m_bSynchronization) {
//			m_bSynchronization	= true;
//			ai().level_graph().set_dest_point();
//			ai().level_graph().select_cover_point();
//			m_bSynchronization	= false;
//		}
		return;
#endif
	}


	if(bindConsoleCmds.execute(key))
		return;

	if( HUD().GetUI()->MainInputReceiver() )return;
	if (CurrentEntity())		{
			IInputReceiver*		IR	= smart_cast<IInputReceiver*>	(smart_cast<CGameObject*>(CurrentEntity()));
			if (IR)				IR->IR_OnKeyboardPress(key_binding[key]);
		}


	#ifdef DEBUG
		CObject *obj = Level().Objects.FindObjectByName("monster");
		if (obj) {
			CBaseMonster *monster = smart_cast<CBaseMonster *>(obj);
			if (monster) 
				monster->debug_on_key(key);
		}
	#endif
}

void CLevel::IR_OnKeyboardRelease(int key)
{
	if( Device.Pause()		) return;
	if(	g_bDisableAllInput	) return;
	if (pHUD->GetUI()->IR_OnKeyboardRelease(key)) return;
	if ( Game().OnKeyboardRelease(key_binding[key]) ) return;

	if( HUD().GetUI()->MainInputReceiver() )return;
	if (CurrentEntity())		{
		IInputReceiver*		IR	= smart_cast<IInputReceiver*>	(smart_cast<CGameObject*>(CurrentEntity()));
		if (IR)				IR->IR_OnKeyboardRelease			(key_binding[key]);
	}
}

void CLevel::IR_OnKeyboardHold(int key)
{
	if( Device.Pause() ) return;
	if(g_bDisableAllInput) return;

	if( HUD().GetUI()->MainInputReceiver() )return;
	if (CurrentEntity())		{
		IInputReceiver*		IR	= smart_cast<IInputReceiver*>	(smart_cast<CGameObject*>(CurrentEntity()));
		if (IR)				IR->IR_OnKeyboardHold				(key_binding[key]);
	}
}

static int mouse_button_2_key []	=	{MOUSE_1,MOUSE_2,MOUSE_3};

void CLevel::IR_OnMousePress(int btn)
{	IR_OnKeyboardPress(mouse_button_2_key[btn]);}
void CLevel::IR_OnMouseRelease(int btn)
{	IR_OnKeyboardRelease(mouse_button_2_key[btn]);}
void CLevel::IR_OnMouseHold(int btn)
{	IR_OnKeyboardHold(mouse_button_2_key[btn]);}
void CLevel::IR_OnMouseMove( int dx, int dy )
{
	if(g_bDisableAllInput)						return;
	if (pHUD->GetUI()->IR_OnMouseMove(dx,dy))	return;
	if (CurrentEntity())		{
		IInputReceiver*		IR	= smart_cast<IInputReceiver*>	(smart_cast<CGameObject*>(CurrentEntity()));
		if (IR)				IR->IR_OnMouseMove					(dx,dy);
	}
}

void CLevel::IR_OnMouseStop( int /**axis/**/, int /**value/**/)
{
}
