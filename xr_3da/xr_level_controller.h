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
	kCROUCH_TOGGLE	,
	kACCEL			,
	kREPAIR			,

	kCAM_1			,
	kCAM_2			,
	kCAM_3			,
	kCAM_4			,
	kCAM_ZOOM_IN	,
	kCAM_ZOOM_OUT	,

	kBINOCULARS		,
	kTORCH			,
	kNIGHT_VISION	,

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
	kWPN_RELOAD		,
	kWPN_ZOOM		,
	kWPN_AMMO		,
	kWPN_FUNC		,

	kUSE			,
	kDROP			,
	kSCORES			,
	kCHAT			,
	kSCREENSHOT		,
	kQUIT			,
	kCONSOLE		,
	kHyperKick		,
	kINVENTORY		,
	kBUY			,
	kSKIN			,
	kTEAM			,
	kPDA			,
	kACTIVE_JOBS	,
	kMAP			,
	kCONTACTS		,
	kEXT_1			,
	kEXT_2			,
	kEXT_3			,
	kEXT_4			,
	kEXT_5			,
	kEXT_6			,
	kEXT_7			,
	kEXT_8			,
	kEXT_9			,
	kEXT_10			,
	kEXT_11			,
	kEXT_12			,
	kEXT_13			,
	kEXT_14			,
	kEXT_15			,
	kFORCEDWORD		= u32(-1)
};

struct _keybind		{
	char *	name;
	int		DIK;
};

extern _keybind	keybind			[];
extern _keybind	keynames		[];
extern int		key_binding		[];
extern void		CCC_RegisterInput();

#define MOUSE_1		0x100
#define MOUSE_2		0x200
#define MOUSE_3		0x400
