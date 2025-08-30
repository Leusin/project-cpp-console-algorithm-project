#include "Territory.h"

#include "QuadTree/QuadTree.h"
#include "Render/Renderer.h"


Territory::Territory(int id, Vector2I position, Vector2I size, const QuadTree& qTree, Team::Type initialOwner)
	: QEntity(position, Team::GetTeamColor(initialOwner), "#", size)
	, id{ id }
	, qTree{ qTree }
	, owner{ initialOwner }
{
}

void Territory::SetOwner(const Team::Type& type)
{
	color = Team::GetTeamColor(type);
	owner = type;
}

void Territory::Draw(Renderer& renderer)
{
	// 일부러 부모 클래스 함수 호출 안 함

	for (int x = bounds.GetX(); x <= bounds.MaxX(); ++x)
	{
		for (int y = bounds.GetY(); y <= bounds.MaxY(); ++y)
		{
			//renerer.WriteToBuffer({ x, y }, GetImage(), color, 2);
			if (x % 2 == 0 || y % 2 == 0)//if ((x + y) % 2 == 0)
			{
				renderer.WriteToBuffer({ x, y }, GetImage(), color, 2);
			}
			else
			{
				renderer.WriteToBuffer({ x, y }, "$", color, 2);
			}
		}
	}
}
