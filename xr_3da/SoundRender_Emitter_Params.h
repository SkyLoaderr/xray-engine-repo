#pragma	once

#pragma pack(push,4)
class CSoundRender_EmitterParams	// User
{
public:
	Fvector			position;
	float			volume;
	float			freq;
	float			min_distance;
	float			max_distance;
};
#pragma pack(pop)
