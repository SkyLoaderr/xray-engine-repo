#include "stdafx.h"
#include "render.h"

IRender_interface::IRender_interface()		{};
IRender_interface::~IRender_interface()		{};

ENGINE_API	IRender_interface*	Render		= NULL;
