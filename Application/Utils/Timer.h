#pragma once

/// <summary>
/// 초 단위로 시간 계산이 필요할 떄 사용
/// </summary>
class Timer
{
public:
	Timer(float targetTime = 0.0f);

	void Tick(float deltaTime); // 초세기 업데이트

	void Reset(); // 타이머 추기화

	bool IsTimeout() const; // 설정한 시간이 경과했는지 확인

	void SetTargetTime(float newTargetTime); // 타이머 시간 변경 함수.

	float GetElapsedTime();

private:
	float elapsedTime = 0.0f;
	float targetTime = 0.0f;
};