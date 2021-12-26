#include"delivery.h"
namespace __private
{

}

namespace dz
{
	ObjectManager ObjectManager::m_self;

	ObjectManager &ObjectManager::GetObjectManager( )
	{
		return m_self;
	}
}