#ifndef soundrender_environmentH
#define soundrender_environmentH
#pragma once

class XRSOUND_EDITOR_API		CSoundRender_Environment		: public CSound_environment
{
public:
	u32				version;
	string64		name;

    // I3DL2 Reverb
    float          	L_Room;              	// 	[DSFX_I3DL2REVERB_ROOM_MIN 				.. DSFX_I3DL2REVERB_ROOM_MAX]
    float          	L_RoomHF;            	//  [DSFX_I3DL2REVERB_ROOMHF_MIN 			.. DSFX_I3DL2REVERB_ROOMHF_MAX]
    float           L_RoomRolloffFactor;	//	[DSFX_I3DL2REVERB_ROOMROLLOFFFACTOR_MIN .. DSFX_I3DL2REVERB_ROOMROLLOFFFACTOR_MAX]
    float           L_DecayTime;          	//  [DSFX_I3DL2REVERB_DECAYTIME_MIN 		.. DSFX_I3DL2REVERB_DECAYTIME_MAX]
    float           L_DecayHFRatio;       	//  [DSFX_I3DL2REVERB_DECAYHFRATIO_MIN 		.. DSFX_I3DL2REVERB_DECAYHFRATIO_MAX]
    float          	L_Reflections;         	//  [DSFX_I3DL2REVERB_REFLECTIONS_MIN 		.. DSFX_I3DL2REVERB_REFLECTIONS_MAX]
    float           L_ReflectionsDelay;   	//  [DSFX_I3DL2REVERB_REFLECTIONSDELAY_MIN 	.. DSFX_I3DL2REVERB_REFLECTIONSDELAY_MAX]
    float          	L_Reverb;              	//  [DSFX_I3DL2REVERB_REVERB_MIN 			.. DSFX_I3DL2REVERB_REVERB_MAX]
    float           L_ReverbDelay;        	//  [DSFX_I3DL2REVERB_REVERBDELAY_MIN 		.. DSFX_I3DL2REVERB_REVERBDELAY_MAX]
    float           L_Diffusion;          	//  [DSFX_I3DL2REVERB_DIFFUSION_MIN 		.. DSFX_I3DL2REVERB_DIFFUSION_MAX]
    float           L_Density;            	//  [DSFX_I3DL2REVERB_DENSITY_MIN 			.. DSFX_I3DL2REVERB_DENSITY_MAX]
    float           L_HFReference;        	//  [DSFX_I3DL2REVERB_HFREFERENCE_MIN 		.. DSFX_I3DL2REVERB_HFREFERENCE_MAX]

    // Waves Reverb
	float			R_InGain;				//	[-96	..	0]		// 0
	float			R_Mix;					//	[-96	..	0]		// -96
	float			R_Time;					//	[0.01	..	3000]
	float			R_HFRatio;				//	[0.001	..	0.999]

    // Echo
	float			E_WetDry;				//	[0		..	100]	// 0
	float			E_FeedBack;				//	[0		..	100]	// 0
	float			E_Delay;				//	[1		..	2000]	
public:
	void			set_identity	(bool R, bool E, bool L);
	void			set_default		(bool R, bool E, bool L);
	void			clamp			(bool R, bool E, bool L);
	void			lerp			(CSoundRender_Environment& A, CSoundRender_Environment& B, float f);
	void			load			(IReader* fs);
	void			save			(IWriter* fs);

	CSoundRender_Environment		(void);
	~CSoundRender_Environment		(void);
};

class XRSOUND_EDITOR_API		SoundEnvironment_LIB
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
#endif
