#include "stdafx.h"
#include "fitter.h"

union var
{
	int		i;
	float	f;
	bool	b;

	operator float()			{ return f; }
	operator int()				{ return i; }
	operator bool()				{ return b; }

	var& operator = (float _f)	{ f=_f;	return *this; }
	var& operator = (int _i)	{ i=_i;	return *this; }
	var& operator = (bool _b)	{ b=_b;	return *this; }

	var()  						{ }
	var(float _f) : f(_f) 		{ }
	var(int _i)	: i(_i)			{ }
	var(bool _b) : b(_b)		{ }
};

/*
var		test;

test	= 0.f;
int	k	= test;

test	= true;
float f = test;

float x = 10.f;
var _x	= var(x);
*/

//-----------------------------------------------------------------------
void xrMU_Model::calc_lighting	(xr_vector<base_color>& dest, Fmatrix& xform, CDB::MODEL* MDL, base_lighting& lights, u32 flags)
{
	// trans-map
	typedef	xr_multimap<float,v_vertices>	mapVert;
	typedef	mapVert::iterator				mapVertIt;
	mapVert									g_trans;
	u32										I;

	// trans-epsilons
	const float eps			= EPS_L;
	const float eps2		= 2.f*eps;

	// calc pure rotation matrix
	Fmatrix Rxform,tmp,R;
	R.set				(xform	);
	R.translate_over	(0,0,0	);
	tmp.transpose		(R		);
	Rxform.invert		(tmp	);

	// Perform lighting
	CDB::COLLIDER				DB;
	DB.ray_options				(0);

	// Disable faces if needed
	BOOL bDisableFaces			= flags&LP_UseFaceDisable;
	if	(bDisableFaces)
		for (I=0; I<m_faces.size(); I++)	m_faces[I]->bDisableShadowCast	= TRUE;

	// Perform lighting
	for (I = 0; I<m_vertices.size(); I++)
	{
		_vertex*	V			= m_vertices[I];

		// Get ambient factor
		float		v_amb		= 0.f;
		float		v_trans		= 0.f;
		for (u32 f=0; f<V->adjacent.size(); f++)
		{
			_face*	F			=	V->adjacent[f];
			v_amb				+=	F->Shader().vert_ambient;
			v_trans				+=	F->Shader().vert_translucency;
		}
		v_amb					/=	float(V->adjacent.size());
		v_trans					/=	float(V->adjacent.size());
		float v_inv				=	1.f-v_amb;

		base_color				vC;
		Fvector					vP,vN;
		xform.transform_tiny	(vP,V->P);
		Rxform.transform_dir	(vN,V->N);
		vN.normalize			();

		// multi-sample
		const int n_samples		= 8;
		for (u32 sample=0; sample<n_samples; sample++)
		{
			float				a	= 0.2f * float(sample) / float(n_samples);
			Fvector				P,N;
			N.random_dir		(vN,deg2rad(45.f));
			P.mad				(vP,N,a);
			LightPoint			(&DB, MDL, vC, P, N, lights, flags, 0);
		}
		vC.scale				(n_samples);
		vC._tmp_				= v_trans;
		V->C					= vC;

		// Search
		const float key			= V->P.x;
		mapVertIt	it			= g_trans.lower_bound	(key);
		mapVertIt	it2			= it;

		// Decrement to the start and inc to end
		while (it!=g_trans.begin() && ((it->first+eps2)>key)) it--;
		while (it2!=g_trans.end() && ((it2->first-eps2)<key)) it2++;
		if (it2!=g_trans.end())	it2++;

		// Search
		for (; it!=it2; it++)
		{
			v_vertices&	VL		= it->second;
			_vertex* Front		= VL.front();
			R_ASSERT			(Front);
			if (Front->P.similar(V->P,eps))
			{
				VL.push_back		(V);
			}
		}

		// Register
		mapVertIt	ins			= g_trans.insert(mk_pair(key,v_vertices()));
		ins->second.reserve		(32);
		ins->second.push_back	(V);
	}

	// Enable faces if needed
	if	(bDisableFaces)
		for (I=0; I<m_faces.size(); I++)	m_faces[I]->bDisableShadowCast	= FALSE;

	// Process all groups
	for (mapVertIt it=g_trans.begin(); it!=g_trans.end(); it++)
	{
		// Unique
		v_vertices&	VL		= it->second;
		std::sort			(VL.begin(),VL.end());
		VL.erase			(std::unique(VL.begin(),VL.end()),VL.end());

		// Calc summary color
		base_color	C;
		for (int v=0; v<int(VL.size()); v++)
			C.max		(VL[v]->C);

		// Calculate final vertex color
		for (v=0; v<int(VL.size()); v++)
		{
			// trans-level
			float	level		= VL[v]->C._tmp_;

			// 
			base_color			R;
			R.lerp				(VL[v]->C,C,level);
			R.max				(VL[v]->C);
			VL[v]->C			= R;
			VL[v]->C.mul		(.5f);
		}
	}

	// Transfer colors to destination
	dest.resize				(m_vertices.size());
	for (I = 0; I<m_vertices.size(); I++)
	{
		Fvector		ptPos	= m_vertices[I]->P;
		base_color	ptColor	= m_vertices[I]->C;

		float 	_N		= 0;
		dest[I]			= ptColor;
	}
}

