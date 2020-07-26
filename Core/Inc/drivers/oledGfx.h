#pragma once

#include <drivers/asciiFonts.h>
#include <drivers/oledDriver.h>
#include "stm32f4xx_hal.h"


#ifdef __cplusplus
extern "C"  {
#endif


class OledGfx : public virtual OledDriver
{
  public:
    struct FontInfo
    {
      int width;
      int height;
      const void* asciiTable;
    };

    static constexpr FontInfo __Font5x8 = {5, 8, ascii_table_5x8};
    static constexpr FontInfo __Font8x16 = {8, 16, ascii_table_8x16};

    enum class FontSize
    {
      F5x8 = 0,
      F8x16
    };

  private:
    uint8_t   _Row;
    uint8_t   _Column;
    FontSize  _FontSize;

  public:
    OledGfx(void);

    void SetFontSize(FontSize size);
    void SetTextCoordinate(uint8_t x, uint8_t y);

    void DrawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1);

    void DrawRect(int16_t x, int16_t y, int16_t w, int16_t h);
    void FillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
    void FillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const Color color);

    void FillCircle(int16_t x0, int16_t y0, int16_t r);
    void DrawCircle(int16_t x0, int16_t y0, int16_t r);

    void DrawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r);
    void DrawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2);

    void PrintString(uint8_t x, uint8_t y, const uint8_t *text, const FontSize size, const Color color);
    void DisplayString(int x, int y, const void* text, const FontInfo info);
    void DisplayString5x8(uint8_t x, uint8_t y, const uint8_t *text);
    void DisplayString8x16(uint8_t x, uint8_t y, const uint8_t *text);

  private:
    void WriteLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1);

    void FillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta);
    void DrawCircleHelper( int16_t x0, int16_t y0, int16_t r, uint8_t corner);
};


#ifdef __cplusplus
}
#endif
