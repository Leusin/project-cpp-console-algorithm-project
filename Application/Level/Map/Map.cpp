#include "Map.h"

#include "Engine.h"
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
	*/

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

	int dataWidth = (int)data[0].size();
	int dataHeight = (int)data.size();

	double scaleX = static_cast<double>(dataWidth) / Engine::Width();
	double scaleY = static_cast<double>(dataHeight) / Engine::Height();

	// Populate the new, scaled map grid
	for (int y = 0; y < Engine::Height(); ++y)
	{
		for (int x = 0; x < Engine::Width(); ++x)
		{
			// Calculate the scaled coordinates using floating-point math
			int originalX = static_cast<int>(x * scaleX);
			int originalY = static_cast<int>(y * scaleY);

			// Clamping (always good practice)
			if (originalX >= dataWidth) { originalX = dataWidth - 1; }
			if (originalY >= dataHeight) { originalY = dataHeight - 1; }

			int terrainCode = data[originalY][originalX];
			map[y][x] = static_cast<TerrainType>(terrainCode);
			weight[y][x] = speedMultipliers[static_cast<size_t>(map[y][x])];
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