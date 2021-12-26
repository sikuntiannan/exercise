# 理念

因为B先于A获取，只是执行偏后，智能指针只负责指针本身的安全性而不负责持有对象的安全。

当然，如果存心要崩溃，那这个代码是防止不了的，比如获取持有的对象，然后取地址，之后delete，我也没办法。

## 细节

 - 移动构造

   ```c++
   	template<typename T>
   	shared_ptr<T>::shared_ptr(shared_ptr<T> &&aim)
   		:m_node(nullptr)
   	{
   		std::swap(m_node , aim.m_node);
   	}
   ```

   - 这里不会引起释放

   - 存在这一种特殊情况

     | A                               | B                     |
     | ------------------------------- | --------------------- |
     | :m_node(nullptr)                |                       |
     |                                 | 获取指针 auto ptr =*B |
     | std::swap(m_node , aim.m_node); |                       |
     |                                 | ptr ->function()      |

     这是合理的

 - 拷贝构造

   ```c++
   	template<typename T>
   	shared_ptr<T>::shared_ptr(const shared_ptr<T> &aim)
   		:m_node(aim.m_node)
   	{
   		m_node.load( )->add( );
   	}
   ```

    - 这里不会引起释放

    - 一些特殊情况

      | A                       | B              |
      | ----------------------- | -------------- |
      | :m_node(aim.m_node)     |                |
      |                         | delete m_node; |
      | m_node.load( )->add( ); |                |

      这种情况并不会发生，因为这个函数本身就已经持有一个shared_ptr，且m_node并不会泄露，所以不会有释放操作。

   

 - 赋值

 - 其他操作对访问操作

 - 获取操作

 - 重置操作

```c++
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
```





