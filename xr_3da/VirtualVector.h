#pragma once

// Desc: acts like an vector, but it is read-only version
//       which is initialized from virtual page-file

template <class T>
class VirtualVector
{
private:
	T*		data;
	int		count;

public:
	void	attach(void* _data, int _size)
	{
		data  = (T*) _data;
		count = _size / sizeof(T);
		VERIFY(_size % sizeof(T) == 0);
	}

	IC T	&operator[]	( int i )	{ VERIFY(i>=0 && i<count); return data[i]; };
	IC T*	begin() { return data; }
	IC T*	end()	{ return data+count; }
	IC int	size()	{ return count; }
};
