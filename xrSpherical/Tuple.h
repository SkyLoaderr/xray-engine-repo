#ifndef TUPLE_H
#define TUPLE_H

namespace MiniBall
{
	template<unsigned int n, class Type = float>
	class Tuple
	{
	public:
		Type element[n];

	public:
		Tuple();
		Tuple(Type e[n]);

		Tuple &operator=(const Tuple &T);

		float &operator[](int i);
		float &operator()(int i);

		const float &operator[](int i) const;
		const float &operator()(int i) const;
	};

	template<unsigned int n, class Type>
	inline Tuple<n, Type>::Tuple()
	{
	}

	template<unsigned int n, class Type>
	inline Tuple<n, Type>::Tuple(Type e[n])
	{
		for(int i = 0; i < n; i++)
			element[i] = e[i];
	}

	template<unsigned int n, class Type>
	inline Tuple<n, Type> &Tuple<n, Type>::operator=(const Tuple<n, Type> &T)
	{
		for(int i = 0; i < n; i++)
			element[i] = T.element[i];

		return *this;
	}

	template<unsigned int n, class Type>
	inline float &Tuple<n, Type>::operator[](int i)
	{
		return element[i];
	}

	template<unsigned int n, class Type>
	inline float &Tuple<n, Type>::operator()(int i)
	{
		return element[(i - 1) & 3];	
	}

	template<unsigned int n, class Type>
	inline const float &Tuple<n, Type>::operator[](int i) const
	{
		return element[i];
	}

	template<unsigned int n, class Type>
	inline const float &Tuple<n, Type>::operator()(int i) const
	{
		return element[(i - 1) & 3];	
	}
}

#endif   // TUPLE_H