#pragma once

class ENGINE_API CObject;

namespace Feel
{
	class	Sound
	{
	public:
		virtual		void	feel_sound_new	(CObject* who, int type, Fvector& Position, float power) {};
	};
};
