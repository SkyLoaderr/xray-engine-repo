#ifndef PSystemH
#define PSystemH
#pragma once

#ifdef PARTICLEDLL_EXPORTS
#define PARTICLEDLL_API __declspec(dllexport)
#else
#define PARTICLEDLL_API __declspec(dllimport)
#endif

#ifdef _EDITOR
	#define PARTICLEDLL_EDITOR_API PARTICLEDLL_API
#else
	#define PARTICLEDLL_EDITOR_API
#endif

// Actually this must be < sqrt(MAXFLOAT) since we store this value squared.
#define P_MAXFLOAT	1.0e16f

#ifdef MAXINT
#define P_MAXINT	MAXINT
#else
#define P_MAXINT	0x7fffffff
#endif

#define P_EPS		1e-3f

#ifndef M_PI
#define M_PI		3.1415926535897932384626433f
#endif

#define drand48() (((float) rand())/((float) RAND_MAX))
#define srand48(x) srand(x)

namespace PAPI
{
	class pVector	: public Fvector
	{
	public:
		IC pVector	(float ax, float ay, float az)		{ set(ax,ay,az);								}
		IC pVector	()									{}
		IC float length() const							{	return _sqrt(x*x+y*y+z*z);					}
		IC float length2() const						{	return (x*x+y*y+z*z);						}
		IC float operator*(const pVector &a) const		{	return x*a.x + y*a.y + z*a.z;				}
		IC pVector operator*(const float s) const		{	return pVector(x*s, y*s, z*s);				}
		IC pVector operator/(const float s) const		{	float invs = 1.0f / s;	return pVector(x*invs, y*invs, z*invs);	}
		IC pVector operator+(const pVector& a) const	{	return pVector(x+a.x, y+a.y, z+a.z);		}
		IC pVector operator-(const pVector& a) const	{	return pVector(x-a.x, y-a.y, z-a.z);		}
		IC pVector operator-()							{	x = -x;	y = -y;	z = -z;	return *this;		}
		IC pVector& operator+=(const pVector& a)		{	x += a.x;y += a.y;z += a.z;	return *this;	}
		IC pVector& operator-=(const pVector& a)		{	x -= a.x;y -= a.y;z -= a.z;	return *this;	}
		IC pVector& operator*=(const float a)			{	x *= a;	y *= a;	z *= a;	return *this;		}
		IC pVector& operator/=(const float a)			{	float b = 1.0f / a;	x *= b;	y *= b;	z *= b;	return *this;		}
		IC pVector& operator=(const pVector& a)			{	x = a.x;y = a.y;z = a.z;return *this;		}
		IC pVector operator^(const pVector& b) const	{	return pVector(y*b.z-z*b.y,z*b.x-x*b.z,x*b.y-y*b.x);		}
	};

	// refs
	struct PAHeader;
	#pragma pack (push,4)
	// A single particle

	struct Particle
	{
		enum{
			BIRTH		= (1<<0),
			DYING		= (1<<1),
			ANIMATE_CCW	= (1<<1),
		};
		pVector	pos;
		pVector	posB;
		pVector	size;
		pVector	rot;
		pVector	vel;
		pVector	velB;	// Used to compute binormal, normal, etc.
		pVector	color;	// Color must be next to alpha so glColor4fv works.
		float	alpha;	// This is both cunning and scary.
		float	age;
		float	frame;
		Flags32	flags;
	};

	// A effect of particles - Info and an array of Particles
	struct PARTICLEDLL_API ParticleEffect
	{
		int p_count;				// Number of particles currently existing.
		int max_particles;			// Max particles allowed in effect.
		int particles_allocated;	// Actual allocated size.
		Particle list[1];			// Actually, num_particles in size

		IC void Remove(int i)
		{
			Particle& m = list[i];
			if (m.flags.is(Particle::DYING))	m		 = list[--p_count];
			else								m.flags.set(Particle::DYING,TRUE);
		}

