#pragma once

enum	EKeyBinding
{
	kFWD			=	1,
	kBACK			,
	kL_STRAFE		,
	kR_STRAFE		,
	kLEFT			,
	kRIGHT			,
	kUP				,
	kDOWN			,
	kJUMP			,
	kCROUCH			,
	kACCEL			,

	kCAM_1			,
	kCAM_2			,
	kCAM_3			,
	kCAM_4			,
	kCAM_ZOOM_IN	,
	kCAM_ZOOM_OUT	,

	kWPN_1			,
	kWPN_2			,
	kWPN_3			,
	kWPN_4			,
	kWPN_5			,
	kWPN_6			,
	kWPN_7			,
	kWPN_8			,
	kWPN_9			,
	kWPN_NEXT		,
	kWPN_PREV		,
	kWPN_FIRE		,
	kWPN_ZOOM		,

	kUSE			,
	kSCORES			,
	kCHAT			,
	kSCREENSHOT		,
	kQUIT			,

	kFORCEDWORD		= DWORD(-1)
};

struct ENGINE_API _keybind {
	char *	name;
	int		DIK;
};

ENGINE_API extern _keybind	keybind			[];
ENGINE_API extern _keybind	keynames		[];
ENGINE_API extern int		key_binding		[];

#define MOUSE_1		0x100
#define MOUSE_2		0x200
#define MOUSE_3		0x400
