#include <drivers/oledGfx.h>
#include "stdlib.h"

#ifdef __cplusplus
extern "C"
{
#endif

OledGfx::OledGfx(void) :
    _Row(0), _Column(0), _FontSize(FontSize::F5x8)
{
}

void OledGfx::SetFontSize(FontSize size)
{
  _FontSize = size;
}

void OledGfx::PrintString(uint8_t x, uint8_t y, const uint8_t *text, const FontSize size, const Color color)
{
  SetColor(color);

  _FontSize = size;

  if (_FontSize == FontSize::F5x8)
  {
    DisplayString5x8(x, y, text);
  }
  else if (_FontSize == FontSize::F8x16)
  {
    DisplayString8x16(x, y, text);
  }
}

void OledGfx::DisplayString(int x, int y, const void* text, const FontInfo info)
{
  char* asciiTableChar = nullptr;
  int n = 0;
  constexpr char space = 0x20;
  constexpr char tilde = 0x7E;

  if (x > (OledDriver::ScreenWidth - info.width))
  {
    x = 0;
  }

  char* ch = (char*)text;

  while (*ch++ != '\0')
  {
    if ((*ch >= space) && (*ch <= tilde))
    {
      // Compute character address into the appropriate ASCII table
      asciiTableChar = (char*)info.asciiTable + (*ch - space);

      for (int k = 0; k < info.width; k++)
      {

      }

      // Increment x by a character width
      x += info.width;
      if (x >= OledDriver::ScreenWidth)
      {
        return;
      }
    }
  }

}

void OledGfx::DisplayString8x16(uint8_t x, uint8_t y, const uint8_t *text)
{
  uint16_t i = 0;
  uint16_t j = 0;
  uint16_t k = 0;
  uint16_t n = 0;

  if (x > 120)
  {
    x = 1;
  }

  while (text[i] > 0x00)
  {
    if ((text[i] >= 0x20) && (text[i] <= 0x7e))
    {
      j = text[i] - 0x20;

      for (n = 0; n < 2; n++)
      {
        for (k = 0; k < 8; k++) // character width
        {
          SetAddress(x + k, n * 8 + y);
          WriteText(ascii_table_8x16[j][k + 8 * n]);
        }
      }
      x += 8;
    }

    i++;
  }
}

void OledGfx::DisplayString5x8(uint8_t x, uint8_t y, const uint8_t *text)
{
  uint8_t i = 0, j, k;

  while (text[i] > 0x00)
  {
    if ((text[i] >= 0x20) && (text[i] <= 0x7e))
    {
      j = text[i] - 0x20;

      for (k = 0; k < 5; k++)
      {
        SetAddress(x + k, y);
        WriteText(ascii_table_5x8[j][k]);
      }
      SetAddress(x + 5, y);
      WriteText(0x00);
      i++;
      x += 6;
      if (x >= 128)
      {
        x = 1;
      }
    }
    else
    {
      i++;
    }
  }
}

void OledGfx::SetTextCoordinate(uint8_t x, uint8_t y)
{
  _Column = x;
  _Row = y;
}

void OledGfx::DrawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1)
{
  // Update in subclasses if desired!
  if (x0 == x1)
  {
    if (y0 > y1)
    {
      swap(y0, y1);
    }
    DrawFastVLine(x0, y0, y1 - y0 + 1);
  }
  else if (y0 == y1)
  {
    if (x0 > x1)
    {
      swap(x0, x1);
    }
    DrawFastHLine(x0, y0, x1 - x0 + 1);
  }
  else
  {
    WriteLine(x0, y0, x1, y1);
  }
}

void OledGfx::WriteLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1)
{

  int16_t steep = abs(y1 - y0) > abs(x1 - x0);

  if (steep)
  {
    swap(x0, y0);
    swap(x1, y1);
  }
  if (x0 > x1)
  {
    swap(x0, x1);
    swap(y0, y1);
  }

  int16_t dx, dy;
  dx = x1 - x0;
  dy = abs(y1 - y0);

  int16_t err = dx / 2;
  int16_t ystep;

  if (y0 < y1)
  {
    ystep = 1;
  }
  else
  {
    ystep = -1;
  }

  for (; x0 <= x1; x0++)
  {
    if (steep)
    {
      DrawPixel(y0, x0);
    }
    else
    {
      DrawPixel(x0, y0);
    }
    err -= dy;

    if (err < 0)
    {
      y0 += ystep;
      err += dx;
    }
  }
}

// Draw a rectangle
void OledGfx::DrawRect(int16_t x, int16_t y, int16_t w, int16_t h)
{

  DrawFastHLine(x, y, w);
  DrawFastHLine(x, y + h - 1, w);
  DrawFastVLine(x, y, h);
  DrawFastVLine(x + w - 1, y, h);
}