		IC BOOL Add(const pVector &pos, const pVector &posB,
			const pVector &size, const pVector &rot, const pVector &vel, const pVector &color,
			const float alpha = 1.0f,
			const float age = 0.0f, float frame=0, u32 flags=Particle::BIRTH)
		{
			if(p_count >= max_particles)
				return FALSE;
			else
			{
				Particle& P = list[p_count];
				P.pos = pos;
				P.posB = posB;
				P.size = size;
				P.rot = rot;
				P.vel = vel;
				P.velB = vel;	// XXX This should be fixed.
				P.color = color;
				P.alpha = alpha;
				P.age = age;
				P.frame = frame;
				P.flags.set(flags);
				p_count++;
				return TRUE;
			}
		}
	};
	//////////////////////////////////////////////////////////////////////
	// Type codes for domains
	PARTICLEDLL_API enum PDomainEnum
	{
		PDPoint = 0,		// Single point
		PDLine = 1,			// Line segment
		PDTriangle = 2,		// Triangle
		PDPlane = 3,		// Arbitrarily-oriented plane
		PDBox = 4,			// Axis-aligned box
		PDSphere = 5,		// Sphere
		PDCylinder = 6,		// Cylinder
		PDCone = 7,			// Cone
		PDBlob = 8,			// Gaussian blob
		PDDisc = 9,			// Arbitrarily-oriented disc
		PDRectangle = 10,	// Rhombus-shaped planar region
		domain_enum_force_dword = DWORD(-1)
	};

	struct pDomain
	{
		PDomainEnum type;	// PABoxDomain, PASphereDomain, PAConeDomain...
		pVector p1, p2;		// Box vertices, Sphere center, Cylinder/Cone ends
		pVector u, v;		// Orthonormal basis vectors for Cylinder/Cone
		float radius1;		// Outer radius
		float radius2;		// Inner radius
		float radius1Sqr;	// Used for fast Within test of spheres,
		float radius2Sqr;	// and for mag. of u and v vectors for plane.

		BOOL Within(const pVector &) const;
		void Generate(pVector &) const;
		// transformation
		void transform(const pDomain& domain, const Fmatrix& m);
		void transform_dir(const pDomain& domain, const Fmatrix& m);
		// This constructor is used when default constructing a
		// ParticleAction that has a pDomain.
		IC pDomain()
		{
		}

		// Construct a domain in the standard way.
		pDomain(PDomainEnum dtype,
			float a0=0.0f, float a1=0.0f, float a2=0.0f,
			float a3=0.0f, float a4=0.0f, float a5=0.0f,
			float a6=0.0f, float a7=0.0f, float a8=0.0f);
	};

	//////////////////////////////////////////////////////////////////////
	// Type codes for all actions
	enum PActionEnum
	{
		PAHeaderID,			// The first action in each list.
		PAAvoidID,			// Avoid entering the domain of space.
		PABounceID,			// Bounce particles off a domain of space.
		PACallActionListID,	// 
		PACopyVertexBID,	// Set the secondary position from current position.
		PADampingID,		// Dampen particle velocities.
		PAExplosionID,		// An Explosion.
		PAFollowID,			// Accelerate toward the previous particle in the effect.
		PAGravitateID,		// Accelerate each particle toward each other particle.
		PAGravityID,		// Acceleration in the given direction.
		PAJetID,			// 
		PAKillOldID,		// 
		PAMatchVelocityID,	// 
		PAMoveID,			// 
		PAOrbitLineID,		// 
		PAOrbitPointID,		// 
		PARandomAccelID,	// 
		PARandomDisplaceID,	// 
		PARandomVelocityID,	// 
		PARestoreID,		// 
		PASinkID,			// 
		PASinkVelocityID,	// 
		PASourceID,			// 
		PASpeedLimitID,		// 
		PATargetColorID,	// 
		PATargetSizeID,		// 
		PATargetRotateID,	// 
		PATargetRotateDID,	// 
		PATargetVelocityID,	// 
		PATargetVelocityDID,// 
		PAVortexID,			// 
		action_enum_force_dword = DWORD(-1)
	};

	///////////////////////////////////////////////////////////////////////////
	// Data types derived from Action.
	struct ParticleAction
	{
		enum{
			ALLOW_TRANSLATE	= (1<<0),
			ALLOW_ROTATE	= (1<<1)
		};
		static float	dt;	// This is copied to here from global state.
		Flags32			m_Flags;
		PActionEnum		type;	// Type field
		ParticleAction	(){m_Flags.zero();}
	};

	// This Methods actually does the particle's action.
	#define Methods	void Execute	(ParticleEffect *pe);\
	void Transform	(const Fmatrix& m);

	struct PAHeader : public ParticleAction
	{
		int actions_allocated;
		int count;			// Total actions in the list.
		float padding[126];	// This must be the largest action.

