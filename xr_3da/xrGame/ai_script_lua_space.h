#pragma once

namespace Lua {
	enum ELuaMessageType {
		eLuaMessageTypeInfo = u32(0),
		eLuaMessageTypeError,
		eLuaMessageTypeMessage = u32(-1),
	};

	int __cdecl LuaOut(ELuaMessageType tLuaMessageType, LPCSTR caFormat, ...);
}

