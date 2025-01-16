#ifndef FLASH_HPP
#define FLASH_HPP

#include "esp_loader_io.h"
#include <cstddef>

namespace flash
{

    class Flasher
    {

    private:
        esp_loader_error_t flash_app(const uint8_t data[], size_t size);
        esp_loader_error_t flash_bootloader(const uint8_t data[], size_t size);
        esp_loader_error_t flash_partition(const uint8_t data[], size_t size);
        esp_loader_error_t flash_data(const uint8_t data[], size_t size, size_t addr);

    public:

        // Attributes of the data to be flashed
        struct BinData
        {
            BinData(const uint8_t *data_, size_t size_): data(data_), size(size_) {}
            const uint8_t *data; // pointer to c style array
            const size_t size; // size in bytes
        };


        struct FlashData
        {
            FlashData(BinData b, BinData p, BinData a): partition(p), bootloader(b), app(a) {}
            BinData partition; // partition-table image
            BinData bootloader; // bootloader image
            BinData app; // app image
        };

        /**
         * @brief initializes the GPIO and UART
         * 
         * @return
         *      - ESP_LOADER_SUCCESS success
         *      - ESP_LOADER_ERROR_FAIL failure
         */
        esp_loader_error_t initialize();

        /**
         * @brief deinitializes the flasher by uninstalling uart driver
         * 
         * @return
         */
        void deinitialize();
        
        /**
         * @brief connects to target microcontroller by resetting and causing it to enter boot mode
         *  
         * @param connect_args -
         *          {
         *              .sync_timeout = 100 : time to wait for response of target
         *              .trials = 10 : number of times to retry connection
         *          }
         *
         * @return
         *      - ESP_LOADER_SUCCESS success
         *      - ESP_LOADER_ERROR_TIMEOUT device timeout
         *      - ESP_LOADER_ERROR_INVALID_RESPONSE internal error
         */
        esp_loader_error_t connect_device(esp_loader_connect_args_t connect_args = ESP_LOADER_CONNECT_DEFAULT());

        /**
         * @brief flashes the bootloader partition-table and app images
         * 
         * @note if baudrate set to high it will cause ESP_LOADER_ERROR_TIMEOUT
         * 
         * @param install_data - contains data of bootloader partition and app
         * 
         * @return
         *      - ESP_LOADER_SUCCESS success
         *      - ESP_LOADER_INVALID_PARAM size and address of image must be 4 byte aligned
         *      - ESP_LOADER_ERROR_TIMEOUT device timeout
         */
        esp_loader_error_t flash_device(FlashData& install_data);

        /**
         * @brief monitors the target device output after flash
         * 
         * @return
         */
        void monitor_target();

    };
}
#endif
