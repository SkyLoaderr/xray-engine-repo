//---------------------------------------------------------------------------
#ifndef particle_coreH
#define particle_coreH

	#pragma pack (push,4)
	class pVector	: public Fvector
	{
	public:
		IC 			pVector		(float ax, float ay, float az)	{ set(ax,ay,az);								}
		IC 			pVector		()								{}
		IC float 	length		() const						{	return _sqrt(x*x+y*y+z*z);					}
		IC float 	length2		() const						{	return (x*x+y*y+z*z);						}
		IC float 	operator*	(const pVector &a) const		{	return x*a.x + y*a.y + z*a.z;				}
		IC pVector 	operator*	(const float s) const			{	return pVector(x*s, y*s, z*s);				}
		IC pVector 	operator/	(const float s) const			{	float invs = 1.0f / s;	return pVector(x*invs, y*invs, z*invs);	}
		IC pVector 	operator+	(const pVector& a) const		{	return pVector(x+a.x, y+a.y, z+a.z);		}
		IC pVector 	operator-	(const pVector& a) const		{	return pVector(x-a.x, y-a.y, z-a.z);		}
		IC pVector 	operator-	()								{	x = -x;	y = -y;	z = -z;	return *this;		}
		IC pVector& operator+=	(const pVector& a)				{	x += a.x;y += a.y;z += a.z;	return *this;	}
		IC pVector& operator-=	(const pVector& a)		 		{	x -= a.x;y -= a.y;z -= a.z;	return *this;	}
		IC pVector& operator*=	(const float a)					{	x *= a;	y *= a;	z *= a;	return *this;		}
		IC pVector& operator/=	(const float a)					{	float b = 1.0f / a;	x *= b;	y *= b;	z *= b;	return *this;		}
		IC pVector& operator=	(const pVector& a)				{	x = a.x;y = a.y;z = a.z;return *this;		}
		IC pVector 	operator^	(const pVector& b) const		{	return pVector(y*b.z-z*b.y,z*b.x-x*b.z,x*b.y-y*b.x);		}
	};
	// A single particle
	struct Particle
	{
		enum{
			BIRTH		= (1<<0),
			DYING		= (1<<1),
			ANIMATE_CCW	= (1<<2),
		};
		pVector		pos;	// 12
		pVector		posB;   // 12
		pVector		vel;    // 12  	
		pVector		size;   // 12
		pVector		rot;	// 12   60
		u32			color;	// 4
		float		age;	// 4       
		u16			frame;	// 2
		Flags16		flags;	// 2
	};                  	// 		72
	// A effect of particles - Info and an array of Particles
	struct PARTICLES_API ParticleEffect
	{
		int			p_count;				// Number of particles currently existing.
		int			max_particles;			// Max particles allowed in effect.
		int			particles_allocated;	// Actual allocated size.
		Particle*	particles;				// Actually, num_particles in size

					ParticleEffect	(int mp)
		{
			p_count					= 0;
			max_particles			= mp;
			particles_allocated		= max_particles;
			particles				= xr_alloc<Particle>(max_particles);
		}
					~ParticleEffect	()
		{
			xr_free					(particles);
		}
		IC int		Resize			(int max_count)
		{
			// Reducing max.
			if(particles_allocated >= max_count)
			{
				max_particles		= max_count;

				// May have to kill particles.
				if(p_count > max_particles)
					p_count			= max_particles;

				return max_count;
			}

			// Allocate particles.
			Particle* new_particles	= xr_alloc<Particle>(max_count);
			if(new_particles==NULL){
				// ERROR - Not enough memory. Just give all we've got.
				max_particles		= particles_allocated;
				return max_particles;
			}

			Memory.mem_copy			(new_particles, particles, p_count * sizeof(Particle));
			xr_free					(particles);
			particles				= new_particles;

			max_particles			= max_count;
			particles_allocated		= max_count;
			return max_count;
		}
		IC void		Remove			(int i)
		{
			Particle& m				= particles[i];
			if (m.flags.is(Particle::DYING))m 			= particles[--p_count];
			else							m.flags.set	(Particle::DYING,TRUE);
		}

		IC BOOL		Add				(const pVector &pos, const pVector &posB,
									const pVector &size, const pVector &rot, const pVector &vel, u32 color,
									const float age = 0.0f, u16 frame=0, u16 flags=0)
		{
			if(p_count >= max_particles)	return FALSE;
			else{
				Particle& P = particles[p_count];
				P.pos 		= pos;
				P.posB 		= posB;
				P.size 		= size;
				P.rot 		= rot;
				P.vel 		= vel;
				P.color 	= color;
				P.age 		= age;
				P.frame 	= frame;
				P.flags.set	(flags); 
                P.flags.set	(Particle::BIRTH,TRUE);
				p_count++;
				return TRUE;
			}
		}
	};
	//////////////////////////////////////////////////////////////////////
	// Type codes for domains
	PARTICLES_API enum PDomainEnum
	{
		PDPoint 	= 0,	// Single point
		PDLine 		= 1,	// Line segment
		PDTriangle 	= 2,	// Triangle
		PDPlane 	= 3,	// Arbitrarily-oriented plane
		PDBox 		= 4,	// Axis-aligned box
		PDSphere 	= 5,	// Sphere
		PDCylinder 	= 6,	// Cylinder
		PDCone 		= 7,	// Cone
		PDBlob 		= 8,	// Gaussian blob
		PDDisc 		= 9,	// Arbitrarily-oriented disc
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
	#pragma pack (pop)

    class ParticleActions;

//---------------------------------------------------------------------------
#endif
 