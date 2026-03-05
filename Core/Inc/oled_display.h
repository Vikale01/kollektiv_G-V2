#ifndef OLED_DISPLAY_H_
#define OLED_DISPLAY_H_

#include "main.h"

extern I2C_HandleTypeDef hi2c1;

#define SSD1327_I2C_ADRESS          0x7A // Eller 0x78

#define LCD_WIDTH                   128
#define LCD_HEIGHT                  128

#define SET_COLUMN_ADDRESS          0x15
#define SET_ROW_ADDRESS             0x75
#define SET_CONTRAST_C              0x81


#define SET_DISPLAY_REMAP           0xA0
#define SET_DISPLAY_STARTLINE       0xA1
#define SET_DISPLAY_OFFSET          0xA2

#define NORMAL_MODE                 0xA4
#define SET_MUX_RATIO               0xA8
#define DISPLAY_ON                  0xAF
#define DISPLAY_OFF                 0xAE

#define SET_PHASE_LENGTH            0xB1
#define SET_FRONT_CLOCK             0xB3

#define SET_COND_C_PERIOD           0xB6
#define SET_GRAY_SCALE_T            0xB8
#define SET_DEFAULT_LINEAR          0xB9
#define SET_PRE_C_VOLTAGE           0xBC
#define SET_VCOM_VOLTAGE            0xBE

#define FUNCTION_SELEC              0xD5

#define SET_COMMANDLOCK             0xFD

#define ICON_GPS_W  20
#define ICON_GPS_H  20

extern const uint8_t icon_gps_20x20[];
extern const uint8_t icon_bt_20x20[];
extern const uint8_t logo[];
extern const uint8_t Afry_logo[];
extern const uint8_t KG_logo[];

extern const uint8_t digit_0[];
extern const uint8_t digit_1[];
extern const uint8_t digit_2[];
extern const uint8_t digit_3[];
extern const uint8_t digit_4[];
extern const uint8_t digit_5[];
extern const uint8_t digit_6[];
extern const uint8_t digit_7[];
extern const uint8_t digit_8[];
extern const uint8_t digit_9[];

void Oled_init(void);
void Oled_Update(void);
void Oled_Clear(uint8_t color);
void Oled_DrawPixel(int16_t x, int16_t y, uint8_t color);
void DrawSquare(int x_start, int y_start, int x_end, int y_end, uint8_t color);
void DrawLine(int x0, int y0, int x1, int y1, int color);
void Oled_BLE_Connected();
void Oled_BLE_IDLE();
void DrawCircle(int x0, int y0, int radius, int color);
void Oled_Draw1BitImage(int x0, int y0, const uint8_t *img, uint8_t w, uint8_t h, uint8_t color);
void Oled_drawDigit(int x, int y, uint8_t digit, uint8_t color);
void Oled_Update_Sqr(uint8_t x, uint8_t y, uint8_t w, uint8_t h);
void UpdateClockDisplay(void);
void Oled_drawColon(int x, int y, uint8_t color);
void SSD1327_UpdateArea(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t *data);
void Oled_introScreen(void);

#endif