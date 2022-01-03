#pragma once
#include<def.h>
#include<atomic>
#include<compare>
#include<cassert>
/*
* @author tiannan
* @note 参文档
*/

namespace __private
{
	//SharedNode

	template<typename T>
	class Node
	{
	public:
		Node(T *ptr);
		Node(std::atomic<T *> &&ptr)noexcept;
		~Node( );
		Node(Node &&) = delete;
		Node(const Node &) = delete;
		Node &operator=(const Node &) = delete;
		std::weak_ordering operator<=> (const Node &right)const noexcept;
		T &operator*( )const noexcept;
		T *get( )const noexcept;
		size_t SharedAdd( )noexcept;
		size_t SharedSub( )noexcept;
		size_t WeakAdd( )noexcept;
		size_t WeakSub( )noexcept;
		size_t GetShared( )noexcept;
		size_t GetWeak( )noexcept;
		bool CompareExchange(T *, T *)noexcept;
		T *Exchange(T *)noexcept;
	private:
		const std::atomic<T *> m_data;
		std::atomic<size_t> m_SharedCounter;
		std::atomic<size_t> m_WeakCounter;
	};

	template<typename T>
	Node<T>::Node(T *ptr)
		:m_data(ptr) , m_SharedCounter(1), m_WeakCounter(0)
	{
	}

	template<typename T>
	Node<T>::Node(std::atomic<T *> &&ptr)noexcept
		:m_data(ptr) , m_counter(1), m_WeakCounter(0)
	{
	}

	template<typename T>
	Node<T>::~Node( )
	{
		#ifdef _DEBUG
		if (m_data.load( ) == nullptr)
		{
			assert(false);//不应该执行到这里，如果有就是我代码写的有问题。
			delete m_data.load( );
		}
		#endif // _DEBUG
	}

	template<typename T>
	std::weak_ordering Node<T>::operator<=>(const Node &right) const noexcept
	{
		return m_data <=> right.m_data;
	}

	template<typename T>
	T &Node<T>::operator*( )const noexcept
	{
		return *(m_data.load( ));
	}

	template<typename T>
	T *Node<T>::get( )const noexcept
	{
		return m_data.load( );
	}

	template<typename T>
	size_t Node<T>::SharedAdd( )noexcept
	{
		return m_SharedCounter.fetch_add(1);
	}

	template<typename T>
	size_t Node<T>::SharedSub( )noexcept
	{
		return m_SharedCounter.fetch_sub(1);
	}

	template<typename T>
	size_t Node<T>::WeakAdd( )noexcept
	{
		return m_WeakCounter.fetch_add(1);
	}

	template<typename T>
	size_t Node<T>::WeakSub( )noexcept
	{
		return m_WeakCounter.fetch_sub(1);
	}

	template<typename T>
	size_t Node<T>::GetShared( )noexcept
	{
		return m_SharedCounter.load( );
	}

	template<typename T>
	size_t Node<T>::GetWeak( )noexcept
	{
		return m_WeakCounter.load( );
	}

	template<typename T>
	bool Node<T>::CompareExchange(T *expected , T *desired)noexcept
	{
		return m_data.compare_exchange_weak(expected , desired);
	}

	template<typename T>
	T *Node<T>::Exchange(T *aim)noexcept
	{
		return m_data.exchange(aim)
	}

}

namespace dz
{
	template<typename T>
	class shared_ptr;

	//week_ptr
	template<typename T>
	class weak_ptr
	{
	public:
		weak_ptr(__private::Node<T>*);
		~weak_ptr( );
		shared_ptr<T> Lock( )const noexcept;
		bool expired( )const noexcept;
	private:
		std::atomic<__private::Node<T> *> m_NodePtr;
	};

	template<typename T>
	weak_ptr<T>::weak_ptr(__private::Node<T> *ptr)
		:m_NodePtr(ptr)
	{

	}

	template<typename T>
	weak_ptr<T>::~weak_ptr( )
	{
		__private::Node<T> * NodePtr = m_NodePtr.load();
		size_t Uses = NodePtr->WeakSub( );
		if (NodePtr->GetShared() || (Uses == 1))
		{
			NodePtr->GetWeak( ) ? : delete NodePtr;
		}
	}

	template<typename T>
	shared_ptr<T> weak_ptr<T>::Lock( )const noexcept
	{
		__private::Node<T> *NodePtr = m_NodePtr.load( );
		if (NodePtr)
		{
			size_t OldUses = NodePtr->SharedAdd( );
			if (OldUses > 0)
			{
				if (NodePtr->get( ))
				{
					return shared_ptr<T>(NodePtr);
				}
			}
			NodePtr->SharedSub( );
		}
		return shared_ptr<T>( );
	}
	template<typename T>
	bool weak_ptr<T>::expired( )const noexcept
	{
		__private::Node<T> *NodePtr = m_NodePtr.load( );
		return NodePtr->GetShared( );
	}

	//shared_ptr

