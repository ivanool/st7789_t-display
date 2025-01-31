#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_log.h"

//PIN

#define TFT_CS 5
#define TFT_DC 16
#define TFT_RST 23
#define TFT_SCLK 18
#define TFT_MOSI 19
#define TFT_BL 4


//CMD
#define SWRESET 0x01 //RESET screen 
#define NOP 0x00
#define SLPIN 0x10 //SLEEP MODE
#define SLPOUT 0x11 //SLEEP OUT
#define PTLON 0x12 //PARTIAL DISPLAY MODE ON
#define NORON 0x13 // NORMAL DISPLAY MODE ON
#define INVOFF 0x20 //DISPLAY INVERSION MODE OFF
#define INVON 0x21 //DISPLAY INVERSION ON
#define DISPOFF 0x28 //DISPLAY OFF 
#define DISPON 0x29 //DISPLAY On
#define CASET 0x2A //COLUMN ADDRESS SET
#define RASET 0x2B
#define COLMOD 0x3A
#define MADCTL 0x36
#define PORCTRL 0xB2
#define GCTRL 0xB7
#define POWSAVE 0xBC
#define VCOMS 0xBB
#define RAMWR 0x2C

//DATA FOR CMD

#define COLOR_65K 0x55

//program
#define CMD_MODE 0
#define DATA_MODE 1
#define SPI_FREQUENCY 40000000
#define TFT_HEIGHT 320
#define TFT_WIDTH 240
#define WINDOW_PIXEL 76800


void RESET();
void spi_init();
void send_data8(uint8_t data);
void send_data16(uint16_t data);
void send_cmd(uint8_t cmd);
void INIT();
void enable_backlight();
void porch_control(uint8_t bpa, uint8_t fpa, bool psen, uint8_t bpb, uint8_t fpb, uint8_t bpc, uint8_t fpc);
void set_window(uint16_t x1, uint16_t x2, uint16_t y1, uint16_t y2);
void clear_screen_slow(uint16_t color, uint32_t window_size);
uint16_t rgb888_to_rgb565(uint8_t r, uint8_t g, uint8_t b);
void draw_row(uint16_t y, uint16_t *pixels);
void draw_pixel(uint16_t x1, uint16_t y, uint16_t color);
void clear_screen(uint16_t color);
void clear_screen_fastest(uint16_t color);