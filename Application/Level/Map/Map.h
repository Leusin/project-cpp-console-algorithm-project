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
		0.5f, // Swamp
		0.7f  // Forest
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

public: // GET SET

	const std::vector<std::vector<float>>& GetWeightMap() { return weight; }

private: // FILD

	std::vector<std::vector<TerrainType>> map;
	std::vector<std::vector<float>> weight;
};
