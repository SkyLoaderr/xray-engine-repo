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
void xrMU_Model::calc_lighting	(vector<Fcolor>& dest, Fmatrix& xform, CDB::MODEL* M, LPCSTR L_layer)
{
	// trans-map
	typedef	multimap<float,v_vertices>	mapVert;
	typedef	mapVert::iterator			mapVertIt;
	mapVert								g_trans;

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
	CDB::COLLIDER			DB;
	DB.ray_options			(0);
	vector<R_Light>	Lights = pBuild->L_layers.front().lights;
	if (Lights.empty())		return;

	for (DWORD I = 0; I<m_vertices.size(); I++)
	{
		_vertex*	V	= m_vertices[I];

		// Get ambient factor
		float		v_amb	= 0.f;
		float		v_trans	= 0.f;
		for (u32 f=0; f<V->adjacent.size(); f++)
		{
			_face*	F			=	V->adjacent[f];
			v_amb				+=	F->Shader().vert_ambient;
			v_trans				+=	F->Shader().vert_translucency;
		}
		v_amb					/=	float(V->adjacent.size());
		v_trans					/=	float(V->adjacent.size());
		float v_inv				=	1.f-v_amb;

		Fcolor					vC;
		Fvector					vP,vN;
		vC.set					(0,0,0,0);
		xform.transform_tiny	(vP,V->P);
		Rxform.transform_dir	(vN,V->N);
		vN.normalize			();

		// multi-sample
		const int n_samples		= 8;
		for (u32 sample=0; sample<n_samples; sample++)
		{
			float					a	= 0.2f * float(sample) / float(n_samples);
			Fvector					P,N;
			N.random_dir			(vN,deg2rad(45.f));
			P.mad					(vP,N,a);

			Fcolor					C;
			C.set					(0,0,0,0);
			LightPoint				(&DB, C, P, N, Lights.begin(), Lights.end(), 0);
			vC.r					+=	C.r;
			vC.g					+=	C.g;
			vC.b					+=	C.b;
		}
		vC.mul_rgb				(1/float(n_samples));

		// 
		V->C.r					= vC.r*v_inv+v_amb;
		V->C.g					= vC.g*v_inv+v_amb;
		V->C.b					= vC.b*v_inv+v_amb;
		V->C.a					= v_trans;

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
		mapVertIt	ins			= g_trans.insert(make_pair(key,v_vertices()));
		ins->second.reserve		(32);
		ins->second.push_back	(V);
	}

	// Process all groups
	for (mapVertIt it=g_trans.begin(); it!=g_trans.end(); it++)
	{
		// Unique
		v_vertices&	VL		= it->second;
		std::sort			(VL.begin(),VL.end());
		VL.erase			(unique(VL.begin(),VL.end()),VL.end());

		// Calc summary color
		Fcolor		C;
		C.set		(0,0,0,0);
		for (int v=0; v<int(VL.size()); v++)
		{
			C.r = _max	(C.r,VL[v]->C.r);
			C.g = _max	(C.g,VL[v]->C.g);
			C.b = _max	(C.b,VL[v]->C.b);
		}

		// Calculate final vertex color
		for (v=0; v<int(VL.size()); v++)
		{
			// trans-level
			float	level		= VL[v]->C.a;

			// 
			Fcolor				R;
			R.lerp				(VL[v]->C,C,level);
			R.r					= _max(R.r,VL[v]->C.r);
			R.g					= _max(R.g,VL[v]->C.g);
			R.b					= _max(R.b,VL[v]->C.b);
			VL[v]->C.lerp		(R,g_params.m_lm_amb_color,g_params.m_lm_amb_fogness);
			VL[v]->C.mul_rgb	(.5f);
			VL[v]->C.a			= 1.f;
		}
	}

	// Transfer colors to destination
	dest.resize				(m_vertices.size());
	for (I = 0; I<m_vertices.size(); I++)
	{
		Fvector	ptPos	= m_vertices[I]->P;
		Fcolor	ptColor	= m_vertices[I]->C;

		Fcolor	_C;		_C.set	(0,0,0,0);
		float 	_N		= 0;

		for (u32 T=0; T<m_vertices.size(); T++)
		{
			Fcolor			vC; 
			float			oD	= ptPos.distance_to	(m_vertices[T]->P);
			float			oA  = 1/(1+10*oD*oD);
			vC.set			(m_vertices[T]->C); 
			vC.mul_rgb		(oA);
			_C.r			+=	vC.r;
			_C.g			+=	vC.g;
			_C.b			+=	vC.b;
			_N				+=	oA;
		}

		_C.mul_rgb		(1/(_N+EPS));
		_C.a			= 1.f;
		dest[I].lerp	(_C,ptColor,.5f);
	}
}

void xrMU_Model::calc_lighting		()
{
	clMsg				("model '%s' - REF_lighted.",m_name);
	// calc_lighting		(color,Fidentity,0,0);
	Fcolor ref;
	ref.set				(.5f, .5f, .5f, 1.f);
	color.assign		(m_vertices.size(),ref);
}

float	simple_optimize				(vector<float>& A, vector<float>& B, float& scale, float& bias)
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