/**************************************************************************/
/*!
 @brief  Draws a filled rectangle using HW acceleration
 */
/**************************************************************************/
void OledGfx::FillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{

  if ((x >= ScreenWidth) || (y >= ScreenHeight))
    return;

  // Y bounds check
  if (y + h > ScreenHeight)
  {
    h = ScreenHeight - y - 1;
  }

  // X bounds check
  if (x + w > ScreenWidth)
  {
    w = ScreenWidth - x - 1;
  }

  // set location
  WriteCommand(SSD1351_CMD_SETCOLUMN);
  WriteData(x);
  WriteData(x + w - 1);
  WriteCommand(SSD1351_CMD_SETROW);
  WriteData(y);
  WriteData(y + h - 1);
  // fill!
  WriteCommand(SSD1351_CMD_WRITERAM);

  for (uint16_t i = 0; i < w * h; i++)
  {
    WriteData(_FillColor, 2);
  }
}

void OledGfx::FillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const Color color)
{
  SetFillColor(color);
  FillRect(x, y, w, h);
}

void OledGfx::FillCircle(int16_t x0, int16_t y0, int16_t r)
{

  DrawFastVLine(x0, y0 - r, 2 * r + 1);
  FillCircleHelper(x0, y0, r, 3, 0);
}

// Used to do circles and roundrects
void OledGfx::FillCircleHelper(int16_t x0, int16_t y0, int16_t r,
    uint8_t corner, int16_t delta)
{

  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;

  while (x < y)
  {
    if (f >= 0)
    {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;

    if (corner & 0x1)
    {
      DrawFastVLine(x0 + x, y0 - y, 2 * y + 1 + delta);
      DrawFastVLine(x0 + y, y0 - x, 2 * x + 1 + delta);
    }
    if (corner & 0x2)
    {
      DrawFastVLine(x0 - x, y0 - y, 2 * y + 1 + delta);
      DrawFastVLine(x0 - y, y0 - x, 2 * x + 1 + delta);
    }
  }
}

// Draw a circle outline
void OledGfx::DrawCircle(int16_t x0, int16_t y0, int16_t r)
{
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;

  DrawPixel(x0, y0 + r);
  DrawPixel(x0, y0 - r);
  DrawPixel(x0 + r, y0);
  DrawPixel(x0 - r, y0);

  while (x < y)
  {
    if (f >= 0)
    {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;

    DrawPixel(x0 + x, y0 + y);
    DrawPixel(x0 - x, y0 + y);
    DrawPixel(x0 + x, y0 - y);
    DrawPixel(x0 - x, y0 - y);
    DrawPixel(x0 + y, y0 + x);
    DrawPixel(x0 - y, y0 + x);
    DrawPixel(x0 + y, y0 - x);
    DrawPixel(x0 - y, y0 - x);
  }
}

// Draw a rounded rectangle
void OledGfx::DrawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h,
    int16_t r)
{
  // smarter version

  DrawFastHLine(x + r, y, w - 2 * r); // Top
  DrawFastHLine(x + r, y + h - 1, w - 2 * r); // Bottom
  DrawFastVLine(x, y + r, h - 2 * r); // Left
  DrawFastVLine(x + w - 1, y + r, h - 2 * r); // Right
  // draw four corners
  DrawCircleHelper(x + r, y + r, r, 1);
  DrawCircleHelper(x + w - r - 1, y + r, r, 2);
  DrawCircleHelper(x + w - r - 1, y + h - r - 1, r, 4);
  DrawCircleHelper(x + r, y + h - r - 1, r, 8);
}

void OledGfx::DrawCircleHelper(int16_t x0, int16_t y0, int16_t r,
    uint8_t corner)
{
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;

  while (x < y)
  {
    if (f >= 0)
    {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;
    if (corner & 0x4)
    {
      DrawPixel(x0 + x, y0 + y);
      DrawPixel(x0 + y, y0 + x);
    }
    if (corner & 0x2)
    {
      DrawPixel(x0 + x, y0 - y);
      DrawPixel(x0 + y, y0 - x);
    }
    if (corner & 0x8)
    {
      DrawPixel(x0 - y, y0 + x);
      DrawPixel(x0 - x, y0 + y);
    }
    if (corner & 0x1)
    {
      DrawPixel(x0 - y, y0 - x);
      DrawPixel(x0 - x, y0 - y);
    }
  }
}

// Draw a triangle
void OledGfx::DrawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
    int16_t x2, int16_t y2)
{
  DrawLine(x0, y0, x1, y1);
  DrawLine(x1, y1, x2, y2);
  DrawLine(x2, y2, x0, y0);
}

#ifdef __cplusplus
}
#endif

