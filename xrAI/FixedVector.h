#pragma once

template <class T, const int dim>
class svector 
{
private:
	T		array[dim];
	DWORD	count;
public:
	typedef T*	iterator;

	svector() : count(0) 
	{}
	svector(T* p, int c) 
	{ assign(p,c); }

	IC T*		begin()				{ return array;							}
	IC T*		end	 ()				{ return array+count;					}
	IC DWORD	size()				{ return count;							}
	IC void		clear()				{ count=0;								}
	IC void		resize(int c)		{ VERIFY(c<=dim); count=c;				}

	IC void	push_back(T e)			{ VERIFY(count<dim); array[count++]=e;	}
	IC void	pop_back()				{ VERIFY(count); count--;				}

	IC T&	operator[] (DWORD id)	{ VERIFY(id<count); return array[id];	}

	IC T&	front()					{ return array[0];						}
	IC T&	back()					{ return array[count-1];				}
	IC T&	last()					{ VERIFY(count<dim); return array[count];}
	IC void	inc	()					{ count++; }
	IC bool	empty()					{ return 0==count;	}

	IC void erase(DWORD id)			{
		VERIFY(id<count);
		count--;
		for (DWORD i=id; i<count; i++)
			array[i] = array[i+1];
	}
	IC void	erase(T* it)		{ erase(it-begin());	}

	IC void insert(int id, T& V)
	{
		VERIFY(id>=0);
		VERIFY(id<count);
		for (int i=count; i>id; i--)
			array[i] = array[i-1];
		count++;
		array[id] = V;
	}
	IC void	assign(T* p, int c) { VERIFY(c>0 && c<dim); CopyMemory(array,p,c*sizeof(T)); count=c; }
};