void xrMU_Reference::calc_lighting	()
{
	model->calc_lighting	(color,xform,RCAST_Model,0);

	// A*C + D = B
/**/
	clMsg("-----------------");
	{
		vector<vector<REAL> >	A;
		vector<vector<REAL> >	B;
		vector<REAL>			C;
		vector<REAL>			D;

		// build data
		A.clear();	A.resize	(color.size());
		B.clear();	B.resize	(color.size());
		for (u32 it=0; it<color.size(); it++)
		{
			Fcolor&		__A		= model->color	[it];
			A[it].push_back		(__A.r);
			Fcolor&		__B		= color			[it];
			B[it].push_back		(__B.r);
		}
		vfOptimizeParameters	(A,B,C,D);
		c_scale.x			= C[0];
		c_bias.x			= D[0];

		// build data
		A.clear();	A.resize	(color.size());
		B.clear();	B.resize	(color.size());
		for (u32 it=0; it<color.size(); it++)
		{
			Fcolor&		__A		= model->color	[it];
			A[it].push_back		(__A.g);
			Fcolor&		__B		= color			[it];
			B[it].push_back		(__B.g);
		}
		vfOptimizeParameters	(A,B,C,D);
		c_scale.y			= C[0];
		c_bias.y			= D[0];

		// build data
		A.clear();	A.resize	(color.size());
		B.clear();	B.resize	(color.size());
		for (u32 it=0; it<color.size(); it++)
		{
			Fcolor&		__A		= model->color	[it];
			A[it].push_back		(__A.b);
			Fcolor&		__B		= color			[it];
			B[it].push_back		(__B.b);
		}
		vfOptimizeParameters	(A,B,C,D);
		c_scale.z				= C[0];
		c_bias.z				= D[0];

		c_scale.w			= 0;
		c_bias.w			= 1;
		
		//
		
		REAL dResult			= 0.0;
		u32 dwTestCount			= B.size();
		u32 dwParameterCount	= 3;
		A.clear();	A.resize	(color.size());
		B.clear();	B.resize	(color.size());
		for (u32 it=0; it<color.size(); it++)
		{
			Fcolor&		__A		= model->color	[it];
			A[it].push_back		(__A.r);
			A[it].push_back		(__A.g);
			A[it].push_back		(__A.b);

			Fcolor&		__B		= color			[it];
			B[it].push_back		(__B.r);
			B[it].push_back		(__B.g);
			B[it].push_back		(__B.b);
		}
		C.resize				(3);
		D.resize				(3);
		C[0]					= c_scale.x	;
		C[1]					= c_scale.y	;
		C[2]					= c_scale.z	;
		D[0]					= c_bias.x	;
		D[1]					= c_bias.y	;
		D[2]					= c_bias.z	;

		for (u32 i=0; i<dwTestCount; i++) {
			for (u32 j=0; j<dwParameterCount; j++) {

				REAL d = A[i][j]*C[j]+D[j];
				REAL dTemp			= B[i][j] - d;
				dResult				+= dTemp*dTemp;
			}
		}
		clMsg				("%f : scale[%2.2f, %2.2f, %2.2f], bias[%2.2f, %2.2f, %2.2f]",dResult/dwTestCount,
			c_scale.x,c_scale.y,c_scale.z,
			c_bias.x,c_bias.y,c_bias.z
			);
	}
/**
	//
	{
		vector<float>			A;
		vector<float>			B;
		c_scale.set				(1,1,1,0);
		c_bias.set				(0,0,0,1);
		Fvector					E;

		// build data (x)
		A.clear();	B.clear();
		for (u32 it=0; it<color.size(); it++) {
			A.push_back			(model->color	[it].r);
			B.push_back			(color			[it].r);
		}
		E.x	= simple_optimize	(A,B,c_scale.x,c_bias.x);

		// build data (y)
		A.clear();	B.clear();
		for (u32 it=0; it<color.size(); it++) {
			A.push_back			(model->color	[it].g);
			B.push_back			(color			[it].g);
		}
		E.y	= simple_optimize	(A,B,c_scale.y,c_bias.y);

		// build data (z)
		A.clear();	B.clear();
		for (u32 it=0; it<color.size(); it++) {
			A.push_back			(model->color	[it].b);
			B.push_back			(color			[it].b);
		}
		E.z	= simple_optimize	(A,B,c_scale.z,c_bias.z);

		// 
		clMsg				("E[%1.5f,%1.5f,%1.5f], scale[%1.5f, %1.5f, %1.5f], bias[%1.5f, %1.5f, %1.5f]",
			E.x,E.y,E.z,
			c_scale.x,c_scale.y,c_scale.z,
			c_bias.x,c_bias.y,c_bias.z
			);
	}
/**
	// build data
	clMsg("-----------------");
	{
		vector<vector<REAL> >	A(color.size());
		vector<vector<REAL> >	B(color.size());
		vector<REAL>			C;
		vector<REAL>			D;

		for (u32 it=0; it<color.size(); it++)
		{
			Fcolor&		__A		= model->color	[it];
			A[it].push_back		(__A.r);
			A[it].push_back		(__A.g);
			A[it].push_back		(__A.b);

			Fcolor&		__B		= color			[it];
			B[it].push_back		(__B.r);
			B[it].push_back		(__B.g);
			B[it].push_back		(__B.b);
		}
		vfOptimizeParameters	(A,B,C,D);

		// 
		c_scale.x			= C[0];
		c_scale.y			= C[1];
		c_scale.z			= C[2];
		c_scale.w			= 0;

		c_bias.x			= D[0];
		c_bias.y			= D[1];
		c_bias.z			= D[2];
		c_bias.w			= 1;
		clMsg				("scale[%2.2f, %2.2f, %2.2f], bias[%2.2f, %2.2f, %2.2f]",
			c_scale.x,c_scale.y,c_scale.z,
			c_bias.x,c_bias.y,c_bias.z
			);
	}
 /**/
}