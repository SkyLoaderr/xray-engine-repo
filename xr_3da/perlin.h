#ifndef perlinH
#define perlinH

#define SAMPLE_SIZE 1024

class CPerlinNoise
{
private:
	void	init_perlin		(int n,float p);

	float	noise1			(float arg);
	float	noise2			(const Fvector2& vec);
	float	noise3			(const Fvector3& vec);
	void	normalize2		(float v[2]);
	void	normalize3		(float v[3]);
	void	init			(void);

	int		mOctaves;
	float	mFrequency;
	float	mAmplitude;
	int		mSeed;

	int		p	[SAMPLE_SIZE + SAMPLE_SIZE + 2];
	float	g3	[SAMPLE_SIZE + SAMPLE_SIZE + 2][3];
	float	g2	[SAMPLE_SIZE + SAMPLE_SIZE + 2][2];
	float	g1	[SAMPLE_SIZE + SAMPLE_SIZE + 2];
	bool	mStart;
public:
			CPerlinNoise	(int octaves,float freq,float amp,int seed);
	float	Get				(float x);
	float	Get				(float x, float y);
};

#endif

