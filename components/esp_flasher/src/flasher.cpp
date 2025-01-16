#include <stdio.h>

#include <iomanip>
#include <cstring>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "flasher.hpp"
#include "esp32_port.h"

// Start Address of each image
#define BOOTLOADER_START_ADDR CONFIG_BOOTLOADER_ADDR_START
#define PARTITION_START_ADDR  CONFIG_PARTITION_TABLE_ADDR_START
#define APP_START_ADDR        CONFIG_APP_ADDR_START

#define FLASH_BAUDRATE CONFIG_FLASH_BAUDRATE
#define PAYLOAD_SIZE 1024
#define BUFFER_LEN 120

void uart_target_output(void* arg);

namespace flash
{
    esp_loader_error_t Flasher::initialize()
    {
        loader_esp32_config_t esp32_cfg = {
            .baud_rate = 115200,
            .uart_port = UART_NUM_1,
            .uart_rx_pin = CONFIG_RX_GPIO,
            .uart_tx_pin = CONFIG_TX_GPIO,
            .reset_trigger_pin = CONFIG_RESET_GPIO,
            .gpio0_trigger_pin = CONFIG_BOOTMODE_GPIO,
            .uart_queue = nullptr
        };

        return loader_port_esp32_init(&esp32_cfg);
    }

    void Flasher::deinitialize()
    {
        loader_port_esp32_deinit();
    }

    esp_loader_error_t Flasher::connect_device(esp_loader_connect_args_t connect_args)
    {

        printf("Trying to Enter Boot mode on target device and Erase Device\n\n");

        auto err = esp_loader_connect(&connect_args);

        if (err == ESP_LOADER_ERROR_TIMEOUT)
        {
            printf("ERRROR: Target Device timed out: CHECK PIN CONNECTION\n");
            return err;
        }
        else if (err == ESP_LOADER_ERROR_INVALID_RESPONSE)
        {
            printf("ERROR: Internal Error occurred\n");
            return err;
        }

        /*
        std::cout << "Changing Transmission Rate of device from default 115200 to " << FLASH_BAUDRATE << std::endl;

        std:: cout << esp_loader_get_target() << std::endl;
        
        err = esp_loader_change_transmission_rate(FLASH_BAUDRATE);

        if (err == ESP_LOADER_ERROR_TIMEOUT)
        {
            std::cout << "ERROR: Device Timed Out" << std::endl;
        }
        else if (err == ESP_LOADER_ERROR_INVALID_RESPONSE)
        {
            std::cout << "ERROR: Invalid Response" << std::endl;
        }
        else if (err == ESP_LOADER_ERROR_UNSUPPORTED_FUNC)
        {
            std::cout << "ERROR: Either the target is running in secure download mode or the stub is running on the target" << std::endl;
        }
        */
        return ESP_LOADER_SUCCESS;
    }

    esp_loader_error_t Flasher::flash_device(FlashData& install_data)
    {
        RETURN_ON_ERROR(this->flash_bootloader(install_data.bootloader.data, install_data.bootloader.size));
        RETURN_ON_ERROR(this->flash_partition(install_data.partition.data, install_data.partition.size));
        RETURN_ON_ERROR(this->flash_app(install_data.app.data, install_data.app.size));

        printf("Restarting Device\n");
        loader_port_reset_target();

        return ESP_LOADER_SUCCESS;
    }

    /*PRIVATE FUNCTIONS */
    esp_loader_error_t Flasher::flash_data(const uint8_t data[], size_t size, size_t addr)
    {
        static uint8_t payload[PAYLOAD_SIZE];

        auto err = esp_loader_flash_start(addr, size, PAYLOAD_SIZE);

        // Data must be 4 byte aligned
        if (err == ESP_LOADER_ERROR_INVALID_PARAM)
        {
            printf("ERROR: Data size must be 4 byte aligned\n");
            return err;
        }
        else if(err != ESP_LOADER_SUCCESS)
        {
            printf("ERROR: ERROR Type %d\n", err);
        }

        // Writing to flash

        size_t written_size = 0;

        while (written_size != size)
        {
            size_t count = PAYLOAD_SIZE > size - written_size ? size - written_size : PAYLOAD_SIZE;

            memcpy(payload, data, count);

            data += count;

            written_size += count;

            auto err = esp_loader_flash_write(payload, count);

            if (err != ESP_LOADER_SUCCESS)
            {
                printf("Error: Flashing Issue: # %d\n", err);
                return err;
            }

            float percentage = 100 * ((float) written_size / size);

            printf("\rInstalling: %.2f%%", percentage);
        }
        printf("\nINSTALL FINISHED\n\n");
        
        return ESP_LOADER_SUCCESS;
    }

    esp_loader_error_t Flasher::flash_bootloader(const uint8_t data[], size_t size)
    {
        printf("FLASHING BOOTLOADER\n");
        return this->flash_data(data, size, BOOTLOADER_START_ADDR);
    }

    esp_loader_error_t Flasher::flash_partition(const uint8_t data[], size_t size)
    {
        printf("FLASHING PARTITION TABLE\n");
        return this->flash_data(data, size, PARTITION_START_ADDR);
    }

    esp_loader_error_t Flasher::flash_app(const uint8_t data[], size_t size)
    {
        printf("FLASHING APP\n");
        return this->flash_data(data, size, APP_START_ADDR);
    }

    void Flasher::monitor_target()
    {
        xTaskCreate(
            uart_target_output,
            "Uart Target Listener",
            2000,
            nullptr,
            2,
            nullptr
        );
    }
}

void uart_target_output(void* arg)
{

    uint8_t buff[BUFFER_LEN+1];
    if (FLASH_BAUDRATE != 115200)
    {
        uart_flush_input(UART_NUM_1);
        uart_set_baudrate(UART_NUM_1, 115200);
    }

    while (1)
    {
        int bytes_read = uart_read_bytes(UART_NUM_1, buff, BUFFER_LEN, portMAX_DELAY);
        buff[bytes_read + 1] = '\0';
        printf("%s", buff);
    }
}

