//=============================================================================
//  Filename:   UIMessages.h
//	Created by Roman E. Marchenko, vortex@gsc-game.kiev.ua
//	Copyright 2004. GSC Game World
//	---------------------------------------------------------------------------
//  Список всех сообщений всех контролов и диалоговых окон интерфейса проекта 
//	S.T.A.L.K.E.R. Shadow of chernobyl
//=============================================================================

#ifndef UI_MESSAGES_H_
#define UI_MESSAGES_H_

//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

enum EUIMessages
{
	// CUIWindow
	WINDOW_LBUTTON_DOWN = 0,
	WINDOW_RBUTTON_DOWN,
	WINDOW_LBUTTON_UP,
	WINDOW_RBUTTON_UP,
	WINDOW_MOUSE_MOVE,
	WINDOW_LBUTTON_DB_CLICK,
	WINDOW_KEY_PRESSED,
	WINDOW_KEY_RELEASED,
	WINDOW_MOUSE_CAPTURE_LOST, 
	WINDOW_KEYBOARD_CAPTURE_LOST,

	// CUIStatic
	STATIC_FOCUS_RECEIVED,
	STATIC_FOCUS_LOST,

	// CUIButton
	BUTTON_CLICKED,
	BUTTON_DOWN,

	// CUITabControl
	TAB_CHANGED,

	// CUICheckButton
	CHECK_BUTTON_SET,
	CHECK_BUTTON_RESET, 

	// CUIRadioButton
	RADIOBUTTON_SET,

	// CUIdragDropItem
	DRAG_DROP_ITEM_DRAG,
	DRAG_DROP_ITEM_DROP, 
	DRAG_DROP_ITEM_MOVE,
	DRAG_DROP_ITEM_DB_CLICK,
	DRAG_DROP_ITEM_RBUTTON_CLICK,
	DRAG_DROP_REFRESH_ACTIVE_ITEM,

	// CUIScrollBox
	SCROLLBOX_MOVE,
	SCROLLBOX_STOP,

	// CUIScrollBar
	SCROLLBAR_VSCROLL,
	SCROLLBAR_HSCROLL,

	// CUIListWnd
	LIST_ITEM_CLICKED,

	// CUIInteractiveItem
	INTERACTIVE_ITEM_CLICK,

	// UIPropertiesBox
	PROPERTY_CLICKED,

	// CUIMessageBox
	MESSAGE_BOX_OK_CLICKED,
	MESSAGE_BOX_YES_CLICKED,
	MESSAGE_BOX_NO_CLICKED,
	MESSAGE_BOX_CANCEL_CLICKED,

	// CUITalkDialogWnd
	TALK_DIALOG_TRADE_BUTTON_CLICKED,
	TALK_DIALOG_QUESTION_CLICKED,

	// CUIMapBaground
	MAPSPOT_FOCUS_RECEIVED,
	MAPSPOT_FOCUS_LOST,
	MAPSPOT_CLICKED,
	MAP_MOVED,

	// CUIPdaWnd
	PDA_MAP_SET_ACTIVE_POINT,

	// CUIPdaDialogWnd
	PDA_DIALOG_WND_BACK_BUTTON_CLICKED,
	PDA_DIALOG_WND_MESSAGE_BUTTON_CLICKED,

	// CUIPdaContactsWnd
	PDA_CONTACTS_WND_CONTACT_SELECTED,

	// CUITradeWnd
	TRADE_WND_CLOSED,

	// CUISleepWnd
	SLEEP_WND_PERFORM_BUTTON_CLICKED,

	// CUIOutfitSlot
	UNDRESS_OUTFIT,
	OUTFIT_RETURNED_BACK,

	// CUIArtifactMerger
	ARTEFACT_MERGER_CLOSE_BUTTON_CLICKED,
	ARTEFACT_MERGER_PERFORM_BUTTON_CLICKED,

	// CUIInventroyWnd
	INVENTORY_DROP_ACTION,
	INVENTORY_EAT_ACTION,
	INVENTORY_TO_BELT_ACTION, 
	INVENTORY_TO_SLOT_ACTION,
	INVENTORY_TO_BAG_ACTION,
	INVENTORY_ARTEFACT_MERGER_ACTIVATE,
	INVENTORY_ARTEFACT_MERGER_DEACTIVATE,
	INVENTORY_ATTACH_ADDON, 
	INVENTORY_DETACH_SCOPE_ADDON,
	INVENTORY_DETACH_SILENCER_ADDON,
	INVENTORY_DETACH_GRENADE_LAUNCHER_ADDON,

	// CUIDiaryWnd
	DIARY_RESET_PREV_ACTIVE_ITEM,

	//CUIAnimationBase
    ANIMATION_STOPPED
};

//////////////////////////////////////////////////////////////////////////

#endif //UI_MESSAGES_H_