	template<typename T>
	class shared_ptr
	{	
	public:
		shared_ptr( );
		shared_ptr(__private::Node<T> *);
		shared_ptr(const shared_ptr &);
		shared_ptr(shared_ptr &&);
		~shared_ptr( );
		shared_ptr<T> &operator=(const shared_ptr &)noexcept;
		std::weak_ordering operator<=> (const shared_ptr &right)const noexcept;
		explicit operator bool( )const noexcept;
		operator weak_ptr<T>( )const noexcept;
		T &operator*( )const noexcept;
		T *operator->( )const noexcept;
		shared_ptr<T> exchange(const shared_ptr &)noexcept;
		bool compare_exchange(const shared_ptr & , const shared_ptr &);
		void swap(shared_ptr<T> &aim)noexcept;
	public:
		template<typename T , typename ...Types>
		friend shared_ptr<T> make_shared(Types ...args);
	private:
		void release(__private::Node<T> *NodePtr = nullptr);
		shared_ptr(T *);
		std::atomic < __private::Node<T> *> m_node;
	};

	template<typename T>
	shared_ptr<T>::shared_ptr( )
		:m_node(nullptr)
	{

	}

	template<typename T>
	shared_ptr<T>::shared_ptr(__private::Node<T> *ptr)
		:m_node(ptr)
	{
		if (ptr != nullptr)
		{
			ptr->SharedAdd( );
		}
	}

	template<typename T>
	shared_ptr<T>::shared_ptr(const shared_ptr<T> &aim)
		:m_node(aim.m_node)
	{
		__private::Node<T> *NodePtr = m_node.load( );
		NodePtr->SharedAdd( );
	}

	template<typename T>
	shared_ptr<T>::shared_ptr(shared_ptr<T> &&aim)
		:m_node(nullptr)
	{
		std::swap(m_node , aim.m_node);
	}

	template<typename T>
	shared_ptr<T>::~shared_ptr( )
	{
		release(m_node.load());
	}

	template<typename T>
	shared_ptr<T> &shared_ptr<T>::operator=(const shared_ptr<T> &aim) noexcept
	{
		if (aim != *this)
		{
			__private::Node<T> *NewValue = aim.m_node.load( );
			__private::Node<T> *OldValue = m_node.load( );
			if (NewValue)
			{
				NewValue->SharedAdd( );
			}
			m_node.store(NewValue);
			release(OldValue);
		}
		return *this;
	}

	template<typename T>
	std::weak_ordering shared_ptr<T>::operator<=> (const shared_ptr<T> &right) const noexcept
	{
		return m_node <=> right.m_node;
	}

	template<typename T>
	shared_ptr<T>::operator bool( )const noexcept
	{
		return m_node != nullptr;
	}

	template<typename T>
	shared_ptr<T>::operator weak_ptr<T>( )const noexcept
	{
		return weak_ptr<T>(m_node);
	}

	template<typename T>
	T &shared_ptr<T>::operator*( )const noexcept
	{
		return *(m_node.load( ));
	}

	template<typename T>
	T *shared_ptr<T>::operator->( )const noexcept
	{
		return m_node.load( ).get();
	}

	template<typename T>
	shared_ptr<T> shared_ptr<T>::exchange(const shared_ptr<T> &aim)noexcept
	{
		if (aim != *this)
		{
			__private::Node<T> *NewValue = aim.m_node.load( );
			__private::Node<T> *OldValue = m_node.load( );
			if (NewValue)
			{
				NewValue->SharedAdd( );
			}
			m_node.store(NewValue);
			release(OldValue);
		}
		return *this;
	}

	template<typename T>
	bool shared_ptr<T>::compare_exchange(const shared_ptr<T> &expected , const shared_ptr<T> &desired)
	{
		if (desired != *this)
		{
			if (expected == *this)
			{
				__private::Node<T> *NewValue = desired.m_node.load( );
				__private::Node<T> *OldValue = m_node.load( );
				if (NewValue)
				{
					NewValue->SharedAdd( );
				}
				m_node.store(NewValue);
				release(OldValue);
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return true;
		}
	}

	template<typename T>
	void shared_ptr<T>::swap(shared_ptr<T> &aim) noexcept
	{
		std::swap(m_node , aim.m_node);
	}

	template<typename T>
	void shared_ptr<T>::release(__private::Node<T> *NodePtr)
	{
		if (NodePtr)
		{
			size_t OldPtrCounter = NodePtr->SharedSub( );
			if (OldPtrCounter == 1)
			{
				delete NodePtr->Exchange(nullptr);
				if ((NodePtr->GetWeak( ) == 0))
				{
					delete NodePtr;
				}
			}
		}
	}

	template<typename T>
	shared_ptr<T>::shared_ptr(T *aim)
		:m_node(aim)
	{

	}

	//tool function
	
	template<typename T , typename ...Types>
	shared_ptr<T> make_shared(Types ...args)
	{
		shared_ptr<T> result(new T(args));
		return result;
	}

}