#ifndef pDomainH
#define pDomainH

class PDomain
{
public:
	PAPI::PDomainEnum	type;
    union{
		float	f[9];
        Fvector	v[3];
    };
protected:
	void __fastcall PDomain::OnTypeChange(PropValue* sender);
public:
	PDomain(PAPI::PDomainEnum type = PAPI::PDPoint,	float inA0 = 0.0f,	float inA1 = 0.0f,	float inA2 = 0.0f,
													float inA3 = 0.0f,	float inA4 = 0.0f,	float inA5 = 0.0f,
													float inA6 = 0.0f,	float inA7 = 0.0f,	float inA8 = 0.0f	);
	~PDomain	();
	PDomain 	(const PDomain &in);
	PDomain&	operator = (PDomain& in);
	PDomain&	operator = (PAPI::PDomainEnum);
	
	void		set(PAPI::PDomainEnum t, 			float inA0, float inA1, float inA2,
													float inA3, float inA4, float inA5, 
													float inA6, float inA7, float inA8	);
	void		MoveXYZ		(float x, float y, float z);
	void		RotateXYZ	(float x, float y, float z);
	void		ScaleXYZ	(float x, float y, float z);

	Fvector&	GetCenter();
	
    void 		Render		(u32 color);
    void 		FillProp	(PropItemVec& items, LPCSTR pref);
};
#endif //pDomainH
