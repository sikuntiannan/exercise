#include<error.h>
#include"queue.h"
namespace dz
{
	//ID
	std::atomic<size_t> ID::m_GlobalNumber = 0;

	ID::ID( )
		:m_number(m_GlobalNumber.fetch_add(1))
	{
	}

	ID::~ID( )
	{
	}

	size_t ID::getID( )
	{
		return m_number;
	}

	void ID::reset( ) noexcept
	{
		m_number = m_GlobalNumber.fetch_add(1);
	}

	std::weak_ordering ID::operator<=>(const ID &right) const noexcept
	{
		return m_number<=> right.m_number;
	}

	//LumpNode

	/*
	* @brief 队列的大小（包含缓冲空间）
	*/
	const size_t LoopSize = 0x100;

	const size_t LoopSize_ = 0xff;

	template<typename T>
	LumpNode<T>::LumpNode( )
	{
		m_loop.resize(LoopSize);
		m_size = 0;
		m_head = 0;
		m_tail = 0;
		m_next = nullptr;
		m_number = m_GlobalNumber.get();
	}

	template<typename T>
	LumpNode<T>::~LumpNode( )
	{
	}

	template<typename T>
	bool LumpNode<T>::push(T &&aim)
	{
		size_t index = m_tail.fetch_add(1);
		if ((index - m_head.load( )) < LoopSize)
		{
			index &= 0xff;
			m_loop[index] = aim;
			m_size.fetch_add(1);
			return ErrorCode::SUCCESS;
		}
		else
		{
			m_tail.fetch_sub(1);
			return ErrorCode::FAIL;
		}
	}

	template<typename T>
	T LumpNode<T>::pop( )
	{
		auto LodSize = m_size.fetch_sub(1);
		if (LodSize > 0)
		{
			T result;
			size_t index = m_head.fetch_add(1);
			index &= 0xff;
			std::swap(m_loop[index] , result);
			return result;
		}
		else
		{
			m_size.fetch_add(1);
			return T( );
		}
	}

	template<typename T>
	int LumpNode<T>::size( )const noexcept
	{
		return m_size.load( );
	}

	template<typename T>
	shared_ptr<LumpNode<T>> LumpNode<T>::getNext( ) const noexcept
	{
		return m_next;
	}

	template<typename T>
	void LumpNode<T>::setNext(shared_ptr<LumpNode<T>> aim)noexcept
	{
		m_next = aim;
	}

	template<typename T>
	void LumpNode<T>::Reset( ) noexcept
	{
		m_size = 0;
		m_head = 0;
		m_tail = 0;
		m_next = nullptr;
		ID::reset( );
	}

	//queue

	template<typename T>
	queue<T>::queue( )
	{
		m_size = 0;
		m_headWork = make_shared<LumpNode<T>>();
		m_tailWork = m_headWork;
	}

	template<typename T>
	queue<T>::~queue( )
	{
	}

	template<typename T>
	void queue<T>::push(T &&aim)
	{
		bool err;
		err = m_tailWork->push(std::forward<T>(aim));

		m_size.fetch_add(1);
	}

	template<typename T>
	T queue<T>::pop( )
	{
		int size = m_size.fetch_sub(1);
		if (size > 0)
		{
			shared_ptr<LumpNode<T>> head = m_headWork;
			T result = head->pop( ) , DefaultValue;
			if (result == DefaultValue)
			{
				do
				{
					head = head->getNext( );
					result = head->pop( );
				} while (result == DefaultValue);
				if (head->getID( ) > m_headWork->getID( ))
				{
					m_headWork.reset(head);
				}
			}
				return result;
		}
		else
		{
			m_size.fetch_add(1);
			return T( );
		}
	}

	template<typename T>
	int queue<T>::size( )const noexcept
	{
		return m_size;
	}

}
