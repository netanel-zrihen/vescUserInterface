#include <drivers/oledDriver.h>
#include "main.h"
#include "gpio.h"
#include <string.h>
#include <drivers/asciiFonts.h>


#ifdef __cplusplus
extern "C"
{
#endif

OledDriver::OledDriver(void)
{
  memset(_ColorBuffer, 0, sizeof(_ColorBuffer));
  memset(_FillColor, 0, sizeof(_FillColor));
}

uint8_t* OledDriver::GetFillColor()
{
  return _FillColor;
}

uint16_t OledDriver::ConvertToRgb565(Color rgb888)
{
  int baseColor = (int)rgb888;

  uint8_t red = (baseColor >> 16) & 0xFF;
  uint8_t green = (baseColor >> 8) & 0xFF;
  uint8_t blue = baseColor & 0xFF;

  return (((red >> 3) & 0x1F) << 11)
       | (((green >> 2) & 0x3F) << 5)
       | ((blue >> 3) & 0x1F);
}

void OledDriver::SetColor(Color rgb888)
{
  uint16_t rgb565 = ConvertToRgb565(rgb888);
  _ColorBuffer[0] = (rgb565 >> 8) & 0xFF;
  _ColorBuffer[1] = rgb565 & 0xFF;
}

void OledDriver::SetFillColor(Color rgb888)
{
  uint16_t rgb565 = ConvertToRgb565(rgb888);
  _FillColor[0] = (rgb565 >> 8) & 0xFF;
  _FillColor[1] = rgb565 & 0xFF;
}

void OledDriver::WriteCommand(uint8_t cmd)
{
  OLED_CS(GPIO_PIN_RESET);

#if  INTERFACE_4WIRE_SPI

  OLED_DC(GPIO_PIN_RESET);

  while (HAL_SPI_Transmit(&hspi1, &cmd, 0x01, 0x10) != HAL_OK);

  OLED_DC(GPIO_PIN_SET);

#elif INTERFACE_3WIRE_SPI

  uint8_t i;
	uint16_t hwData = 0;

  hwData = (uint16_t)cmd & ~0x0100;

	for(i = 0; i < 9; i ++) {
		OLED_SCK(GPIO_PIN_RESET);
    if(hwData & 0x0100) {
      OLED_DIN(GPIO_PIN_SET);
		}
    else  {
      OLED_DIN(GPIO_PIN_RESET);
		}
    OLED_SCK(GPIO_PIN_SET);
		hwData <<= 1;
	}


#endif

  OLED_CS(GPIO_PIN_SET);
}

void OledDriver::WriteData(uint8_t dat)
{
  OLED_CS(GPIO_PIN_RESET);

#if  INTERFACE_4WIRE_SPI

  OLED_DC(GPIO_PIN_SET);

  while (HAL_SPI_Transmit(&hspi1, &dat, 0x01, 0x10) != HAL_OK);

  OLED_DC(GPIO_PIN_RESET);

#elif INTERFACE_3WIRE_SPI

  uint8_t i;
	uint16_t hwData = 0;

  hwData = (uint16_t)dat | 0x0100;

	for(i = 0; i < 9; i ++) {
    OLED_SCK(GPIO_PIN_RESET);
		if(hwData & 0x0100) {
      OLED_DIN(GPIO_PIN_SET);
		}
    else  {
      OLED_DIN(GPIO_PIN_RESET);
		}
    OLED_SCK(GPIO_PIN_SET);
		hwData <<= 1;
	}

#endif

  OLED_CS(GPIO_PIN_SET);
}

void OledDriver::WriteData(uint8_t *dat_p, uint16_t length)
{
  OLED_CS(GPIO_PIN_RESET);

#if INTERFACE_4WIRE_SPI

  OLED_DC(GPIO_PIN_SET);

  while (HAL_SPI_Transmit(&hspi1, dat_p, length, 0x10) != HAL_OK);

  OLED_DC(GPIO_PIN_RESET);

#elif INTERFACE_3WIRE_SPI

  uint8_t i,j;
	uint16_t hwData = 0;


  for(i = 0; i < length; i++) {

    hwData = (uint16_t)dat_p[i] | 0x0100;

    for(j = 0; j < 9; j ++) {
      OLED_SCK(GPIO_PIN_RESET);
      if(hwData & 0x0100) {
        OLED_DIN(GPIO_PIN_SET);
      } else {
        OLED_DIN(GPIO_PIN_RESET);
      }
      OLED_SCK(GPIO_PIN_SET);
      hwData <<= 1;
    }
  }
#endif

  OLED_CS(GPIO_PIN_SET);
}

void OledDriver::RAMAddress(void)
{
  WriteCommand(0x15);
  WriteData(0x00);
  WriteData(0x7f);

  WriteCommand(0x75);
  WriteData(0x00);
  WriteData(0x7f);
}

void OledDriver::ClearScreen(void)
{
  int i, j;

  uint8_t clear_byte[] = { 0x00, 0x00 };
  RAMAddress();
  WriteCommand(0x5C);
  for (i = 0; i < 128; i++)
  {
    for (j = 0; j < 128; j++)
    {
      WriteData(clear_byte, 2); //RAM data clear
    }
  }
}

void OledDriver::FillColor(Color rgb888)
{
  int i;
  int j;

  RAMAddress();
  WriteCommand(0x5C);
  SetColor(rgb888);
  for (i = 0; i < 128; i++)
  {
    for (j = 0; j < 128; j++)
    {
      WriteData(_ColorBuffer, 2);
    }
  }
}

void OledDriver::SetCoordinates(uint16_t x, uint16_t y)
{
  if ((x >= ScreenWidth) || (y >= ScreenHeight))
    return;
  //Set x and y coordinate
  WriteCommand(SSD1351_CMD_SETCOLUMN);
  WriteData(x);
  WriteData(ScreenWidth - 1);
  WriteCommand(SSD1351_CMD_SETROW);
  WriteData(y);
  WriteData(ScreenHeight - 1);
  WriteCommand(SSD1351_CMD_WRITERAM);
}

void OledDriver::SetAddress(uint8_t column, uint8_t row)
{
  WriteCommand(SSD1351_CMD_SETCOLUMN);
  WriteData(column);	//X start
  WriteData(column);	//X end
  WriteCommand(SSD1351_CMD_SETROW);
  WriteData(row);	//Y start
  WriteData(row + 7);	//Y end
  WriteCommand(SSD1351_CMD_WRITERAM);
}

void OledDriver::WriteText(uint8_t dat)
{
  uint8_t i;

  for (i = 0; i < 8; i++)
  {
    if (dat & 0x01)
    {
      WriteData(_ColorBuffer, 2);
    }
    else
    {
      WriteData(0x00);
      WriteData(0x00);
    }
    dat >>= 1;
  }
}

void OledDriver::Invert(bool v)
{
  if (v)
  {
    WriteCommand(SSD1351_CMD_INVERTDISPLAY);
  }
  else
  {
    WriteCommand(SSD1351_CMD_NORMALDISPLAY);
  }
}

void OledDriver::DrawPixel(int16_t x, int16_t y)
{
  // Bounds check.
  if ((x >= ScreenWidth) || (y >= ScreenHeight))
  {
    return;
  }
  if ((x < 0) || (y < 0))
  {
    return;
  }

  SetAddress(x, y);

  // transfer data
  WriteData(_ColorBuffer, 2);
}

void OledDriver::DeviceInit(void)
{

#if INTERFACE_3WIRE_SPI

  OLED_DC(GPIO_PIN_RESET);
  HAL_SPI_DeInit(&hspi1);
  SPI_GPIO_Init();

#endif

  OLED_CS(GPIO_PIN_RESET);

  OLED_RST(GPIO_PIN_RESET);
  HAL_Delay(50);
  OLED_RST(GPIO_PIN_SET);
  HAL_Delay(50);

  WriteCommand(0xfd);	// command lock
  WriteData(0x12);

  WriteCommand(0xfd);	// command lock
  WriteData(0xB1);

  WriteCommand(0xae);	// display off
  WriteCommand(0xa4); 	// Normal Display mode

  WriteCommand(0x15);	//set column address
  WriteData(0x00);     //column address start 00
  WriteData(0x7f);     //column address end 95
  WriteCommand(0x75);	//set row address
  WriteData(0x00);     //row address start 00
  WriteData(0x7f);     //row address end 63

  WriteCommand(0xB3);  // Internal oscillator frequency
  WriteData(0xF1);

  WriteCommand(0xCA);
  WriteData(0x7F);

  WriteCommand(0xa0);  //set re-map & data format
  WriteData(0x74);     //Horizontal address increment

  WriteCommand(0xa1);  //set display start line
  WriteData(0x00);     //start 00 line

  WriteCommand(0xa2);  //set display offset
  WriteData(0x00);

  WriteCommand(0xAB);
  WriteCommand(0x01);

  WriteCommand(0xB4);
  WriteData(0xA0);
  WriteData(0xB5);
  WriteData(0x55);

  WriteCommand(0xC1);
  WriteData(0xC8);
  WriteData(0x80);
  WriteData(0xC0);

  WriteCommand(0xC7);
  WriteData(0x0F);

  WriteCommand(0xB1);
  WriteData(0x32);

  WriteCommand(0xB2);  // Display enhancement
  WriteData(0xA4);
  WriteData(0x00);
  WriteData(0x00);

  WriteCommand(0xBB);
  WriteData(0x17);

  WriteCommand(0xB6);
  WriteData(0x01);

  WriteCommand(0xBE);
  WriteData(0x05);

  WriteCommand(0xA6);

  ClearScreen();
  WriteCommand(0xaf);	 //display on
}

// Draw a horizontal line ignoring any screen rotation.
void OledDriver::DrawFastHLine(int16_t x, int16_t y, int16_t length)
{
  // Bounds check
  if ((x >= ScreenWidth) || (y >= ScreenHeight))
  {
    return;
  }

  // X bounds check
  if (x + length > ScreenWidth)
  {
    length = ScreenWidth - x - 1;
  }

  if (length < 0)
  {
    return;
  }

  // set location
  WriteCommand(SSD1351_CMD_SETCOLUMN);
  WriteData(x);
  WriteData(x + length - 1);
  WriteCommand(SSD1351_CMD_SETROW);
  WriteData(y);
  WriteData(y);
  // fill!
  WriteCommand(SSD1351_CMD_WRITERAM);

  for (uint16_t i = 0; i < length; i++)
  {
    WriteData(_ColorBuffer, 2);
  }
}

// Draw a vertical line ignoring any screen rotation.
void OledDriver::DrawFastVLine(int16_t x, int16_t y, int16_t length)
{
  // Bounds check
  if ((x >= ScreenWidth) || (y >= ScreenHeight))
  {
    return;
  }
  // X bounds check
  if (y + length > ScreenHeight)
  {
    length = ScreenHeight - y - 1;
  }
  if (length < 0)
  {
    return;
  }

  // set location
  WriteCommand(SSD1351_CMD_SETCOLUMN);
  WriteData(x);
  WriteData(x);
  WriteCommand(SSD1351_CMD_SETROW);
  WriteData(y);
  WriteData(y + length - 1);
  // fill!
  WriteCommand(SSD1351_CMD_WRITERAM);

  for (uint16_t i = 0; i < length; i++)
  {
    WriteData(_ColorBuffer, 2);
  }
}

#ifdef __cplusplus
}
#endif

