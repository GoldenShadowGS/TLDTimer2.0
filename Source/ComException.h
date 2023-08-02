#include "PCH.h"

struct ComException
{
	HRESULT result;
	ComException(HRESULT const value) : result(value)
	{}
};

inline void HR(HRESULT const result)
{
	if (S_OK != result)
	{
		throw ComException(result);
	}
}