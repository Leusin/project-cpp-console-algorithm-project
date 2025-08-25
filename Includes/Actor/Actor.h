#pragma once

#include "Core.h"
#include "RTTI.h"
#include "Math/Vector2I.h"
#include "Math/Color.h"

/// <summary>
/// "물체가 뭘해야할까?" 를 정의한다
/// - 위치 점령
/// - 콘솔 창에 그리기(어떻케)
/// - 엔진의 이벤트 함수 호출: BeginPlay / Tick / Draw
/// </summary>
class Engine_API Actor : public RTTI
{
	RTTI_DECLARATIONS(Actor, RTTI)

public:
	Actor(const char* image = "", Color color = Color::White, const Vector2I& position = Vector2I::Zero);
	virtual ~Actor();

	virtual void BeginPlay(); // 단 한번만 호출.
	virtual void Tick(float deltaTime); // 매 프레임 호출. (반복성 작업/지속성이 필요한 작업)
	virtual void Render(); // 그리기
	
	virtual void OnDestroy(); // 객체가 삭제되기 직전에 호출되는 함수.
	
	bool TestIntersect(const Actor* const other);

	void Destroy(); // 삭제 요청
	void QuitGame();

/// <summary>
/// Getter와 Setter 함수
/// </summary>
public:
	Vector2I Position() const;
	void SetPosition(const Vector2I& newPosition);

	unsigned int GetSortingOrder() const;
	void SetSortingOrder(unsigned int sortingOrder);

	class Level* GetOwner() const;
	void SetOwner(class Level* owner);

	void SetImage(const char* newImage);

	/// <summary>
	/// 수명 주기 설정 함수.
	/// 0.0f 이상 값으로 한 번이라도 호출되지 않으면 자동 삭제되지 않는다.
	/// </summary>
	void SetLifetime(float newLifetime);

	inline int Width() const { return width; };
	inline bool HasBegonPlay() const { return hasBegonPlay; }
	inline bool IsActive() const { return isActive; };
	inline bool IsExpired() const { return isExpired; };
	inline bool AutoDestory() const { return autoDestroy; };
	inline char* GetImage() const { return image; };
	inline Color GetColor() const { return color; }

protected:
	Color color = Color::White;// = Color::Blue | Color::Green | Color::Red;

private:
	Vector2I position; // 개체의 위치

	char* image = nullptr;

	int width = 0; // 문자열 길이

	bool hasBegonPlay = false; // 호출이 되었는지 확인

	unsigned int sortingOrder = 0; // 정렬 순서

	bool isActive = true;// 액터가 활성 상태인지

	bool isExpired = false;	// 삭제 요청됐는지 

	bool autoDestroy = false; // 이 플래그를 true로 설정하면 수명 주기를 사용해 자동 제거.

	float lifetime = 0.0f; // 수명 주기 (단위: 초).

	class Level* owner = nullptr; // 소유 레벨
};