		Methods
	};

	struct PAAvoid : public ParticleAction
	{
		pDomain positionL;	// Avoid region (in local space)
		pDomain position;	// Avoid region
		float look_ahead;	// how many time units ahead to look
		float magnitude;	// what percent of the way to go each time
		float epsilon;		// add to r^2 for softening

		Methods
	};

	struct PABounce : public ParticleAction
	{
		pDomain positionL;	// Bounce region (in local space)
		pDomain position;	// Bounce region
		float oneMinusFriction;	// Friction tangent to surface
		float resilience;	// Resilence perpendicular to surface
		float cutoffSqr;	// cutoff velocity; friction applies iff v > cutoff

		Methods
	};

	struct PACallActionList : public ParticleAction
	{
		int action_list_num;	// The action list number to call

		Methods
	};

	struct PACopyVertexB : public ParticleAction
	{
		BOOL copy_pos;		// True to copy pos to posB.
		BOOL copy_vel;		// True to copy vel to velB.

		Methods
	};

	struct PADamping : public ParticleAction
	{
		pVector damping;	// Damping constant applied to velocity
		float vlowSqr;		// Low and high cutoff velocities
		float vhighSqr;

		Methods
	};

	struct PAExplosion : public ParticleAction
	{
		pVector centerL;	// The center of the explosion (in local space)
		pVector center;		// The center of the explosion
		float velocity;		// Of shock wave
		float magnitude;	// At unit radius
		float stdev;		// Sharpness or width of shock wave
		float age;			// How long it's been going on
		float epsilon;		// Softening parameter

		Methods
	};

	struct PAFollow : public ParticleAction
	{
		float magnitude;	// The grav of each particle
		float epsilon;		// Softening parameter
		float max_radius;	// Only influence particles within max_radius

		Methods
	};

	struct PAGravitate : public ParticleAction
	{
		float magnitude;	// The grav of each particle
		float epsilon;		// Softening parameter
		float max_radius;	// Only influence particles within max_radius

		Methods
	};

	struct PAGravity : public ParticleAction
	{
		pVector directionL;	// Amount to increment velocity (in local space)
		pVector direction;	// Amount to increment velocity

		Methods
	};

	struct PAJet : public ParticleAction
	{
		pVector	centerL;	// Center of the fan (in local space)
		pDomain accL;		// Acceleration vector domain  (in local space)
		pVector	center;		// Center of the fan
		pDomain acc;		// Acceleration vector domain
		float magnitude;	// Scales acceleration
		float epsilon;		// Softening parameter
		float max_radius;	// Only influence particles within max_radius

		Methods
	};

	struct PAKillOld : public ParticleAction
	{
		float age_limit;		// Exact age at which to kill particles.
		BOOL kill_less_than;	// True to kill particles less than limit.

		Methods
	};

	struct PAMatchVelocity : public ParticleAction
	{
		float magnitude;	// The grav of each particle
		float epsilon;		// Softening parameter
		float max_radius;	// Only influence particles within max_radius

		Methods
	};

	struct PAMove : public ParticleAction
	{

		Methods
	};

	struct PAOrbitLine : public ParticleAction
	{
		pVector pL, axisL;	// Endpoints of line to which particles are attracted (in local space)
		pVector p, axis;	// Endpoints of line to which particles are attracted
		float magnitude;	// Scales acceleration
		float epsilon;		// Softening parameter
		float max_radius;	// Only influence particles within max_radius

		Methods
	};

	struct PAOrbitPoint : public ParticleAction
	{
		pVector centerL;	// Point to which particles are attracted (in local space)
		pVector center;		// Point to which particles are attracted
		float magnitude;	// Scales acceleration
		float epsilon;		// Softening parameter
		float max_radius;	// Only influence particles within max_radius

		Methods
	};

	struct PARandomAccel : public ParticleAction
	{
		pDomain gen_accL;	// The domain of random accelerations.(in local space)
		pDomain gen_acc;	// The domain of random accelerations.

		Methods
	};

	struct PARandomDisplace : public ParticleAction
	{
		pDomain gen_dispL;	// The domain of random displacements.(in local space)
		pDomain gen_disp;	// The domain of random displacements.

		Methods
	};

