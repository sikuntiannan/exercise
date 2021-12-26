#pragma once

#include<string>
#include<typeinfo>
#include<map>
#include<cassert>
#include<type_traits>
#include<utility>

namespace __private
{
	class TypePackDef
	{
	public:
		TypePackDef( ) = default;
		virtual ~TypePackDef( ) = default;
		virtual std::string GetTypeName( ) const noexcept = 0;
		virtual std::type_info GetTypeInfo( ) const noexcept = 0;
		template<typename T>
		T *GetObject( ) const noexcept;
		template<typename T>
		void SetObject(T &&)noexcept;
	private:

	};

	template<typename T>
	T *TypePackDef::GetObject( ) const noexcept
	{
		assert(false);
		return nullptr;
	}

	template<typename T>
	void TypePackDef::SetObject(T&&) noexcept
	{
		assert(false);
		return;
	}

	template<typename T>
	class TypePack:public TypePackDef
	{
	public:
		TypePack( )=delete;
		TypePack(T&& )noexcept;
		TypePack(const TypePack &)noexcept;
		TypePack(TypePack &&)noexcept;
		~TypePack( );
		TypePack &operator=(const TypePack &)noexcept;
		static constexpr std::string &GetTypeName( ) const;
		static constexpr std::type_info &GetTypeInfo( ) const;
		T* GetObject() const noexcept;
		void SetObject(T &&)noexcept;
	private:
		T *m_ObjectPtr;
		static const std::string m_TypeName;
		static const std::type_info m_TypeInfo;
	};

	template<typename T>
	const std::type_info TypePack<T>::m_TypeInfo = typeid(T);

	template<typename T>
	const std::string TypePack<T>::m_TypeName = typeid(T).name();

	template<typename T>
	TypePack<T>::TypePack(T &&aim)noexcept
	{
		if constexpr (std::is_pointer<T>::value)
		{
			m_ObjectPtr = aim;
		}
		else
		{
			m_ObjectPtr = &aim;
		}
	}

	template<typename T>
	TypePack<T>::TypePack(const TypePack<T> & aim)noexcept
	{
		m_ObjectPtr = aim.m_ObjectPtr;
	}

	template<typename T>
	TypePack<T>::TypePack(TypePack<T> && aim)noexcept
	{
		std::swap(m_ObjectPtr , aim.m_ObjectPtr);
	}

	template<typename T>
	TypePack<T>::~TypePack( )
	{
	}

	template<typename T>
	TypePack<T> &TypePack<T>::operator=(const TypePack<T> &aim)noexcept
	{
		m_ObjectPtr = aim.m_ObjectPtr;
		return *this;
	}

	template<typename T>
	constexpr std::string &TypePack<T>::GetTypeName( )
	{
		return m_TypeName;
	}

	template<typename T>
	constexpr std::type_info& TypePack<T>::GetTypeInfo( )
	{
		return m_TypeInfo;
	}

	template<typename T>
	T *TypePack<T>::GetObject( ) const noexcept
	{
		return m_ObjectPtr;
	}

	template<typename T>
	void TypePack<T>::SetObject(T &&aim)noexcept 
	{
		if constexpr (std::is_pointer<T>::value)
		{
			m_ObjectPtr = aim;
		}
		else
		{
			m_ObjectPtr = &aim;
		}
	}
}

namespace dz
{
	class ObjectManager
	{
	public:
		static ObjectManager &GetObjectManager( );
		template<typename T>
		void ObjectRegister(T && );
		template<typename T>
		void ObjectUnregister(T &&);
		template<typename T>
		T *GetObject( )const noexcept;
		~ObjectManager( ) = default;
	private:
		ObjectManager( );
		static ObjectManager m_self;
		std::map<std::string , __private::TypePackDef> m_data;
	};
	template<typename T>
	void ObjectManager::ObjectRegister(T &&aim)
	{
		m_data.insert(std::pair<std::string , __private::TypePackDef>(__private::TypePack<T>::GetTypeName( ) , __private::TypePack<T>(aim)));
	}

	template<typename T>
	void ObjectManager::ObjectUnregister(T &&aim)
	{
		m_data.erase(__private::TypePack<T>::GetTypeName( ));
	}

	template<typename T>
	T *ObjectManager::GetObject( )const noexcept
	{
		std::map<std::string , __private::TypePackDef *>::const_iterator index = m_data.find(__private::TypePack<T>::GetTypeName( ));
		if (index != std::map<std::string , __private::TypePackDef *>::cend())
		{
			return index->second.GetObject();
		}
		else
		{
			return nullptr;
		}
	}
}