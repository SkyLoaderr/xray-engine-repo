#ifndef _ENVELOPE_H_
#define _ENVELOPE_H_

// refs
class CFS_Base;
class CStream;

/*
======================================================================
envelope.h

  Definitions for local copies of LightWave envelopes.
  
	Ernie Wright  31 Aug 00
	
	  The LightWave plug-in SDK provides its own representation of LightWave
	  envelopes that plug-ins can use.  These definitions are for standalone
	  programs that, for example, read scene or object files and must store
	  the envelopes.
====================================================================== */

#define SHAPE_TCB   0
#define SHAPE_HERM  1
#define SHAPE_BEZI  2
#define SHAPE_LINE  3
#define SHAPE_STEP  4
#define SHAPE_BEZ2  5

#define BEH_RESET      0
#define BEH_CONSTANT   1
#define BEH_REPEAT     2
#define BEH_OSCILLATE  3
#define BEH_OFFSET     4
#define BEH_LINEAR     5

struct st_Key{
	float	value;
	float	time;
	int		shape;
	float	tension;
	float	continuity;
	float	bias;
	float	param[ 4 ];
	st_Key		(){ZeroMemory(this,sizeof(st_Key));}
};

DEFINE_VECTOR(st_Key*,KeyVec,KeyIt);

class ENGINE_API CEnvelope {
	friend class CBoneData;
	friend class CCustomMotion;
	friend float evalEnvelope(CEnvelope *env, float time);

	KeyVec		keys;
	int			behavior[2];
public:
				CEnvelope	(){behavior[0]=0;behavior[1]=0;}
				CEnvelope	(CEnvelope* source);
	virtual		~CEnvelope	();

	float		Evaluate	(float t);

	void		Save		(CFS_Base& F);
	void		Load		(CStream& F);
};

#endif //_ENVELOPE_H_