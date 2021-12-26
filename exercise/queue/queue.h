#pragma once
#include<def.h>
#include<auto_ptr.h>

#include<vector>
#include<utility>
namespace dz
{
	/*
	* @brief 唯一ID。继承使用
	*/
	class ID
	{
	public:
		ID( );
		~ID( );
		/*
		* @brief 获取ID
		* @return ID
		*/
		size_t getID( );

		/*
		* @brief 三路比较
		*/
		std::weak_ordering operator<=>(const ID &right)const noexcept;

		/*
		* @brief 重置，编号会自增
		*/
		void reset()noexcept;
	private:
		size_t m_number;
		static std::atomic<size_t> m_GlobalNumber;
	};

	/*
	* @brief 固定大小的队列
	* @note 线程安全
	*/
	template<typename T>
	class LumpNode:public ID
	{
	public:
		LumpNode( );

		/*
		* @warning 不做任何安全保证，自己保证调用时已经没有竞争。
		*/
		~LumpNode( );

		/*
		* @brief 插入元素
		* @param aim 待插入的值
		* @return 成功与否
		*/
		bool push(T &&aim);

		/*
		* @brief 吐出元素
		* @return 吐出的元素
		* @warning 当空时，返回一个默认构造的元素
		*/
		T pop( );

		/*
		* @brief 返回当前队列大小（有效元素数量）
		* @note 返回值可能小于零，但无关紧要
		* @return 有效元素数量
		*/
		int size( )const noexcept;

		/*
		* @brief 返回下一个节点的指针
		* @return 下一个节点的指针
		*/
		shared_ptr<LumpNode<T>> getNext( ) const noexcept;

		/*
		* @brief 返回下一个节点的指针
		* @param aim 下一个节点的指针
		* @return 无
		*/
		void setNext(shared_ptr<LumpNode<T>> aim) noexcept;

		/*
		* @brief 重置对象，也会修改对象的编号
		* @return 无
		*/
		void Reset() noexcept;
	private:
		
		std::vector<T> m_loop;
		std::atomic<int> m_size;
		std::atomic<int> m_head;
		std::atomic<int> m_tail;
		shared_ptr<LumpNode<T>> m_next;
	};


	/*
	* @brief 浮动大小的队列
	* @note 线程安全
	*/
	template<typename T>
	class queue
	{
	public:
		queue( );
		~queue( );

		/*
		* @brief 插入元素
		* @param aim 待插入的值
		* @return 空
		*/
		void push(T &&aim);

		/*
		* @brief 吐出元素
		* @return 吐出的元素
		* @warning 当空时，返回一个默认构造的元素
		*/
		T pop( );

		/*
		* @brief 返回当前队列大小（有效元素数量）
		* @return 有效元素数量
		*/
		int size( )const noexcept;
	private:
		std::atomic<int> m_size;//元素数量
		shared_ptr<LumpNode<T>> m_headWork;//第一个节点
		shared_ptr<LumpNode<T>> m_tailWork;//最后一个节点
	};


}