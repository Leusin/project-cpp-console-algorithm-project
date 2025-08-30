#pragma once

#include <vector>

class Map
{
	enum class TerrainType
	{
		Ground = 0, // 잔디밭
		Road = 1, // 도로
		Swamp = 2, // 늪지대
		Pit = 3, // 구멍(이동 못함)
		Territory = 4, // 땅따먹기
		SpawnPool = 5, // 스폰하는 곳
		Size
	};

	// 속도 계수 테이블
	const float speedMultipliers[6] =
	{
		1.0f, // Grass
		1.5f, // Road
		0.2f, // Swamp
		-1.f,  // Pit
		1.0f,  // Territory
		1.0f  // Pool
	};

public: // RAII

	Map();
	~Map();

public: // MESSAGE

	// 초기화
	void Initialize();
	bool CreateTerritory(class QuadTree& qTree, std::vector<class Territory*>& territorys);
	bool CreateSpawnPool(class QuadTree& qTree, const class UnitFactory& factory, std::vector<class SpawnPool*>& SpawnPools);

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

	const std::vector<std::vector<int>>& GetOriginalData() const { return data; }

private: // MESSAGE

private: // FILD

	float weightMultiple = 1.0f;
	std::vector<std::vector<TerrainType>> map;
	std::vector<std::vector<float>> weight;
	std::vector<std::vector<bool>> occupied;

	std::vector<std::vector<int>> data;
};
