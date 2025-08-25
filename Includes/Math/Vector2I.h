#pragma once

#include <windows.h>
#include "Core.h"
#include "Vector2F.h"

struct Engine_API Vector2I
{
    Vector2I(int x = 0, int y = 0);

    Vector2I operator+(const Vector2I& other) const;
    Vector2I operator-(const Vector2I& other) const;
    bool operator==(const Vector2I& other) const;
    bool operator!=(const Vector2I& other) const;

    // 형변환 연산자 오버로딩
    operator COORD();
    operator COORD() const;
    operator Vector2F();
    operator Vector2F() const;

    // 직교 좌표계에서의 방향 벡터
    static Vector2I Zero;
    static Vector2I One;
    static Vector2I Right;

public:
    int x = 0;
    int y = 0;
};
