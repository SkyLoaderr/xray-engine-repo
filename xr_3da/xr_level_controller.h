#pragma once

#define kFWD		1
#define kBACK		2
#define kL_STRAFE	3
#define kR_STRAFE	4
#define kLEFT		5
#define kRIGHT		6
#define kUP			7
#define kDOWN		8

#define kJUMP		9
#define kCROUCH		10
#define kACCEL		11

#define kCAM1		12
#define kCAM2		13
#define kCAM3		14
#define kCAM4		15

#define kFIRE		16
#define kWPN_ZOOM	17

#define kACTION		18
#define kQUIT		19
#define kZOOMIN		20
#define kZOOMOUT	21
#define kWCHANGE	22

struct ENGINE_API _keybind {
	char *	name;
	int		DIK;
};

ENGINE_API extern _keybind	keybind[];
ENGINE_API extern _keybind	keynames[];
ENGINE_API extern int		key_binding[];

#define MOUSE_1		0x100
#define MOUSE_2		0x200
#define MOUSE_3		0x400
