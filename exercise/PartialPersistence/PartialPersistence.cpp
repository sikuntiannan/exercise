#include"PartialPersistence.h"

#include<tuple>

//Node
class Node
{
private:
	Node* m_last;
	std::array<char, BUFF_SIZE> m_buff;
	size_t m_size;//记录已用空间
public:
	Node() = default;
	Node(const Node&) = default;
	Node(Node&&) = default;
	operator = (const Node&) = default;
	operator = (Node&&) = default;
	~Node() = default;

	void write(const char const* data);
	auto read();
	Node* prev();

	bool operator == (const Node&)const;
};

void Node::write(const char const* data)
{
	memcpy(m_buff.data(), data, BUFF_SIZE);
}

auto Node::read()
{
	return m_buff.data();
}

Node* Node::prev()
{
	return m_last;
}

bool Node::operator == (const Node& right)const
{
	bool result = m_size == right.m_size ? memcmp(m_buff.data(), right.m_buff.data(), m_size) : false;
	return result;
}

//VersionNumber
struct VersionNumber
{
	VersionNumber(Node*, size_t);
	VersionNumber() = delete;
	VersionNumber(const VersionNumber&) = default;
	VersionNumber(VersionNumber&&) = default;
	~VersionNumber() = default;
	void initialize();
	operator = (const VersionNumber&) = delete;
	Node* m_aim;//版本的最后一个节点
	size_t m_index;//版号
};

VersionNumber::VersionNumber(Node*, size_t)
{

}

void VersionNumber::initialize()
{
	m_aim = nullptr;
	m_index = 0;
}

//PersistenceData

PersistenceData::PersistenceData()
{

}

PersistenceData::~PersistenceData()
{

}

std::string PersistenceData::GetVersion(const VersionNumber&)
{

}

dz::ErrorCode PersistenceData::AddVersion(const std::string&)
{

}

dz::ErrorCode PersistenceData::DeleteVersion(const VersionNumber&)
{

}

dz::ErrorCode PersistenceData::Save(const std::string&)
{

}

dz::ErrorCode PersistenceData::Load(const std::string&)
{

}

const std::vector<const VersionNumber>& PersistenceData::GetVersionNumber()
{

}




#include<iostream>
#include<array>
#include<vector>


const N_MAX = 10;

void begin()
{
	int n;
	std::vector<int> data;
	std::cin >> n;
	data.resize(n);
	std::array<int, N_MAX + 1> counter;
	size_t result = 0;
	for (size_t i = 0; i < n; i++)
	{
		int var;
		std::cin >> var;
		for (size_t i = 0; i < var; i++)
		{
			result += counter[i] * i;
		}
		counter[var] += 1;
	}
	std::count << result;
}