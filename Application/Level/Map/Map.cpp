#include "Map.h"

#include "Engine.h"
#include "Math/Vector2I.h"
#include "Render/Renderer.h"
#include "QuadTree/QuadTree.h"
#include "Actor/AUnit/UnitFactory.h"
#include "Actor/Territory/Territory.h"
#include "Actor/Territory/SpawnPool.h"


Map::Map()
	: map(Engine::Height(), std::vector<TerrainType>(Engine::Width(), TerrainType::Ground))
	, weight(Engine::Height(), std::vector<float>(Engine::Width(), speedMultipliers[0]))
	, occupied(Engine::Height(), std::vector<bool>(Engine::Width(), false))
{
}

Map::~Map()
{
}

void Map::Initialize()
{
	/*
	*/
	data =
	{
		{5, 1, 1, 1, 1, 1, 1, 5},
		{1, 3, 3, 0, 0, 3, 3, 1},
		{1, 3, 4, 4, 4, 4, 3, 1},
		{1, 0, 4, 2, 2, 4, 0, 1},
		{1, 0, 4, 2, 2, 4, 0, 1},
		{1, 3, 4, 4, 4, 4, 3, 1},
		{1, 3, 3, 0, 0, 3, 3, 1},
		{5, 1, 1, 1, 1, 1, 1, 1}
	};

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

			TerrainType type = static_cast<TerrainType>(terrainIndex);
			map[y][x] = type;
			weight[y][x] = speedMultipliers[terrainIndex];
		}
	}
}

bool Map::CreateTerritory(QuadTree& qTree, const UnitFactory& factory, std::vector<Territory*>& territorys)
{
	if (data.empty())
	{
		return false;
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

			TerrainType type = static_cast<TerrainType>(terrainIndex);

			//
			// 땅따먹기 영역 생성
			//

			if (type != TerrainType::Territory)
			{
				continue;
			}

			int key = dataY * dataHeight + dataX;

			// 같은 아이디를 가진 지형 찾기
			bool find = false;
			for (Territory* terr: territorys)
			{
				if (terr->GetId() == key)
				{
					// 지형 너비 업데이트 하기
					Bounds bounds = terr->GetBounds();
					bounds.SetSize({ x - bounds.GetX(), y - bounds.GetY() });
					terr->SetBounds(bounds);

					find = true;
					break;
				}
			}
			// 새로운 지형 만들기
			if (!find)
			{
				Territory* terr = new Territory(key, Vector2I{ x, y }, Vector2I::Zero, qTree, &factory.none);
				territorys.emplace_back(terr);
			}
		}
	}

	return true;
}

bool Map::CreateSpawnPool(QuadTree& qTree, const UnitFactory& factory, std::vector<SpawnPool*>& SpawnPools)
{
	if (data.empty())
	{
		return false;
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

			TerrainType type = static_cast<TerrainType>(terrainIndex);

			//
			// 영역 생성
			//

			if (type != TerrainType::SpawnPool)
			{
				continue;
			}

			int key = dataY * dataHeight + dataX;

			// 같은 아이디를 가진 지형 찾기
			bool find = false;
			for (SpawnPool* terr : SpawnPools)
			{
				if (terr->GetId() == key)
				{
					// 지형 너비 업데이트 하기
					Bounds bounds = terr->GetBounds();
					bounds.SetSize({ x - bounds.GetX(), y - bounds.GetY() });
					terr->SetBounds(bounds);

					find = true;
					break;
				}
			}

			// 새로운 지형 만들기
			if (!find)
			{
				SpawnPool* terr = new SpawnPool(key, Vector2I{ x, y }, Vector2I::Zero, qTree, *this, factory);
				SpawnPools.emplace_back(terr);
			}
		}
	}

	return true;
}

void Map::Draw(Renderer& renderer)
{
	for (int y = 0; y < (int)map.size(); ++y)
	{
		for (int x = 0; x < (int)map[y].size(); ++x)
		{
			TerrainType type = map[y][x];

			switch (type) {
			case TerrainType::Ground:
				{

					if (x % 2 == 0)
					{
						renderer.WriteToBuffer({ x, y }, ".", Color::Intensity);
					}
					else
					{
						renderer.WriteToBuffer({ x, y }, "`", Color::Intensity);
					}

				}
				break;
			case TerrainType::Road:
			{
				if (x % 2 == 0)
				{
					renderer.WriteToBuffer({ x, y }, "=", Color::Intensity);
				}
				else
				{
					renderer.WriteToBuffer({ x, y }, "-", Color::Intensity);
				}
			}
			break;
			case TerrainType::Swamp:
			{
				if ((x + y) % 2 == 0)
				{
					renderer.WriteToBuffer({ x, y }, "^", Color::Intensity);
				}
				else 
				{
					renderer.WriteToBuffer({ x, y }, "~", Color::Intensity);
				}
			}
			break;
			case TerrainType::Territory:
				renderer.WriteToBuffer({ x, y }, " ", Color::Intensity);
				break;
			case TerrainType::Pit:
				renderer.WriteToBuffer({ x, y }, " ", Color::Intensity);
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
		for (int x = 0; x < Width(); ++x)
		{
			char buffer[4];

			// 이동 가능 여부
			bool canMove = CanMove({ x, y });

			// 이동 가능 여부에 따른 색깔
			Color color = canMove ? Color::LightBlue : Color::LightRed;

			int data = (int)(GetWeightMap({ x, y }) * 2);
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

	bool isBlocked = occupied[position.y][position.x];
	bool canMove = !(weight[position.y][position.x] < 0.0f);

	return !isBlocked && canMove;
}

float Map::GetWeightMap(const Vector2I& position) const
{
	return weightMultiple * weight[position.y][position.x];
}

void Map::SetOccupiedMap(const Vector2I& position, bool value)
{
	occupied[position.y][position.x] = value;
}
