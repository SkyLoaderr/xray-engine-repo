#pragma once

namespace boost {
	template <typename T>
	class function;
};

struct slipch_holder {
	typedef boost::function<void> FUNCTION;
	xr_vector<FUNCTION>	*m_storage;
};

