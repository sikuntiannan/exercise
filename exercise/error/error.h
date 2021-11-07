#pragma once
#include<string>
#include<def.h>
namespace dz
{
	enum ErrorCode
	{
		FAIL,
		SUCCESS,
	};

	DLL_EXPORT const std::string& GetError(ErrorCode aim);
}
