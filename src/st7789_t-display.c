#include "st7789_t-display/include/st7789_t-display.h"


spi_device_handle_t spi;


void send_cmd(uint8_t cmd) {
    gpio_set_level(TFT_DC, CMD_MODE);
    spi_transaction_t t = {
        .length = 8,
        .tx_buffer = &cmd
    };
    ESP_ERROR_CHECK(spi_device_transmit(spi, &t));
}

void send_data16(uint16_t data){
  gpio_set_level(TFT_DC, DATA_MODE);
  uint8_t buffer[2] = {data >> 8, data & 0xFF};
  spi_transaction_t t = {
    .length = 16,
    .tx_buffer = buffer
  };
  ESP_ERROR_CHECK(spi_device_transmit(spi, &t));
}

void send_data8(uint8_t data) {
    gpio_set_level(TFT_DC, DATA_MODE);
    spi_transaction_t t = {
        .length = 8,
        .tx_buffer = &data
    };
    ESP_ERROR_CHECK(spi_device_transmit(spi, &t));
}


void enable_backlight() {
    gpio_set_direction(TFT_BL, GPIO_MODE_OUTPUT);
    gpio_set_level(TFT_BL, 1);
}


void RESET(){
    gpio_set_level(TFT_RST, 0);
    vTaskDelay(pdMS_TO_TICKS(20));
    gpio_set_level(TFT_RST, 1);
    vTaskDelay(pdMS_TO_TICKS(150));
}

void porch_control(uint8_t bpa, uint8_t fpa, bool psen, uint8_t bpb, uint8_t fpb, uint8_t bpc, uint8_t fpc) {
    send_cmd(PORCTRL);
    send_data(bpa & 0x7F);
    send_data(fpa & 0x7F);
    uint8_t byte3 = (psen ? 0x80 : 0x00) | (bpb & 0x0F);
    send_data(byte3);
    send_data(fpb & 0x0F);
    send_data(bpc & 0x0F);
    send_data(fpc & 0x0F);
}

void set_orientation(uint8_t data){
  send_cmd(MADCTL);
  send_data(data);
}

void set_window(uint16_t x0, uint16_t x1, uint16_t y0, uint16_t y1) {
    send_cmd(CASET);
    send_data16(x0);
    send_data16(x1);
    
    send_cmd(RASET);
    send_data16(y0);
    send_data16(y1);
    
    //send_cmd(RAMWR);
}

void gpio_init() {
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << TFT_DC) | (1ULL << TFT_RST) | (1ULL << TFT_BL),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);
}

void INIT() {
    gpio_init();
    RESET();
    
    send_cmd(SLPOUT);
    vTaskDelay(pdMS_TO_TICKS(120));

    send_cmd(COLMOD);
    send_data8(0x55); // TODO: ALLOW USER CONTROL

    set_orientation(0x20); // TODO: ALLOW USER CONTROL

    porch_control(0x0C, 0x0C, 0x00, 0x02, 0x02, 0x02, 0x02); // TODO: ALLOW USER CONTROL
    
    send_cmd(GCTRL);
    send_data8(0x35);

    send_cmd(VCOMS);
    send_data8(0x1F); // TODO: ALLOW USER CONTROL

    vTaskDelay(pdMS_TO_TICKS(10));

    send_cmd(DISPON);
    vTaskDelay(pdMS_TO_TICKS(150));
    
    enable_backlight();
}

void spi_init() {
    spi_bus_config_t buscfg = {
        .mosi_io_num = TFT_MOSI,
        .sclk_io_num = TFT_SCLK,
        .miso_io_num = -1,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4096
    };

    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = SPI_FREQUENCY,
        .mode = 0,
        .spics_io_num = TFT_CS,
        .queue_size = 7,
        .flags = SPI_DEVICE_NO_DUMMY
    };

    ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO));
    ESP_ERROR_CHECK(spi_bus_add_device(SPI2_HOST, &devcfg, &spi));
}
