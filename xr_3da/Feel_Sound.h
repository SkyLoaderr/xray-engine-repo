#pragma once

class ENGINE_API CObject;

namespace Feel
{
	class	CSound
	{
		virtual		void	feel_sound_new	(CObject* who, int type, Fvector& Position, float power) {};
	};
};
