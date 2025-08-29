#pragma once

#include <vector>

class Map
{
	enum class TerrainType
	{
		Grass = 0, // 잔디밭
		Road, // 도로
		Swamp, // 늪지대
		Forest, // 숲
		Mountain, // 산
		Size
	};

	// 속도 계수 테이블
	const float speedMultipliers[5] =
	{
		1.0f, // Grass
		1.5f, // Road
		0.2f, // Swamp
		0.8f,  // Forest
		- 1.f  // Mountain
	};

public: // RAII

	Map();
	~Map();

public: // MESSAGE

	// 초기화
	void Initialize();

	// 배경 그리기
	void Draw(class Renderer& renderer);

	// 디버그용 이동 가중치
	void DrawWeight(class Renderer& renderer);

	// 이동 가능 
	bool CanMove(const struct Vector2I& position) const;

public: // GET SET

	const std::vector<std::vector<float>>& GetWeightMap() const { return weight; }
	float GetWeightMap(const struct Vector2I& position) const;

	const std::vector<std::vector<bool>>& GetOccupiedMap() const { return occupied; }
	void SetOccupiedMap(const struct Vector2I& position, bool value);

	int Width() const { return (int)map[0].size(); }
	int Height() const { return (int)map.size(); }

private: // MESSAGE

private: // FILD

	int weightMultiple = 1.0f;
	std::vector<std::vector<TerrainType>> map;
	std::vector<std::vector<float>> weight;
	std::vector<std::vector<bool>> occupied;
};
