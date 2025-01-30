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
  spi_transaction_t t = {
    .length = 16,
    .tx_buffer = &data
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
    send_cmd(PORCTRL); // 0xB2
    
    // Byte 1: BPA[6:0]
    send_data(bpa & 0x7F); // Asegura que solo se usen 7 bits
    
    // Byte 2: FPA[6:0]
    send_data(fpa & 0x7F); // Asegura que solo se usen 7 bits
    
    // Byte 3: PSEN (bit 7) + BPB[3:0]
    uint8_t byte3 = (psen ? 0x80 : 0x00) | (bpb & 0x0F); // PSEN en bit 7, BPB en bits 3:0
    send_data(byte3);
    
    // Byte 4: FPB[3:0]
    send_data(fpb & 0x0F); // Asegura que solo se usen 4 bits
    
    // Byte 5: BPC[3:0]
    send_data(bpc & 0x0F); // Asegura que solo se usen 4 bits
    
    // Byte 6: FPC[3:0]
    send_data(fpc & 0x0F); // Asegura que solo se usen 4 bits
}

void set_orientation(uint8_t data){
  send_cmd(MADCTL);
  send_data(data);
}

void set_window(){
  send_cmd(CASET);
  send_data8()
}


// Secuencia de inicialización optimizada
void INIT() {
    RESET();
    
    // Secuencia de comandos crítica
    send_cmd(SLPOUT);
    vTaskDelay(pdMS_TO_TICKS(120));

    send_cmd(COLMOD);
    send_data8(COLOR_65K);
    vTaskDelay(pdMS_TO_TICKS(120));
    
    set_orientatation(0x08); // Orientación MX=0, MV=1 (modo vertical)

    // Configuración de timings
    porch_control(0x01, 0x01, false, 0x01, 0x33, 0x33);
    send_cmd(GCTRL);  
    send_data8(0x35);

    //configuración de VCOMS
    send_cmd(VCOMS);
    send_data8(0x20);

    send_cmd(INVOFF); // Inversión de colores OFF
    vTaskDelay(pdMS_TO_TICKS(10));

    send_cmd(DISPON);
    vTaskDelay(pdMS_TO_TICKS(150));
}

// Inicialización SPI optimizada con catch error
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
