#include "stdafx.h"
#pragma hdrstop

/*
#include "xr_resource.h"

// res
class	test_resource	: public xr_resorce	{
public:
	void				_release_	(test_resource * ptr)	{	xr_delete	(ptr);	}
};

// ptr
typedef	xr_resorce_ptr<test_resource>	test_ptr;

// the test itself
BOOL	AAA_test		()
{
	test_ptr	A		(xr_new<test_resource>());
	test_ptr	B		= xr_new<test_resource>	();
	A					= B;
	return		A!=B;
}
*/

void	CConstant::Load	(IReader* fs)
{
	dwMode		= modeWaveForm;
	fs->r		(&_R,sizeof(WaveForm));
	fs->r		(&_G,sizeof(WaveForm));
	fs->r		(&_B,sizeof(WaveForm));
	fs->r		(&_A,sizeof(WaveForm));
}

void	CConstant::Save	(IWriter* fs)
{
	fs->w		(&_R,sizeof(WaveForm));
	fs->w		(&_G,sizeof(WaveForm));
	fs->w		(&_B,sizeof(WaveForm));
	fs->w		(&_A,sizeof(WaveForm));
}

