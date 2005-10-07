#include "stdafx.h"

// Dima needs this because we have to exclude option /EHsc (exception handling) from the project
namespace boost {
	void  throw_exception(const std::exception &A)
	{
		Debug.fatal("Boost exception raised %s",A.what());
	}
};

