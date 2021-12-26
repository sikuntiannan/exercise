#pragma once
#include<def.h>
#include<atomic>
#include<compare>

namespace dz
{
	/*
	* @author tiannan
	* @note 参文档
	*/
	template<typename T>
	class shared_ptr
	{
	private:
		class Node
		{
		public:
			Node(T * ptr)noexcept;
			Node(std::atomic<T *> &&ptr)noexcept;
			~Node( );
			Node(Node &&) = delete;
			Node(const Node &) = delete;
			Node &operator=(const Node &) = delete;
			std::weak_ordering operator<=> (const Node &right)const noexcept;
			T &operator*( )const noexcept;
			T *get( )const noexcept;
			size_t add( )noexcept;
			size_t sub( )noexcept;
		private:
			const std::atomic<T *> m_data;
			std::atomic<size_t> m_counter;
		};
	public:
		shared_ptr( );
		shared_ptr(const shared_ptr<T>&);
		shared_ptr(shared_ptr<T> &&);
		~shared_ptr( );
		shared_ptr<T> &operator=(T *)noexcept;
		shared_ptr<T> &operator=(const shared_ptr<T> &)noexcept;
		std::weak_ordering operator<=> (const shared_ptr<T> &right)const noexcept;
		explicit operator bool( )const noexcept;
		T &operator*( )const noexcept;
		T *operator->( )const noexcept;
		T *get( )const noexcept;
		void reset(const shared_ptr<T> &aim = shared_ptr<T>())noexcept;
		void swap(shared_ptr<T> &aim)noexcept;
	public:
		template<typename T , typename ...Types>
		friend shared_ptr<T> make_shared(Types ...args);
	private:
		shared_ptr(T*);
		std::atomic < Node *> m_node;
	};

	//Node

	template<typename T>
	shared_ptr<T>::Node::Node(T *ptr)noexcept
		:m_data(ptr), m_counter(1)
	{

	}

	template<typename T>
	shared_ptr<T>::Node::Node(std::atomic<T *> &&ptr)noexcept
		:m_data(ptr) , m_counter(1)
	{

	}

	template<typename T>
	shared_ptr<T>::Node::~Node( )
	{
		delete m_data.load( );
	}

	template<typename T>
	std::weak_ordering shared_ptr<T>::Node::operator<=>(const shared_ptr<T>::Node &right) const noexcept
	{
		return m_data <=> right.m_data;
	}

	template<typename T>
	T &shared_ptr<T>::Node::operator*( )const noexcept
	{
		return *(m_data.load( ));
	}

	template<typename T>
	T *shared_ptr<T>::Node::get( )const noexcept
	{
		return m_data.load( );
	}

	template<typename T>
	size_t shared_ptr<T>::Node::add( )noexcept
	{
		return m_counter.fetch_add(1);
	}

	template<typename T>
	size_t shared_ptr<T>::Node::sub( )noexcept
	{
		return m_counter.fetch_sub(1);
	}

	//shared_ptr

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
		Node *OldValue = m_node.load( );
		size_t OldPtrCounter = OldValue->sub( );
		if (OldPtrCounter == 1)
		{
			delete OldValue;
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
	T *shared_ptr<T>::get( ) const noexcept
	{
		return m_node.load( ).get();
	}

	template<typename T>
	void shared_ptr<T>::reset(const shared_ptr<T> & aim)noexcept
	{
		if (m_node)
		{
			Node *OldValue = m_node.load( );
			size_t OldPtrCounter = OldValue->sub( );
			if (OldPtrCounter == 1)
			{
				delete OldValue;
			}
		}
		m_node = aim.m_node;
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