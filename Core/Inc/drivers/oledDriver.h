#pragma once


#include "stm32f4xx_hal.h"
#include "spi.h"

#define SSD1351_CMD_SETCOLUMN       0x15
#define SSD1351_CMD_SETROW          0x75
#define SSD1351_CMD_WRITERAM        0x5C
#define SSD1351_CMD_READRAM         0x5D
#define SSD1351_CMD_SETREMAP        0xA0
#define SSD1351_CMD_STARTLINE       0xA1
#define SSD1351_CMD_DISPLAYOFFSET 	0xA2
#define SSD1351_CMD_DISPLAYALLOFF 	0xA4
#define SSD1351_CMD_DISPLAYALLON  	0xA5
#define SSD1351_CMD_NORMALDISPLAY 	0xA6
#define SSD1351_CMD_INVERTDISPLAY 	0xA7
#define SSD1351_CMD_FUNCTIONSELECT 	0xAB
#define SSD1351_CMD_DISPLAYOFF      0xAE
#define SSD1351_CMD_DISPLAYON     	0xAF
#define SSD1351_CMD_PRECHARGE       0xB1
#define SSD1351_CMD_DISPLAYENHANCE  0xB2
#define SSD1351_CMD_CLOCKDIV        0xB3
#define SSD1351_CMD_SETVSL          0xB4
#define SSD1351_CMD_SETGPIO 		    0xB5
#define SSD1351_CMD_PRECHARGE2      0xB6
#define SSD1351_CMD_SETGRAY         0xB8
#define SSD1351_CMD_USELUT          0xB9
#define SSD1351_CMD_PRECHARGELEVEL 	0xBB
#define SSD1351_CMD_VCOMH           0xBE
#define SSD1351_CMD_CONTRASTABC     0xC1
#define SSD1351_CMD_CONTRASTMASTER  0xC7
#define SSD1351_CMD_MUXRATIO        0xCA
#define SSD1351_CMD_COMMANDLOCK     0xFD
#define SSD1351_CMD_HORIZSCROLL     0x96
#define SSD1351_CMD_STOPSCROLL      0x9E
#define SSD1351_CMD_STARTSCROLL     0x9F


#define swap(a, b) { uint16_t t = a; a = b; b = t; }

#define OLED_RST(x)   HAL_GPIO_WritePin(OLED_RST_GPIO_Port, OLED_RST_Pin, x)
#define OLED_DC(x)    HAL_GPIO_WritePin(OLED_DC_GPIO_Port, OLED_DC_Pin, x)
#define OLED_CS(x)    HAL_GPIO_WritePin(OLED_CS_GPIO_Port, OLED_CS_Pin, x)

#if INTERFACE_3WIRE_SPI

#define OLED_SCK(x)   HAL_GPIO_WritePin(OLED_SCK_GPIO_Port, OLED_SCK_Pin, x)
#define OLED_DIN(x)   HAL_GPIO_WritePin(OLED_DIN_GPIO_Port, OLED_DIN_Pin, x)

#endif


#ifdef __cplusplus
extern "C" {
#endif

  class OledDriver
  {
    public:
      static constexpr int ScreenWidth = 128;
      static constexpr int ScreenHeight = 128;

      enum class Color
      {
        Black             = (0x000000),
        Cyan              = (0x00FFFF),
        Magenta           = (0xFF00FF),
        Yellow            = (0xFFFF00),
        White             = (0xFFFFFF),
        Grey              = (0x696969),

        // Red palette
        Lightsalmon       = (0xFFA07A),
        Salmon            = (0xFA8072),
        Darksalmon        = (0xE9967A),
        Lightcoral        = (0xF08080),
        Indianred         = (0xCD5C5C),
        Crimson           = (0xDC143C),
        Firebrick         = (0xB22222),
        Darkred           = (0x8B0000),
        Red               = (0xFF0000),

        // Orange palette
        Tomato            = (0xFF6347),

        // Green palette
        Darkolivegreen    = (0x556B2F),
        Olive             = (0x808000),
        Olivedrab         = (0x6B8E23),
        Yellowgreen       = (0x9ACD32),
        Limegreen         = (0x32CD32),
        Lime              = (0x00FF00),
        Lawgreen          = (0x7CFC00),
        Chartreuse        = (0x7FFF00),
        Greenyellow       = (0xADFF2F),
        Springgreen       = (0x00FF7F),
        Mediumspringgreen = (0x00FA9A),
        Lightgreen        = (0x90EE90),
        Palegreen         = (0x98FB98),
        Darkseagreeen     = (0x8FBC8F),
        Mediumaquamarine  = (0x66CDAA),
        Mediumseagreen    = (0x3CB371),
        Seagreen          = (0x2E8B57),
        Forestgreen       = (0x228B22),
        Green             = (0x008000),
        Darkgreen         = (0x006400),

        // Yellow palette
        Khaki             = (0xF0E68C),
        Gold              = (0xFFD700),

        // Cyan palette
        Darkcyan          = (0x008B8B),

        // Blue palette
        Royalblue         = (0x4169E1),
        Blue              = (0x0000FF),
      };

    private:
      uint8_t _ColorBuffer[2];

    protected:
      uint8_t _FillColor[2];

    public:
      OledDriver(void);

      uint8_t *GetFillColor();

      void DeviceInit(void);
      void ClearScreen(void);
      void FillColor(Color rgb888);
      void SetCoordinates(uint16_t x, uint16_t y);

      void WriteText(uint8_t data1);
      void SetAddress(uint8_t column, uint8_t row);

      void SetColor(Color rgb888);
      void SetFillColor(Color rgb888);

      void Invert(bool v);
      void DrawPixel(int16_t x, int16_t y);

      void WriteData(uint8_t dat);
      void WriteData(uint8_t* dat_p, uint16_t length);

      void DrawFastHLine(int16_t x, int16_t y, int16_t length);
      void DrawFastVLine(int16_t x, int16_t y, int16_t length);
      void WriteCommand(uint8_t data1);

    private:
      void RAMAddress(void);
      uint16_t ConvertToRgb565(Color rgb888);
  };



#ifdef __cplusplus
}
#endif