	struct PARandomVelocity : public ParticleAction
	{
		pDomain gen_velL;	// The domain of random velocities.(in local space)
		pDomain gen_vel;	// The domain of random velocities.

		Methods
	};

	struct PARestore : public ParticleAction
	{
		float time_left;		// Time remaining until they should be in position.

		Methods
	};

	struct PASink : public ParticleAction
	{
		BOOL kill_inside;	// True to dispose of particles *inside* domain
		pDomain positionL;	// Disposal region (in local space)
		pDomain position;	// Disposal region

		Methods
	};

	struct PASinkVelocity : public ParticleAction
	{
		BOOL kill_inside;	// True to dispose of particles with vel *inside* domain
		pDomain velocityL;	// Disposal region (in local space)
		pDomain velocity;	// Disposal region

		Methods
	};

	struct PASpeedLimit : public ParticleAction
	{
		float min_speed;		// Clamp speed to this minimum.
		float max_speed;		// Clamp speed to this maximum.

		Methods
	};

	struct PASource : public ParticleAction
	{
		enum{
			flVertexB_tracks	= (1<<31),// True to get positionB from position.
			flSilent			= (1<<30),
			fl_FORCEDWORD		= u32(-1)
		};
		pDomain positionL;	// Choose a position in this domain. (local_space)
		pDomain velocityL;	// Choose a velocity in this domain. (local_space)
		pDomain position;	// Choose a position in this domain.
		pDomain velocity;	// Choose a velocity in this domain.
		pDomain rot;		// Choose a rotation in this domain.
		pDomain size;		// Choose a size in this domain.
		pDomain color;		// Choose a color in this domain.
		float alpha;		// Alpha of all generated particles
		float particle_rate;// Particles to generate per unit time
		float age;			// Initial age of the particles
		float age_sigma;	// St. dev. of initial age of the particles
		pVector parent_vel;	
		float parent_motion;

		Methods
	};

	struct PATargetColor : public ParticleAction
	{
		pVector color;		// Color to shift towards
		float alpha;		// Alpha value to shift towards
		float scale;		// Amount to shift by (1 == all the way)

		Methods
	};

	struct PATargetSize : public ParticleAction
	{
		pVector size;		// Size to shift towards
		pVector scale;		// Amount to shift by per frame (1 == all the way)

		Methods
	};

	struct PATargetRotate : public ParticleAction
	{
		pVector rot;		// Rotation to shift towards
		float scale;		// Amount to shift by per frame (1 == all the way)

		Methods
	};

	struct PATargetVelocity : public ParticleAction
	{
		pVector velocityL;	// Velocity to shift towards (in local space)
		pVector velocity;	// Velocity to shift towards
		float scale;		// Amount to shift by (1 == all the way)

		Methods
	};

	struct PAVelocityD : public ParticleAction
	{
		pDomain gen_velL;	// The domain of startup velocity.(in local space)
		pDomain gen_vel;	// The domain of startup velocity.

		Methods
	};

	struct PAVortex : public ParticleAction
	{
		pVector centerL;	// Center of vortex (in local space)
		pVector axisL;		// Axis around which vortex is applied (in local space)
		pVector center;		// Center of vortex
		pVector axis;		// Axis around which vortex is applied
		float magnitude;	// Scale for rotation around axis
		float epsilon;		// Softening parameter
		float max_radius;	// Only influence particles within max_radius

		Methods
	};

	// Global state _vector
	struct _ParticleState
	{
		float	dt;
		BOOL	in_call_list;
		BOOL	in_new_list;

		int effect_id;
		int list_id;
		ParticleEffect *peff;
		PAHeader *pact;
		int tid; // Only used in the MP case, but always define it.

		// These are static because all threads access the same effects.
		// All accesses to these should be locked.
		DEFINE_VECTOR(ParticleEffect*,ParticleEffectVec,ParticleEffectVecIt);
		DEFINE_VECTOR(PAHeader*,PAHeaderVec,PAHeaderVecIt);
		static ParticleEffectVec	effect_vec;
		static PAHeaderVec		alist_vec;

		// state part
		Flags32	flags;
		pDomain Size;
		pDomain Vel;
		pDomain VertexB;
		pDomain Color;
		pDomain Rot;
		float	Alpha;
		float	Age;
		float	AgeSigma;
		float	parent_motion;

		_ParticleState();

