#pragma once

enum class Color : int
{
    Black = 0,
    Blue = 1,
    Green = 2,
    Cyan = Blue | Green, // 청록색 (1|2 = 3)
    Red = 4,
    Magenta = Red | Blue, // 자홍색 (4|1 = 5)
    Yellow = Red | Green, // 노란색 (4|2 = 6)
    White = Red | Green | Blue, // 흰색 (4|2|1 = 7)

    // 밝은 색상 (Intensity 비트를 추가)
    Intensity = 8,
    LightBlue = Blue | Intensity,
    LightGreen = Green | Intensity,
    LightCyan = Cyan | Intensity,
    LightRed = Red | Intensity,
    LightMagenta = Magenta | Intensity,
    LightYellow = Yellow | Intensity,
    LightWhite = White | Intensity,
};