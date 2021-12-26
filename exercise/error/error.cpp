#include"error.h"

#include<unordered_map>

namespace dz
{

	std::unordered_map<ErrorCode, std::string> ErrorInformation =
	{
		{FAIL,"失败"},
		{SUCCESS,"成功"},
	};

	const std::string& GetError(ErrorCode aim)
	{
		return ErrorInformation.find(aim)->second;
	}
}
