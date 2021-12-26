﻿#pragma once
#include<def.h>
#include<atomic>
#include<compare>

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
		delete m_data.load( );
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
			delete NodePtr;
		}
	}

	template<typename T>
	shared_ptr<T> weak_ptr<T>::Lock( )const noexcept
	{
		__private::Node<T> *NodePtr = m_NodePtr.load( );
		size_t Uses = NodePtr->SharedAdd( );
		if (!Uses)
		{
			return shared_ptr<T>( );
		}
		else
		{
			return shared_ptr<T>(NodePtr);
		}
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
		T &operator*( )const noexcept;
		T *operator->( )const noexcept;
		shared_ptr<T> exchange(const shared_ptr &)noexcept;
		bool compare_exchange(const shared_ptr & , const shared_ptr &);
		void swap(shared_ptr<T> &aim)noexcept;
	public:
		template<typename T , typename ...Types>
		friend shared_ptr<T> make_shared(Types ...args);
	private:
		shared_ptr(T *);
		std::atomic < __private::Node<T> *> m_node;
	};
	template<typename T>
	shared_ptr<T>::shared_ptr( )
		:m_node(nullptr)
	{

	}

	template<typename T>
	shared_ptr<T>::shared_ptr(__private::Node<T> * ptr)
		:m_node(ptr)
	{

	}

	template<typename T>
	shared_ptr<T>::shared_ptr( )
		:m_node(nullptr)
	{
	}

	template<typename T>
	shared_ptr<T>::shared_ptr(const shared_ptr<T> &aim)
		:m_node(aim.m_node)
	{
		m_node.load( )->add( );
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
		if (!m_node.load( ))
		{
			__private::Node<T> *NodePtr = m_node.load( );
			size_t OldPtrCounter = NodePtr->sub( );
			if ((OldPtrCounter == 1) && (NodePtr->GetWeak()==0))
			{
				delete NodePtr;
			}
		}
	}

	template<typename T>
	shared_ptr<T> &shared_ptr<T>::operator=(const shared_ptr<T> &aim) noexcept
	{
		Node *NewValue = aim.m_node.load( );
		Node *OldValue = m_node.load( );
		NewValue->add( );
		size_t OldPtrCounter = OldValue->sub( );
		m_node.store(NewValue);
		if (OldPtrCounter == 1)
		{
			delete OldValue;
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
		Node *NewValue = aim.m_node.load( );
		Node *OldValue = m_node.load( );
		NewValue->add( );
		size_t OldPtrCounter = OldValue->sub( );
		m_node.store(NewValue);
		if (OldPtrCounter == 1)
		{
			delete OldValue;
		}
		return *this;
	}

	template<typename T>
	bool shared_ptr<T>::compare_exchange(const shared_ptr<T> &expected , const shared_ptr<T> &desired)
	{
		if (expected.m_node == m_node)
		{
			Node *NewValue = desired.m_node.load( );
			Node *OldValue = m_node.load( );
			NewValue->add( );
			size_t OldPtrCounter = OldValue->sub( );
			m_node.store(NewValue);
			if (OldPtrCounter == 1)
			{
				delete OldValue;
			}
		}
		else
		{
			return false;
		}
	}

	template<typename T>
	void shared_ptr<T>::swap(shared_ptr<T> &aim) noexcept
	{
		std::swap(m_node , aim.m_node);
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