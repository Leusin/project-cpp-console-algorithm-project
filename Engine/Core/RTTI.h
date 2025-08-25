#pragma once

#include "Core.h"


/// <summary>
/// RTTI: Run-Time Type Information
/// 실행 중에 클래스 간의 타입을 빠르게 비교하고 형변환하기 위한 인터페이스
/// </summary>
class Engine_API RTTI
{
public:
	// 현재 객체의 고유 타입 ID를 반환 (순수 가상 함수)
	// 내부에 static int 값을 둬서 주소를 담음
	virtual const size_t& TypeIdInstance() const = 0;
	// → 각 클래스에서 아래처럼 구현됨:
	/*
	* return Type::TypeIdClass();
	*/

	// 해당 ID가 자신의 타입이거나 부모 타입이면 true 반환
	// 기본 구현은 false이며, 자식에서 override해야 함
	virtual bool Is(const size_t id) const
	{
		return false;
	}
	// → 각 클래스에서 아래처럼 override함:
	/*
	* if (id == TypeIdClass())
	* {
	* 	return true;
	* }
	* else
	* {
	* 	return ParentType::Is(id);
	* }
	*/

	// 현재 객체를 타입 T로 변환 시도 
	// 성공 시 포인터 반환, 실패 시 nullptr
	template<typename T>
	T* As()
	{
		if (Is(T::TypeIdClass()))
		{
			return (T*)this;
		}

		return nullptr;
	}

	// const 버전 (오버로드)
	template<typename T>
	const T* As() const
	{
		if (Is(T::TypeIdClass()))
		{
			return (T*)this;
		}

		return nullptr;
	}

	//
	// 아래 함수들은 매크로를 통해 각 클래스별로 생성됨:
	//

	// 이 타입이 상속한 부모 타입을 super로 지정해놓음. 상속 트래킹용
	/*
	* using super = ParentType;
	*/

	// 현재 타입 고유 ID 반환 (클래스의 주소를 가진 정적 멤버). 
	// 같은 클래스는 항상 같은 주소를 반환함
	/*
	* static const size_t TypeIdClass()
	* {
	*   static int runTimeTypeId = 0;
	*   return reinterpret_cast<size_t>(&runTimeTypeId);
	* }
	*/

	// 현재 객체가 id와 같은 타입인지 체크하는 재귀 함수.
	// 적 배열이나 해시테이블을 사용해 캐싱을 적용하면 성능을 더 개선시킬 수 있음.
	/*
	* virtual bool Is(const size_t id) const override
	* {
	*	if (id == TypeIdClass())
	*	{
	*		return true;
	*	}
	*   else
	*	{
	*		 return ParentType::Is(id);
	*	}
	* }
	*/
};

// RTTI 클래스를 상속하는 클래스의 구현을 자동화 해주는 매크로
#define RTTI_DECLARATIONS(Type, ParentType)												\
public:																					\
	using super = ParentType;															\
	virtual const size_t& TypeIdInstance() const { return Type::TypeIdClass(); }		\
	static const size_t TypeIdClass()													\
	{																					\
		static int runTimeTypeId = 0;													\
		return reinterpret_cast<size_t>(&runTimeTypeId);								\
	}																					\
	virtual bool Is(const size_t id) const												\
	{																					\
		if (id == TypeIdClass())														\
		{																				\
			return true;																\
		}																				\
		else																			\
		{																				\
			return ParentType::Is(id);													\
		}																				\
	}