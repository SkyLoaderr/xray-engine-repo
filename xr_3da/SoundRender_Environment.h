#pragma once

#include "soundrender.h"

class CSoundRender_Environment
{
public:
	u32				version;
	string64		name;

	float			R_InGain;		//	[-96	..	0]		// 0
	float			R_Mix;			//	[-96	..	0]		// -96
	float			R_Time;			//	[0.01	..	3000]
	float			R_HFRatio;		//	[0.001	..	0.999]

	float			E_WetDry;		//	[0		..	100]	// 0
	float			E_FeedBack;		//	[0		..	100]	// 0
	float			E_Delay;		//	[1		..	2000]	

	float			D_Gain;			//	[-60	..	0]		// 0
	float			D_Edge;			//	[0		..	100]	// 0
	float			D_CenterF;		//	[100	..	8000]	// 8k
	float			D_BandWidth;	//	[100	..	8000]	// 8k
	float			D_Cutoff;		//	[100	..	8000]	// 8k
public:
	void			set_identity	(bool R, bool E, bool D);
	void			set_default		(bool R, bool E, bool D);
	void			clamp			(bool R, bool E, bool D);
	void			lerp			(CSoundRender_Environment& A, CSoundRender_Environment& B, float f);
	void			load			(IReader* fs);
	void			save			(IWriter* fs);

	CSoundRender_Environment(void);
	~CSoundRender_Environment(void);
};

class SoundEnvironment_LIB
{
public:
	DEFINE_VECTOR				(CSoundRender_Environment*,SE_VEC,SE_IT);
private:
	SE_VEC						library;
public:
	void						Load	(LPCSTR name);
	void						Save	(LPCSTR name);
	void						Unload	();
	int							GetID	(LPCSTR name);
	CSoundRender_Environment*	Get		(LPCSTR name);
	CSoundRender_Environment*	Get		(int id);
	CSoundRender_Environment*	Append	(CSoundRender_Environment* parent=0);
	void						Remove	(LPCSTR name);
	void						Remove	(int id);
	SE_VEC&						Library	();
};
