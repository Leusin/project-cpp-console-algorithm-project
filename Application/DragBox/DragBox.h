#pragma once

#include <vector>
#include "Math/Vector2I.h"
#include "QuadTree/Bounds.h"

// DragBox.h
class DragBox 
{
public: // RAII

    DragBox(class QuadTree& quadTree);

public: // MESSAGE

    void Tick(); // 마우스 입력 감지 및 상태 업데이트
    void Draw(class Renderer& renderer); // 드래그 박스 그리기
    std::vector<class Unit*> GetSelectedUnits(const std::vector<Unit*>& allUnits);

private: // FILD

    Bounds bounds;
    bool isDragging = false;

    class QuadTree& quadTree;
};