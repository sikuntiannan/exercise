#pragma
/*
获取版本，添加版本，删除版本，保存到文件，从文件加载，获取所有版号；
部分持久化，改前需要删后，可以查看所有版本，但只能修改最新版
*/

#include<array>
#include<pair>
#include<vector>

#include"../def/def.h"
#include"../error/error.h"

#ifdef TEST
const size_t BUFF_SIZE = 16;
#else
const size_t BUFF_SIZE = 256;
#endif // TEST

/*
* 文档
*/

//Node
class Node;

//VersionNumber
struct VersionNumber;

//PersistenceData
class DLL_EXPORT PersistenceData
{
public:
	PersistenceData();
	~PersistenceData();
	/*
	* 请求某个版本的内容
	*/
	std::string GetVersion(const VersionNumber &);
	/*
	* 添加一个新版本
	*/
	dz::ErrorCode AddVersion(const std::string &);
	/*
	* 删除一个版本
	*/
	dz::ErrorCode DeleteVersion(const VersionNumber &);
	/*
	* 保存结构到文件
	*/
	dz::ErrorCode Save(const std::string&);
	/*
	* 从文件加载结构
	*/
	dz::ErrorCode Load(const std::string&);
	/*
	* 获取所有版本节点
	*/
	const std::vector<const VersionNumber>& GetVersionNumber();

private:
	std::vector<const VersionNumber> m_version;//所有版本
	VersionNumber m_newest;//指向最新版
	std::vector<const Node const*> m_nodeList;//记录所有节点
};
