#pragma once

#include <windows.h>
#include <utility> // For std::pair
#include <functional> // For std::hash
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

    // 성분들을 절댓값으로 만드는 함수
    void Abs();
    Vector2I GetAbs() const;

    // 직교 좌표계에서의 방향 벡터
    static Vector2I Zero;
    static Vector2I One;
    static Vector2I Right;

public:
    int x = 0;
    int y = 0;
};

// 해시가 필요한 std:: 자료에게 전달하는 해시 함수
struct Vector2IHash
{
    std::size_t operator()(const Vector2I& v) const 
    {
        // A simple but effective way to combine two hashes.
        // It's a good practice to use a robust hash combination.
        return std::hash<int>()(v.x) ^ (std::hash<int>()(v.y) << 1);
    }
};