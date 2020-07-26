#include <userInterface.h>
#include <drivers/oledDriver.h>
#include <drivers/oledGfx.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BATTERY_MAX_VOLTAGE (42)                                        // Maximum battery voltage (assuming a 10s battery)
#define BATTERY_MIN_VOLTAGE (36)                                        // Minimum battery voltage (assuming a 10s battery)
#define BATTERY_WORK_AREA   (BATTERY_MAX_VOLTAGE - BATTERY_MIN_VOLTAGE) // Working amplitude of the battery


typedef struct
{
  OledGfx screenDriver;
} userInterfaceContext;

static userInterfaceContext context;

static void draw_battery(void);

/**
 * @brief Convert a state of charge to a battery level
 *
 * @param stateOfCharge The state of charge to convert
 *
 * @return a battery level between 0 and 5
 */
static int sof2battery_level(int stateOfCharge);


int ui_initialize(void)
{
  context.screenDriver = OledGfx();
  context.screenDriver.DeviceInit();
  draw_battery();
  return 0;
}

void ui_print_esc_values(mc_values *val)
{
//  printf("Input voltage: %.2f V\r\n", val->v_in);
//  printf("Temp:          %.2f degC\r\n", val->temp_mos);
//  printf("Current motor: %.2f A\r\n", val->current_motor);
//  printf("Current in:    %.2f A\r\n", val->current_in);
//  printf("RPM:           %.1f RPM\r\n", val->rpm);
//  printf("Duty cycle:    %.1f %%\r\n", val->duty_now * 100.0);
//  printf("Ah Drawn:      %.4f Ah\r\n", val->amp_hours);
//  printf("Ah Regen:      %.4f Ah\r\n", val->amp_hours_charged);
//  printf("Wh Drawn:      %.4f Wh\r\n", val->watt_hours);
//  printf("Wh Regen:      %.4f Wh\r\n", val->watt_hours_charged);
//  printf("Tacho:         %i counts\r\n", val->tachometer);
//  printf("Tacho ABS:     %i counts\r\n", val->tachometer_abs);
//  printf("Fault Code:    %s\r\n", bldc_interface_fault_to_string(val->fault_code));
  int battery_percent = (((float)(val->v_in - BATTERY_MIN_VOLTAGE) * 100) / BATTERY_WORK_AREA);
  if (battery_percent < 0)
  {
    battery_percent = 0;
  }
  ui_fill_battery(battery_percent);

  context.screenDriver.SetColor(OledDriver::Color::White);
  context.screenDriver.SetFontSize(OledGfx::FontSize::F5x8);

  char value[20];
  // Battery voltage
  sprintf(value, "%.1f V", val->v_in);
  context.screenDriver.DisplayString5x8(2, 75, (uint8_t *)value);
  // Mosfet temperature
  sprintf(value, "%.1f C", val->temp_mos);
  context.screenDriver.DisplayString5x8(2, 85, (uint8_t *)value);
  // Current motor
  sprintf(value, "%.1f A", val->current_motor);
  context.screenDriver.DisplayString5x8(2, 95, (uint8_t *)value);
  // Duty cycle
  sprintf(value, "%.1f %%", val->duty_now * 100);
  context.screenDriver.DisplayString5x8(2, 105, (uint8_t *)value);
}

void draw_battery(void)
{
  context.screenDriver.SetColor(OledDriver::Color::White);
  context.screenDriver.DrawFastHLine(2, 5, 13);
  context.screenDriver.DrawFastHLine(2, 6, 13);
  context.screenDriver.DrawFastHLine(24, 5, 13);
  context.screenDriver.DrawFastHLine(24, 6, 13);

  context.screenDriver.DrawFastHLine(14, 1, 11);
  context.screenDriver.DrawFastHLine(14, 2, 11);

  context.screenDriver.DrawFastHLine(2, 70, 13);
  context.screenDriver.DrawFastHLine(2, 71, 13);
  context.screenDriver.DrawFastHLine(24, 70, 13);
  context.screenDriver.DrawFastHLine(24, 71, 13);

  context.screenDriver.DrawFastVLine(14, 1, 5);
  context.screenDriver.DrawFastVLine(24, 1, 5);

  context.screenDriver.DrawFastVLine(3, 6, 64);
  context.screenDriver.DrawFastVLine(2, 6, 64);
  context.screenDriver.DrawFastVLine(35, 6, 64);
  context.screenDriver.DrawFastVLine(36, 6, 64);
}

static int sof2battery_level(int stateOfCharge)
{
  if (stateOfCharge > 80)
  {
    return 5;
  }
  if (stateOfCharge > 60)
  {
    return 4;
  }
  if (stateOfCharge > 40)
  {
    return 3;
  }
  if (stateOfCharge > 20)
  {
    return 2;
  }
  if (stateOfCharge > 0)
  {
    return 1;
  }

  return 0;
}

void ui_fill_battery(int stateOfCharge)
{
  static int currentBatteryLevel = -1;
  int requiredBatteryLevel = sof2battery_level(stateOfCharge);
  int areaToDelete = 0;

  if (requiredBatteryLevel == currentBatteryLevel)
  {
    // Nothing to do, as the battery level didn't change since last measurement
    return;
  }

  // Compute if there is some graph bars to delete
  if (currentBatteryLevel > -1)
  {
    int barsToDelete = (currentBatteryLevel - requiredBatteryLevel);
    areaToDelete = (barsToDelete * 10) + (barsToDelete * 2);
  }

  // Delete the useless battery level area
  if (areaToDelete > 0)
  {
    context.screenDriver.FillRect(6, 9, 27, areaToDelete, OledDriver::Color::Black);
  }

  // Draw battery bars
  if (requiredBatteryLevel > 4)
  {
    context.screenDriver.FillRect(6, 9, 27, 10, OledDriver::Color::Mediumspringgreen);
  }

  if (requiredBatteryLevel > 3)
  {
    context.screenDriver.FillRect(6, 21, 27, 10, OledDriver::Color::Springgreen);
  }

  if (requiredBatteryLevel > 2)
  {
    context.screenDriver.FillRect(6, 33, 27, 10, OledDriver::Color::Greenyellow);
  }

  if (requiredBatteryLevel > 1)
  {
    context.screenDriver.FillRect(6, 45, 27, 10, OledDriver::Color::Yellow);
  }

  if (requiredBatteryLevel > 0)
  {
    context.screenDriver.FillRect(6, 57, 27, 10, OledDriver::Color::Red);
  }
}

#ifdef __cplusplus
}
#endif