		// Return an index into the list of particle effects where
		// p_effect_count effects can be added.
		int				GenerateEffects	(int p_effect_count);
		int				GenerateLists	(int alist_count);
		ParticleEffect*	GetEffectPtr	(int p_effect_num);
		PAHeader*		GetListPtr		(int action_list_num);
		// 
		void			ResetState		();
	};

	// All entry points call this to get their particle state.
	// For the non-MP case this is practically a no-op.
	PARTICLEDLL_API		_ParticleState& _GetPState();
	PARTICLEDLL_API		ParticleEffect*	_GetEffectPtr(int p_effect_num);
	PARTICLEDLL_API		PAHeader*		_GetListPtr(int action_list_num);
	#pragma pack( pop ) // push 4

	// State setting calls
	PARTICLEDLL_API void pResetState();

	PARTICLEDLL_API void pColor(float red, float green, float blue, float alpha = 1.0f);

	PARTICLEDLL_API void pColorD(float alpha, PDomainEnum dtype,
		float a0 = 0.0f, float a1 = 0.0f, float a2 = 0.0f,
		float a3 = 0.0f, float a4 = 0.0f, float a5 = 0.0f,
		float a6 = 0.0f, float a7 = 0.0f, float a8 = 0.0f);

	PARTICLEDLL_API void pParentMotion(float scale);

	PARTICLEDLL_API void pRotate(float rot_x, float rot_y=0.f, float rot_z=0.f);

	PARTICLEDLL_API void pRotateD(PDomainEnum dtype,
		float a0 = 0.0f, float a1 = 0.0f, float a2 = 0.0f,
		float a3 = 0.0f, float a4 = 0.0f, float a5 = 0.0f,
		float a6 = 0.0f, float a7 = 0.0f, float a8 = 0.0f);

	PARTICLEDLL_API void pSize(float size_x, float size_y = 1.0f, float size_z = 1.0f);

	PARTICLEDLL_API void pSizeD(PDomainEnum dtype,
		float a0 = 0.0f, float a1 = 0.0f, float a2 = 0.0f,
		float a3 = 0.0f, float a4 = 0.0f, float a5 = 0.0f,
		float a6 = 0.0f, float a7 = 0.0f, float a8 = 0.0f);

	PARTICLEDLL_API void pStartingAge(float age, float sigma = 1.0f);

	PARTICLEDLL_API void pTimeStep(float new_dt);

	PARTICLEDLL_API void pVelocity(float x, float y, float z);

	PARTICLEDLL_API void __cdecl pVelocityD(PDomainEnum dtype,
		float a0 = 0.0f, float a1 = 0.0f, float a2 = 0.0f,
		float a3 = 0.0f, float a4 = 0.0f, float a5 = 0.0f,
		float a6 = 0.0f, float a7 = 0.0f, float a8 = 0.0f);

	PARTICLEDLL_API void pVertexB(float x, float y, float z);

	PARTICLEDLL_API void pVertexBD(PDomainEnum dtype,
		float a0 = 0.0f, float a1 = 0.0f, float a2 = 0.0f,
		float a3 = 0.0f, float a4 = 0.0f, float a5 = 0.0f,
		float a6 = 0.0f, float a7 = 0.0f, float a8 = 0.0f);

	PARTICLEDLL_API void pVertexBTracks(BOOL track_vertex = TRUE);


	// Action List Calls
	PARTICLEDLL_API void pCallActionList(int action_list_num);

	PARTICLEDLL_API void pDeleteActionLists(int action_list_num, int action_list_count = 1);

	PARTICLEDLL_API void pEndActionList();

	PARTICLEDLL_API int pGenActionLists(int action_list_count = 1);

	PARTICLEDLL_API void pNewActionList(int action_list_num);

	PARTICLEDLL_API void pSetActionListParenting(int action_list_num, const Fmatrix& m, const Fvector& velocity);

	PARTICLEDLL_API void pAddActionToList(PAHeader *S);		

	PARTICLEDLL_API void pStopPlaying(int action_list_num);

	PARTICLEDLL_API void pStartPlaying(int action_list_num);

	// Particle Effect Calls

	PARTICLEDLL_API void pCopyEffect(int p_src_effect_num, int index = 0, int copy_count = P_MAXINT);

	PARTICLEDLL_API void pCurrentEffect(int p_effect_num);

	PARTICLEDLL_API void pDeleteParticleEffects(int p_effect_num, int p_effect_count = 1);