void xrMU_Model::calc_lighting	()
{
	// BB
	Fbox			BB; 
	BB.invalidate	();
	for (v_vertices_it vit=m_vertices.begin(); vit!=m_vertices.end(); vit++)
		BB.modify	((*vit)->P);

	// Export CForm
	CDB::CollectorPacked	CL	(BB,(u32)m_vertices.size(),(u32)m_faces.size());
	export_cform_rcast		(CL,Fidentity);
	CDB::MODEL*				M	= xr_new<CDB::MODEL>	();
	M->build				(CL.getV(),(u32)CL.getVS(),CL.getT(),(u32)CL.getTS());

	calc_lighting			(color,Fidentity,M,pBuild->L_static,LP_dont_rgb+LP_dont_sun);

	xr_delete				(M);

	clMsg					("model '%s' - REF_lighted.",m_name);
}

float	simple_optimize				(xr_vector<float>& A, xr_vector<float>& B, float& scale, float& bias)
{
	float	accum;
	u32		it;


	float	error	= flt_max;
	float	elements= float(A.size());
	u32		count	= 0;
	clMsg	("---");
	for (;;)
	{
		clMsg			("%d : %f",count,error);

		count++;
		if (count>64)	return error;

		float	old_scale	= scale;
		float	old_bias	= bias;

		//1. scale
		for (accum=0, it=0; it<A.size(); it++)
			accum	+= (B[it]-bias)/A[it];
		float	s	= accum	/ elements;

		//2. bias
		for (accum=0, it=0; it<A.size(); it++)
			accum	+= B[it]-A[it]/scale;
		float	b	= accum	/ elements;

		// mix
		float	conv	= float(count+11.f)*2.f;
		scale			= ((conv-1)*scale+s)/conv;
		bias			= ((conv-1)*bias +b)/conv;

		// error
		for (accum=0, it=0; it<A.size(); it++)
			accum	+= _sqr(B[it] - (A[it]*scale + bias));
		float	err	= _sqrt(accum)/elements;

		if (err<error)	error = err;
		else 
		{
			// exit
			scale	= old_scale;
			bias	= old_bias;
			return	error;
		}
	}
}

void	o_test (int iA, int iB, int count, base_color* A, base_color* B, float& C, float& D)
{
	xr_vector<float>	_A,_B;
	_A.resize			(count);
	_B.resize			(count);
	for (int it=0; it<count; it++)
	{
		base_color& _a	= A[it];	float*	f_a	= (float*)&_a;
		base_color& _b	= B[it];	float*	f_b	= (float*)&_b;
		_A[it]			= f_a[iA];
		_B[it]			= f_b[iB];
	}
	C=1, D=0;
	simple_optimize		(_A,_B,C,D);
}

void xrMU_Reference::calc_lighting	()
{
	static BOOL					bFirst	= TRUE;
	model->calc_lighting		(color,xform,RCAST_Model,pBuild->L_static,LP_DEFAULT);

	R_ASSERT					(color.size()==model->color.size());

	// A*C + D = B
	// build data
	{
		float*	_s=(float*)&c_scale;
		float*	_b=(float*)&c_bias;
		for (u32 index=0; index<5; index++)
			o_test	(4/*hemi*/,index,color.size(),&model->color.front(),&color.front(),_s[index],_b[index]);

		clMsg				("	scale[%2.2f, %2.2f, %2.2f, %2.2f, %2.2f], bias[%2.2f, %2.2f, %2.2f, %2.2f, %2.2f]",
								c_scale.rgb.x,c_scale.rgb.y,c_scale.rgb.z,c_scale.hemi,c_scale.sun,
								c_bias.rgb.x,c_bias.rgb.y,c_bias.rgb.z,c_bias.hemi,c_bias.sun
							);
	}
}

		/*
		xr_vector<xr_vector<REAL> >	A;	A.resize(color.size());
		xr_vector<xr_vector<REAL> >	B;	B.resize(color.size());
		xr_vector<REAL>					C;
		xr_vector<REAL>					D;
		for (u32 it=0; it<color.size(); it++)
		{
			base_color&		__A		= model->color	[it];
			A[it].push_back		(__A.hemi);
			A[it].push_back		(__A.hemi);
			A[it].push_back		(__A.hemi);
			A[it].push_back		(__A.hemi);
			A[it].push_back		(__A.hemi);

			base_color&		__B		= color			[it];
			B[it].push_back		(__B.rgb.x);
			B[it].push_back		(__B.rgb.y);
			B[it].push_back		(__B.rgb.z);
			B[it].push_back		(__B.hemi);
			B[it].push_back		(__B.sun);
		}
		vfOptimizeParameters	(A,B,C,D,REAL(0.000001));

		// 
		c_scale.rgb.x		= C[0];
		c_scale.rgb.y		= C[1];
		c_scale.rgb.z		= C[2];
		c_scale.hemi		= C[3];
		c_scale.sun			= C[4];

		c_bias.rgb.x		= D[0];
		c_bias.rgb.y		= D[1];
		c_bias.rgb.z		= D[2];
		c_bias.hemi			= D[3];
		c_bias.sun			= D[4];
		*/
