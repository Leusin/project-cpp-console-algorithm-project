#include "Map.h"

#include "Engine.h"
#include "Math/Vector2I.h"
#include "Render/Renderer.h"

Map::Map()
	: map(Engine::Height(), std::vector<TerrainType>(Engine::Width(), TerrainType::Grass))
	, weight(Engine::Height(), std::vector<float>(Engine::Width(), speedMultipliers[0]))
{
}

Map::~Map()
{
}

void Map::Initialize()
{
	/*
	*/
	std::vector<std::vector<int>> data =
	{
		{1, 1, 1, 1, 1, 1, 1, 1 },
		{1, 4, 4, 0, 0, 4, 4, 1},
		{1, 4, 3, 3, 3, 3, 4, 1},
		{1, 0, 3, 2, 2, 3, 0, 1},
		{1, 0, 3, 2, 2, 3, 0, 1},
		{1, 4, 3, 3, 3, 3, 4, 1},
		{1, 4, 4, 0, 0, 4, 4, 1},
		{1, 1, 1, 1, 1, 1, 1, 1}
	};

	/*
	std::vector<std::vector<int>> data =
	{
		{4, 4, 4, 4, 4, 4, 4, 4},
		{4, 3, 3, 0, 0, 3, 3, 4},
		{4, 3, 2, 2, 2, 2, 3, 4},
		{4, 0, 2, 1, 1, 2, 0, 4},
		{4, 0, 2, 1, 1, 2, 0, 4},
		{4, 3, 2, 2, 2, 2, 3, 4},
		{4, 3, 3, 0, 0, 3, 3, 4},
		{4, 4, 4, 4, 4, 4, 4, 4}
	};
	*/

	/*std::vector<std::vector<int>> data =
	{
		{0, 1, 2},
		{3, 4, 5}
	};*/

	/*
	std::vector<std::vector<int>> data =
	{
		{0, 1},
		{3, 4}
	};
	*/

	if (data.empty())
	{
		return;
	}

	int dataWidth = (int)data[0].size();
	int dataHeight = (int)data.size();

	int mapWidth = (int)map[0].size();
	int mapHeight = (int)map.size();

	// 홀수값 오차를 위해 1 더한다
	int offsetX = (mapWidth + 1) / dataWidth;
	int offsetY = (mapHeight + 1) / dataHeight;

	for (int y = 0; y < mapHeight; ++y)
	{
		for (int x = 0; x < mapWidth; ++x)
		{
			int dataX = y / offsetY;
			int dataY = x / offsetX;

			// 인덱스 범위로 검사 및 클리핑
			dataX = (dataX >= dataWidth) ? dataWidth - 1 : dataX;
			dataY = (dataY >= dataHeight) ? dataHeight - 1 : dataY;

			int terrainIndex = data[dataX][dataY];
			terrainIndex %= (int)TerrainType::Size;

			map[y][x] = static_cast<TerrainType>(terrainIndex);
			weight[y][x] = speedMultipliers[terrainIndex];
		}
	}
}

void Map::Draw(Renderer& renderer)
{
	for (int y = 0; y < (int)map.size(); ++y)
	{
		for (int x = 0; x < (int)map[y].size(); ++x)
		{
			TerrainType type = map[y][x];

			switch (type) {
			case TerrainType::Grass:
				renderer.WriteToBuffer({ x, y }, " ", Color::Intensity);
				break;
			case TerrainType::Road:
				renderer.WriteToBuffer({ x, y }, "#", Color::Intensity);
				break;
			case TerrainType::Swamp:
				renderer.WriteToBuffer({ x, y }, "~", Color::Intensity);
				break;
			case TerrainType::Forest:
				renderer.WriteToBuffer({ x, y }, "=", Color::Intensity);
				break;
			case TerrainType::Mountain:
				renderer.WriteToBuffer({ x, y }, "^", Color::Intensity);
				break;
			default:
				// 기본값 또는 에러 처리
				break;
			}
		}
	}
}

void Map::DrawWeight(Renderer& renderer)
{
	for (int y = 0; y < Height(); ++y)
	{
		for (int x = 0; x <Width(); ++x)
		{
			char buffer[4];

			// 이동 가능 여부
			bool canMove = CanMove({ x, y });

			// 이동 가능 여부에 따른 색깔
			Color color = canMove ? Color::LightBlue : Color::LightRed;

			int data = (int)GetWeightMap()[y][x];
			sprintf_s(buffer, sizeof(buffer), "%d", data);
			renderer.WriteToBuffer({ x, y }, buffer, color, 2);
		}
	}
}

bool Map::CanMove(const Vector2I& position) const
{
	if (position.x < 0 || position.y < 0 ||
		position.x >= Width() || position.y >= Height())
	{
		return false;
	}

	return !(weight[position.y][position.x] < 0.0f);
}
