#pragma
/*
��ȡ�汾����Ӱ汾��ɾ���汾�����浽�ļ������ļ����أ���ȡ���а�ţ�
���ֳ־û�����ǰ��Ҫɾ�󣬿��Բ鿴���а汾����ֻ���޸����°�
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
* �ĵ�
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
	* ����ĳ���汾������
	*/
	std::string GetVersion(const VersionNumber &);
	/*
	* ���һ���°汾
	*/
	dz::ErrorCode AddVersion(const std::string &);
	/*
	* ɾ��һ���汾
	*/
	dz::ErrorCode DeleteVersion(const VersionNumber &);
	/*
	* ����ṹ���ļ�
	*/
	dz::ErrorCode Save(const std::string&);
	/*
	* ���ļ����ؽṹ
	*/
	dz::ErrorCode Load(const std::string&);
	/*
	* ��ȡ���а汾�ڵ�
	*/
	const std::vector<const VersionNumber>& GetVersionNumber();

private:
	std::vector<const VersionNumber> m_version;//���а汾
	VersionNumber m_newest;//ָ�����°�
	std::vector<const Node const*> m_nodeList;//��¼���нڵ�
};