	PARTICLEDLL_API void pDrawEffectl(int dlist, BOOL const_size = FALSE,
		BOOL const_color = FALSE, BOOL const_rotation = FALSE);

	PARTICLEDLL_API void pDrawEffectp(int primitive, BOOL const_size = FALSE,
		BOOL const_color = FALSE);

	PARTICLEDLL_API int pGenParticleEffects(int p_effect_count = 1, int max_particles = 0);

	PARTICLEDLL_API int pGetEffectCount();

	PARTICLEDLL_API int pGetParticles(int index, int count, float *position = NULL, float *color = NULL,
		float *vel = NULL, float *size = NULL, float *age = NULL);

	PARTICLEDLL_API int pSetMaxParticlesG(int effect_num, int max_count);

	PARTICLEDLL_API int pSetMaxParticles(int max_count);

	// Actions
	PARTICLEDLL_API void pAvoid(float magnitude, float epsilon, float look_ahead,
		PDomainEnum dtype,
		float a0 = 0.0f, float a1 = 0.0f, float a2 = 0.0f,
		float a3 = 0.0f, float a4 = 0.0f, float a5 = 0.0f,
		float a6 = 0.0f, float a7 = 0.0f, float a8 = 0.0f, BOOL allow_translate=TRUE, BOOL allow_rotate=TRUE);

	PARTICLEDLL_API void pBounce(float friction, float resilience, float cutoff,
		PDomainEnum dtype,
		float a0 = 0.0f, float a1 = 0.0f, float a2 = 0.0f,
		float a3 = 0.0f, float a4 = 0.0f, float a5 = 0.0f,
		float a6 = 0.0f, float a7 = 0.0f, float a8 = 0.0f, BOOL allow_translate=TRUE, BOOL allow_rotate=TRUE);

	PARTICLEDLL_API void pCopyVertexB(BOOL copy_pos = TRUE, BOOL copy_vel = FALSE);

	PARTICLEDLL_API void pDamping(float damping_x, float damping_y, float damping_z,
		float vlow = 0.0f, float vhigh = P_MAXFLOAT);

	PARTICLEDLL_API void pExplosion(float center_x, float center_y, float center_z, float velocity,
		float magnitude, float stdev, float epsilon = P_EPS, float age = 0.0f, BOOL allow_translate=TRUE, BOOL allow_rotate=TRUE);

	PARTICLEDLL_API void pFollow(float magnitude = 1.0f, float epsilon = P_EPS, float max_radius = P_MAXFLOAT);

	PARTICLEDLL_API void pGravitate(float magnitude = 1.0f, float epsilon = P_EPS, float max_radius = P_MAXFLOAT);

	PARTICLEDLL_API void pGravity(float dir_x, float dir_y, float dir_z, BOOL allow_translate=TRUE, BOOL allow_rotate=TRUE);

	PARTICLEDLL_API void pJet(float center_x, float center_y, float center_z, float magnitude = 1.0f,
		float epsilon = P_EPS, float max_radius = P_MAXFLOAT, BOOL allow_translate=TRUE, BOOL allow_rotate=TRUE);

	PARTICLEDLL_API void pKillOld(float age_limit, BOOL kill_less_than = FALSE);

	PARTICLEDLL_API void pMatchVelocity(float magnitude = 1.0f, float epsilon = P_EPS,
		float max_radius = P_MAXFLOAT);

	PARTICLEDLL_API void pMove();

	PARTICLEDLL_API void pOrbitLine(float p_x, float p_y, float p_z,
		float axis_x, float axis_y, float axis_z, float magnitude = 1.0f,
		float epsilon = P_EPS, float max_radius = P_MAXFLOAT, BOOL allow_translate=TRUE, BOOL allow_rotate=TRUE);

	PARTICLEDLL_API void pOrbitPoint(float center_x, float center_y, float center_z,
		float magnitude = 1.0f, float epsilon = P_EPS,
		float max_radius = P_MAXFLOAT, BOOL allow_translate=TRUE, BOOL allow_rotate=TRUE);

	PARTICLEDLL_API void pRandomAccel(PDomainEnum dtype,
		float a0 = 0.0f, float a1 = 0.0f, float a2 = 0.0f,
		float a3 = 0.0f, float a4 = 0.0f, float a5 = 0.0f,
		float a6 = 0.0f, float a7 = 0.0f, float a8 = 0.0f, BOOL allow_translate=TRUE, BOOL allow_rotate=TRUE);

