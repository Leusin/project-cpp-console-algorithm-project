#include "DragBox.h"

#include <cmath>
#include "Input.h"
#include "Render/Renderer.h"
#include "QuadTree/QuadTree.h"
#include "QuadTree/QNode.h"
#include "Actor/AUnit/AUnit.h"


DragBox::DragBox(QuadTree& quadTree)
	: bounds{ Vector2I::Zero }
	, isDragging{ false }
	, quadTree{ quadTree }
{
}

void DragBox::Tick()
{
	static Vector2I dragStart = Vector2I::Zero;

	// 마우스를 누르는 순간
	if (Input::Get().GetMouseDown(MounseButton::Left))
	{
		isDragging = true;
		// drag start
		dragStart.x = Input::Get().GetMouseX();
		dragStart.y = Input::Get().GetMouseY();
	}

	// 누르는 중
	if (isDragging)
	{
		// drag end
		int mouseX = Input::Get().GetMouseX();
		int mouseY = Input::Get().GetMouseY();

		// 만일 시작점이 현재 마우스 보다 작은값이면(화면 좌표계 기존)
		int x = (dragStart.x < mouseX) ? dragStart.x : mouseX;
		int y = (dragStart.y < mouseY) ? dragStart.y : mouseY;

		// 너비/높이를 구한다
		int width = std::abs(mouseX - dragStart.x);
		int height = std::abs(mouseY - dragStart.y);

		bounds.SetPosition(x, y);
		bounds.SetSize(width, height);
	}

	// 마우스 떼는 순간
	if (Input::Get().GetMouseUp(MounseButton::Left))
	{
		// drag end
		isDragging = false;

		// 쿼드 트리에게 해당 영역에 관리중인 객체들이 있는지 질의한다
		// 그 중 AUnit 인 객체의 선택 상태를 true로 만든다
		std::vector<QEntity*> intersects;
		if (quadTree.Query(bounds, intersects))
		{
			for (QEntity* entity: intersects)
			{
				if (entity->As<AUnit>())
				{
					((AUnit*)entity)->SetIsSelected(true);
				}
			}
		}
	}
}

void DragBox::Draw(Renderer& renderer)
{
	if (!isDragging)
	{
		return;
	}

	bounds.Draw(renderer);
}

std::vector<class Unit*> DragBox::GetSelectedUnits(const std::vector<Unit*>& allUnits)
{
	return std::vector<class Unit*>();
}
