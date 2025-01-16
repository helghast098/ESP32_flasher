#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "flasher.hpp"


extern const uint8_t bootloader_data_start[] asm("_binary_bootloader_bin_start");
extern const uint8_t bootloader_data_end[] asm("_binary_bootloader_bin_end");

extern const uint8_t partition_data_start[] asm("_binary_partition_table_bin_start");
extern const uint8_t partition_data_end[] asm("_binary_partition_table_bin_end");

extern const uint8_t app_data_start[] asm("_binary_app_bin_start");
extern const uint8_t app_data_end[] asm("_binary_app_bin_end");



extern "C" void app_main(void) {
    flash::Flasher flash_control;

    size_t bootloader_size = bootloader_data_end - bootloader_data_start;
    size_t partition_size = partition_data_end - partition_data_start;
    size_t app_size = app_data_end - app_data_start;

    flash::Flasher::BinData bootloader_data(bootloader_data_start, bootloader_size);
    flash::Flasher::BinData partition_data(partition_data_start, partition_size);
    flash::Flasher::BinData app_data(app_data_start, app_size);

    flash::Flasher::FlashData data_to_flash(bootloader_data, partition_data, app_data);

    flash_control.initialize();
    flash_control.connect_device();
    flash_control.flash_device(data_to_flash);
    flash_control.monitor_target();
}