	PARTICLEDLL_API void pRandomDisplace(PDomainEnum dtype,
		float a0 = 0.0f, float a1 = 0.0f, float a2 = 0.0f,
		float a3 = 0.0f, float a4 = 0.0f, float a5 = 0.0f,
		float a6 = 0.0f, float a7 = 0.0f, float a8 = 0.0f, BOOL allow_translate=TRUE, BOOL allow_rotate=TRUE);

	PARTICLEDLL_API void pRandomVelocity(PDomainEnum dtype,
		float a0 = 0.0f, float a1 = 0.0f, float a2 = 0.0f,
		float a3 = 0.0f, float a4 = 0.0f, float a5 = 0.0f,
		float a6 = 0.0f, float a7 = 0.0f, float a8 = 0.0f, BOOL allow_translate=TRUE, BOOL allow_rotate=TRUE);

	PARTICLEDLL_API void pRestore(float time);

	PARTICLEDLL_API void pSink(BOOL kill_inside, PDomainEnum dtype,
		float a0 = 0.0f, float a1 = 0.0f, float a2 = 0.0f,
		float a3 = 0.0f, float a4 = 0.0f, float a5 = 0.0f,
		float a6 = 0.0f, float a7 = 0.0f, float a8 = 0.0f, BOOL allow_translate=TRUE, BOOL allow_rotate=TRUE);

	PARTICLEDLL_API void pSinkVelocity(BOOL kill_inside, PDomainEnum dtype,
		float a0 = 0.0f, float a1 = 0.0f, float a2 = 0.0f,
		float a3 = 0.0f, float a4 = 0.0f, float a5 = 0.0f,
		float a6 = 0.0f, float a7 = 0.0f, float a8 = 0.0f, BOOL allow_translate=TRUE, BOOL allow_rotate=TRUE);

	PARTICLEDLL_API void pSource(float particle_rate, PDomainEnum dtype,
		float a0 = 0.0f, float a1 = 0.0f, float a2 = 0.0f,
		float a3 = 0.0f, float a4 = 0.0f, float a5 = 0.0f,
		float a6 = 0.0f, float a7 = 0.0f, float a8 = 0.0f, BOOL allow_translate=TRUE, BOOL allow_rotate=TRUE);

	PARTICLEDLL_API void pSpeedLimit(float min_speed, float max_speed = P_MAXFLOAT);

	PARTICLEDLL_API void pTargetColor(float color_x, float color_y, float color_z,
		float alpha, float scale);

	PARTICLEDLL_API void pTargetSize(float size_x, float size_y, float size_z,
		float scale_x = 0.0f, float scale_y = 0.0f, float scale_z = 0.0f);

	PARTICLEDLL_API void pTargetRotate(float rot_x, float rot_y, float rot_z, float scale = 0.0f);

	PARTICLEDLL_API void pTargetRotateD(float scale, PDomainEnum dtype,
		float a0 = 0.0f, float a1 = 0.0f, float a2 = 0.0f,
		float a3 = 0.0f, float a4 = 0.0f, float a5 = 0.0f,
		float a6 = 0.0f, float a7 = 0.0f, float a8 = 0.0f);

	PARTICLEDLL_API void pTargetVelocity(float vel_x, float vel_y, float vel_z, float scale, BOOL allow_translate=TRUE, BOOL allow_rotate=TRUE);

	PARTICLEDLL_API void pTargetVelocityD(float scale, PDomainEnum dtype,
		float a0 = 0.0f, float a1 = 0.0f, float a2 = 0.0f,
		float a3 = 0.0f, float a4 = 0.0f, float a5 = 0.0f,
		float a6 = 0.0f, float a7 = 0.0f, float a8 = 0.0f, BOOL allow_translate=TRUE, BOOL allow_rotate=TRUE);

	PARTICLEDLL_API void pVertex(float x, float y, float z);

	PARTICLEDLL_API void pVortex(float center_x, float center_y, float center_z,
		float axis_x, float axis_y, float axis_z,
		float magnitude = 1.0f, float epsilon = P_EPS,
		float max_radius = P_MAXFLOAT, BOOL allow_translate=TRUE, BOOL allow_rotate=TRUE);
}

#endif //PSystemH