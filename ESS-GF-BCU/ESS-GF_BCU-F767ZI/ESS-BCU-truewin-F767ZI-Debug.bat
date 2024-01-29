@echo off
REM Run this file to build the project outside of the IDE.
REM WARNING: if using a different machine, copy the .rsp files together with this script.
echo stm32f4xx_STLcpurunGCC.s
c:\sysgcc\arm-eabi\bin\arm-none-eabi-g++.exe @"VisualGDB/Debug/stm32f4xx_STLcpurunGCC.gcc.rsp" || exit 1
echo stm32f4xx_STLcpustartGCC.s
c:\sysgcc\arm-eabi\bin\arm-none-eabi-g++.exe @"VisualGDB/Debug/stm32f4xx_STLcpustartGCC.gcc.rsp" || exit 1
echo stm32fxx_STLRamMcMxGCC.s
c:\sysgcc\arm-eabi\bin\arm-none-eabi-g++.exe @"VisualGDB/Debug/stm32fxx_STLRamMcMxGCC.gcc.rsp" || exit 1
echo except.S
c:\sysgcc\arm-eabi\bin\arm-none-eabi-g++.exe @"VisualGDB/Debug/except.gcc.rsp" || exit 1
echo irq_cm4f.S
c:\sysgcc\arm-eabi\bin\arm-none-eabi-g++.exe @"VisualGDB/Debug/irq_cm4f.gcc.rsp" || exit 1
echo Main-Thread-Callback.cpp
c:\sysgcc\arm-eabi\bin\arm-none-eabi-g++.exe @"VisualGDB/Debug/Main-Thread-Callback.gcc.rsp" || exit 1
echo BackupSram.cpp
c:\sysgcc\arm-eabi\bin\arm-none-eabi-g++.exe @"VisualGDB/Debug/BackupSram.gcc.rsp" || exit 1
echo Balance.cpp
c:\sysgcc\arm-eabi\bin\arm-none-eabi-g++.exe @"VisualGDB/Debug/Balance.gcc.rsp" || exit 1
echo GlobalVar.cpp
c:\sysgcc\arm-eabi\bin\arm-none-eabi-g++.exe @"VisualGDB/Debug/GlobalVar.gcc.rsp" || exit 1
echo Watchdog.cpp
c:\sysgcc\arm-eabi\bin\arm-none-eabi-g++.exe @"VisualGDB/Debug/Watchdog.gcc.rsp" || exit 1
echo BatteryMbuCan.cpp
c:\sysgcc\arm-eabi\bin\arm-none-eabi-g++.exe @"VisualGDB/Debug/BatteryMbuCan.gcc.rsp" || exit 1
echo BatterySys.cpp
c:\sysgcc\arm-eabi\bin\arm-none-eabi-g++.exe @"VisualGDB/Debug/BatterySys.gcc.rsp" || exit 1
echo BmsEngine.cpp
c:\sysgcc\arm-eabi\bin\arm-none-eabi-g++.exe @"VisualGDB/Debug/BmsEngine.gcc.rsp" || exit 1
echo BmsEventCheck.cpp
c:\sysgcc\arm-eabi\bin\arm-none-eabi-g++.exe @"VisualGDB/Debug/BmsEventCheck.gcc.rsp" || exit 1
echo DS1302.cpp
c:\sysgcc\arm-eabi\bin\arm-none-eabi-g++.exe @"VisualGDB/Debug/DS1302.gcc.rsp" || exit 1
echo CY15B256J_I2C.cpp
c:\sysgcc\arm-eabi\bin\arm-none-eabi-g++.exe @"VisualGDB/Debug/CY15B256J_I2C.gcc.rsp" || exit 1
echo SHT_driver.cpp
c:\sysgcc\arm-eabi\bin\arm-none-eabi-g++.exe @"VisualGDB/Debug/SHT_driver.gcc.rsp" || exit 1
echo Isolation.cpp
c:\sysgcc\arm-eabi\bin\arm-none-eabi-g++.exe @"VisualGDB/Debug/Isolation.gcc.rsp" || exit 1
echo ModbusMaster.cpp
c:\sysgcc\arm-eabi\bin\arm-none-eabi-g++.exe @"VisualGDB/Debug/ModbusMaster.gcc.rsp" || exit 1
echo SEGGER_RTT.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/SEGGER_RTT.gcc.rsp" || exit 1
echo SEGGER_RTT_printf.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/SEGGER_RTT_printf.gcc.rsp" || exit 1
echo SMBus_I2C.cpp
c:\sysgcc\arm-eabi\bin\arm-none-eabi-g++.exe @"VisualGDB/Debug/SMBus_I2C.gcc.rsp" || exit 1
echo SOC.cpp
c:\sysgcc\arm-eabi\bin\arm-none-eabi-g++.exe @"VisualGDB/Debug/SOC.gcc.rsp" || exit 1
echo stm32756g_eval.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32756g_eval.gcc.rsp" || exit 1
echo VersionInfo.cpp
c:\sysgcc\arm-eabi\bin\arm-none-eabi-g++.exe @"VisualGDB/Debug/VersionInfo.gcc.rsp" || exit 1
echo mbed_critical_section_api.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/mbed_critical_section_api.gcc.rsp" || exit 1
echo mbed_flash_api.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/mbed_flash_api.gcc.rsp" || exit 1
echo mbed_gpio.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/mbed_gpio.gcc.rsp" || exit 1
echo mbed_itm_api.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/mbed_itm_api.gcc.rsp" || exit 1
echo mbed_lp_ticker_api.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/mbed_lp_ticker_api.gcc.rsp" || exit 1
echo mbed_pinmap_common.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/mbed_pinmap_common.gcc.rsp" || exit 1
echo mbed_sleep_manager.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/mbed_sleep_manager.gcc.rsp" || exit 1
echo mbed_ticker_api.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/mbed_ticker_api.gcc.rsp" || exit 1
echo mbed_us_ticker_api.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/mbed_us_ticker_api.gcc.rsp" || exit 1
echo mbed_application.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/mbed_application.gcc.rsp" || exit 1
echo mbed_assert.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/mbed_assert.gcc.rsp" || exit 1
echo mbed_board.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/mbed_board.gcc.rsp" || exit 1
echo mbed_critical.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/mbed_critical.gcc.rsp" || exit 1
echo mbed_error.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/mbed_error.gcc.rsp" || exit 1
echo mbed_error_hist.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/mbed_error_hist.gcc.rsp" || exit 1
echo mbed_interface.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/mbed_interface.gcc.rsp" || exit 1
echo mbed_mktime.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/mbed_mktime.gcc.rsp" || exit 1
echo mbed_sdk_boot.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/mbed_sdk_boot.gcc.rsp" || exit 1
echo mbed_semihost_api.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/mbed_semihost_api.gcc.rsp" || exit 1
echo mbed_stats.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/mbed_stats.gcc.rsp" || exit 1
echo mbed_wait_api_no_rtos.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/mbed_wait_api_no_rtos.gcc.rsp" || exit 1
echo mbed_lp_ticker_wrapper.cpp
c:\sysgcc\arm-eabi\bin\arm-none-eabi-g++.exe @"VisualGDB/Debug/mbed_lp_ticker_wrapper.gcc.rsp" || exit 1
echo ATCmdParser.cpp
c:\sysgcc\arm-eabi\bin\arm-none-eabi-g++.exe @"VisualGDB/Debug/ATCmdParser.gcc.rsp" || exit 1
echo CallChain.cpp
c:\sysgcc\arm-eabi\bin\arm-none-eabi-g++.exe @"VisualGDB/Debug/CallChain.gcc.rsp" || exit 1
echo FileBase.cpp
c:\sysgcc\arm-eabi\bin\arm-none-eabi-g++.exe @"VisualGDB/Debug/FileBase.gcc.rsp" || exit 1
echo FileHandle.cpp
c:\sysgcc\arm-eabi\bin\arm-none-eabi-g++.exe @"VisualGDB/Debug/FileHandle.gcc.rsp" || exit 1
echo FilePath.cpp
c:\sysgcc\arm-eabi\bin\arm-none-eabi-g++.exe @"VisualGDB/Debug/FilePath.gcc.rsp" || exit 1
echo FileSystemHandle.cpp
c:\sysgcc\arm-eabi\bin\arm-none-eabi-g++.exe @"VisualGDB/Debug/FileSystemHandle.gcc.rsp" || exit 1
echo LocalFileSystem.cpp
c:\sysgcc\arm-eabi\bin\arm-none-eabi-g++.exe @"VisualGDB/Debug/LocalFileSystem.gcc.rsp" || exit 1
echo mbed_alloc_wrappers.cpp
c:\sysgcc\arm-eabi\bin\arm-none-eabi-g++.exe @"VisualGDB/Debug/mbed_alloc_wrappers.gcc.rsp" || exit 1
echo mbed_mem_trace.cpp
c:\sysgcc\arm-eabi\bin\arm-none-eabi-g++.exe @"VisualGDB/Debug/mbed_mem_trace.gcc.rsp" || exit 1
echo mbed_poll.cpp
c:\sysgcc\arm-eabi\bin\arm-none-eabi-g++.exe @"VisualGDB/Debug/mbed_poll.gcc.rsp" || exit 1
echo mbed_retarget.cpp
c:\sysgcc\arm-eabi\bin\arm-none-eabi-g++.exe @"VisualGDB/Debug/mbed_retarget.gcc.rsp" || exit 1
echo mbed_rtc_time.cpp
c:\sysgcc\arm-eabi\bin\arm-none-eabi-g++.exe @"VisualGDB/Debug/mbed_rtc_time.gcc.rsp" || exit 1
echo mbed_wait_api_rtos.cpp
c:\sysgcc\arm-eabi\bin\arm-none-eabi-g++.exe @"VisualGDB/Debug/mbed_wait_api_rtos.gcc.rsp" || exit 1
echo Stream.cpp
c:\sysgcc\arm-eabi\bin\arm-none-eabi-g++.exe @"VisualGDB/Debug/Stream.gcc.rsp" || exit 1
echo mbed_tz_context.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/mbed_tz_context.gcc.rsp" || exit 1
echo analogin_api.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/analogin_api.gcc.rsp" || exit 1
echo analogout_api.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/analogout_api.gcc.rsp" || exit 1
echo can_api.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/can_api.gcc.rsp" || exit 1
echo gpio_api.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/gpio_api.gcc.rsp" || exit 1
echo gpio_irq_api.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/gpio_irq_api.gcc.rsp" || exit 1
echo hal_tick_16b.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/hal_tick_16b.gcc.rsp" || exit 1
echo hal_tick_32b.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/hal_tick_32b.gcc.rsp" || exit 1
echo hal_tick_common.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/hal_tick_common.gcc.rsp" || exit 1
echo i2c_api.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/i2c_api.gcc.rsp" || exit 1
echo lp_ticker.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/lp_ticker.gcc.rsp" || exit 1
echo mbed_crc_api.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/mbed_crc_api.gcc.rsp" || exit 1
echo mbed_overrides.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/mbed_overrides.gcc.rsp" || exit 1
echo pinmap.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/pinmap.gcc.rsp" || exit 1
echo port_api.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/port_api.gcc.rsp" || exit 1
echo pwmout_api.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/pwmout_api.gcc.rsp" || exit 1
echo rtc_api.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/rtc_api.gcc.rsp" || exit 1
echo serial_api.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/serial_api.gcc.rsp" || exit 1
echo sleep.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/sleep.gcc.rsp" || exit 1
echo stm_spi_api.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm_spi_api.gcc.rsp" || exit 1
echo trng_api.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/trng_api.gcc.rsp" || exit 1
echo us_ticker.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/us_ticker.gcc.rsp" || exit 1
echo analogin_device.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/analogin_device.gcc.rsp" || exit 1
echo analogout_device.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/analogout_device.gcc.rsp" || exit 1
echo flash_api.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/flash_api.gcc.rsp" || exit 1
echo gpio_irq_device.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/gpio_irq_device.gcc.rsp" || exit 1
echo pwmout_device.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/pwmout_device.gcc.rsp" || exit 1
echo serial_device.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/serial_device.gcc.rsp" || exit 1
echo spi_api.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/spi_api.gcc.rsp" || exit 1
echo stm32f7xx_hal.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32f7xx_hal.gcc.rsp" || exit 1
echo stm32f7xx_hal_adc.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32f7xx_hal_adc.gcc.rsp" || exit 1
echo stm32f7xx_hal_adc_ex.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32f7xx_hal_adc_ex.gcc.rsp" || exit 1
echo stm32f7xx_hal_can.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32f7xx_hal_can.gcc.rsp" || exit 1
echo stm32f7xx_hal_can_legacy.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32f7xx_hal_can_legacy.gcc.rsp" || exit 1
echo stm32f7xx_hal_cec.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32f7xx_hal_cec.gcc.rsp" || exit 1
echo stm32f7xx_hal_cortex.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32f7xx_hal_cortex.gcc.rsp" || exit 1
echo stm32f7xx_hal_crc.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32f7xx_hal_crc.gcc.rsp" || exit 1
echo stm32f7xx_hal_crc_ex.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32f7xx_hal_crc_ex.gcc.rsp" || exit 1
echo stm32f7xx_hal_cryp.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32f7xx_hal_cryp.gcc.rsp" || exit 1
echo stm32f7xx_hal_cryp_ex.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32f7xx_hal_cryp_ex.gcc.rsp" || exit 1
echo stm32f7xx_hal_dac.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32f7xx_hal_dac.gcc.rsp" || exit 1
echo stm32f7xx_hal_dac_ex.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32f7xx_hal_dac_ex.gcc.rsp" || exit 1
echo stm32f7xx_hal_dcmi.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32f7xx_hal_dcmi.gcc.rsp" || exit 1
echo stm32f7xx_hal_dcmi_ex.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32f7xx_hal_dcmi_ex.gcc.rsp" || exit 1
echo stm32f7xx_hal_dfsdm.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32f7xx_hal_dfsdm.gcc.rsp" || exit 1
echo stm32f7xx_hal_dma.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32f7xx_hal_dma.gcc.rsp" || exit 1
echo stm32f7xx_hal_dma2d.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32f7xx_hal_dma2d.gcc.rsp" || exit 1
echo stm32f7xx_hal_dma_ex.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32f7xx_hal_dma_ex.gcc.rsp" || exit 1
echo stm32f7xx_hal_dsi.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32f7xx_hal_dsi.gcc.rsp" || exit 1
echo stm32f7xx_hal_eth.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32f7xx_hal_eth.gcc.rsp" || exit 1
echo stm32f7xx_hal_flash.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32f7xx_hal_flash.gcc.rsp" || exit 1
echo stm32f7xx_hal_flash_ex.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32f7xx_hal_flash_ex.gcc.rsp" || exit 1
echo stm32f7xx_hal_gpio.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32f7xx_hal_gpio.gcc.rsp" || exit 1
echo stm32f7xx_hal_hash.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32f7xx_hal_hash.gcc.rsp" || exit 1
echo stm32f7xx_hal_hash_ex.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32f7xx_hal_hash_ex.gcc.rsp" || exit 1
echo stm32f7xx_hal_hcd.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32f7xx_hal_hcd.gcc.rsp" || exit 1
echo stm32f7xx_hal_i2c.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32f7xx_hal_i2c.gcc.rsp" || exit 1
echo stm32f7xx_hal_i2c_ex.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32f7xx_hal_i2c_ex.gcc.rsp" || exit 1
echo stm32f7xx_hal_i2s.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32f7xx_hal_i2s.gcc.rsp" || exit 1
echo stm32f7xx_hal_irda.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32f7xx_hal_irda.gcc.rsp" || exit 1
echo stm32f7xx_hal_iwdg.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32f7xx_hal_iwdg.gcc.rsp" || exit 1
echo stm32f7xx_hal_jpeg.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32f7xx_hal_jpeg.gcc.rsp" || exit 1
echo stm32f7xx_hal_lptim.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32f7xx_hal_lptim.gcc.rsp" || exit 1
echo stm32f7xx_hal_ltdc.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32f7xx_hal_ltdc.gcc.rsp" || exit 1
echo stm32f7xx_hal_ltdc_ex.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32f7xx_hal_ltdc_ex.gcc.rsp" || exit 1
echo stm32f7xx_hal_mdios.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32f7xx_hal_mdios.gcc.rsp" || exit 1
echo stm32f7xx_hal_mmc.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32f7xx_hal_mmc.gcc.rsp" || exit 1
echo stm32f7xx_hal_nand.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32f7xx_hal_nand.gcc.rsp" || exit 1
echo stm32f7xx_hal_nor.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32f7xx_hal_nor.gcc.rsp" || exit 1
echo stm32f7xx_hal_pcd.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32f7xx_hal_pcd.gcc.rsp" || exit 1
echo stm32f7xx_hal_pcd_ex.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32f7xx_hal_pcd_ex.gcc.rsp" || exit 1
echo stm32f7xx_hal_pwr.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32f7xx_hal_pwr.gcc.rsp" || exit 1
echo stm32f7xx_hal_pwr_ex.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32f7xx_hal_pwr_ex.gcc.rsp" || exit 1
echo stm32f7xx_hal_qspi.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32f7xx_hal_qspi.gcc.rsp" || exit 1
echo stm32f7xx_hal_rcc.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32f7xx_hal_rcc.gcc.rsp" || exit 1
echo stm32f7xx_hal_rcc_ex.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32f7xx_hal_rcc_ex.gcc.rsp" || exit 1
echo stm32f7xx_hal_rng.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32f7xx_hal_rng.gcc.rsp" || exit 1
echo stm32f7xx_hal_rtc.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32f7xx_hal_rtc.gcc.rsp" || exit 1
echo stm32f7xx_hal_rtc_ex.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32f7xx_hal_rtc_ex.gcc.rsp" || exit 1
echo stm32f7xx_hal_sai.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32f7xx_hal_sai.gcc.rsp" || exit 1
echo stm32f7xx_hal_sai_ex.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32f7xx_hal_sai_ex.gcc.rsp" || exit 1
echo stm32f7xx_hal_sd.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32f7xx_hal_sd.gcc.rsp" || exit 1
echo stm32f7xx_hal_sdram.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32f7xx_hal_sdram.gcc.rsp" || exit 1
echo stm32f7xx_hal_smartcard.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32f7xx_hal_smartcard.gcc.rsp" || exit 1
echo stm32f7xx_hal_smartcard_ex.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32f7xx_hal_smartcard_ex.gcc.rsp" || exit 1
echo stm32f7xx_hal_smbus.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32f7xx_hal_smbus.gcc.rsp" || exit 1
echo stm32f7xx_hal_spdifrx.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32f7xx_hal_spdifrx.gcc.rsp" || exit 1
echo stm32f7xx_hal_spi.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32f7xx_hal_spi.gcc.rsp" || exit 1
echo stm32f7xx_hal_spi_ex.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32f7xx_hal_spi_ex.gcc.rsp" || exit 1
echo stm32f7xx_hal_sram.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32f7xx_hal_sram.gcc.rsp" || exit 1
echo stm32f7xx_hal_tim.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32f7xx_hal_tim.gcc.rsp" || exit 1
echo stm32f7xx_hal_tim_ex.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32f7xx_hal_tim_ex.gcc.rsp" || exit 1
echo stm32f7xx_hal_uart.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32f7xx_hal_uart.gcc.rsp" || exit 1
echo stm32f7xx_hal_usart.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32f7xx_hal_usart.gcc.rsp" || exit 1
echo stm32f7xx_hal_wwdg.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32f7xx_hal_wwdg.gcc.rsp" || exit 1
echo stm32f7xx_ll_adc.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32f7xx_ll_adc.gcc.rsp" || exit 1
echo stm32f7xx_ll_crc.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32f7xx_ll_crc.gcc.rsp" || exit 1
echo stm32f7xx_ll_dac.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32f7xx_ll_dac.gcc.rsp" || exit 1
echo stm32f7xx_ll_dma.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32f7xx_ll_dma.gcc.rsp" || exit 1
echo stm32f7xx_ll_dma2d.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32f7xx_ll_dma2d.gcc.rsp" || exit 1
echo stm32f7xx_ll_exti.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32f7xx_ll_exti.gcc.rsp" || exit 1
echo stm32f7xx_ll_fmc.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32f7xx_ll_fmc.gcc.rsp" || exit 1
echo stm32f7xx_ll_gpio.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32f7xx_ll_gpio.gcc.rsp" || exit 1
echo stm32f7xx_ll_i2c.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32f7xx_ll_i2c.gcc.rsp" || exit 1
echo stm32f7xx_ll_lptim.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32f7xx_ll_lptim.gcc.rsp" || exit 1
echo stm32f7xx_ll_pwr.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32f7xx_ll_pwr.gcc.rsp" || exit 1
echo stm32f7xx_ll_rcc.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32f7xx_ll_rcc.gcc.rsp" || exit 1
echo stm32f7xx_ll_rng.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32f7xx_ll_rng.gcc.rsp" || exit 1
echo stm32f7xx_ll_rtc.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32f7xx_ll_rtc.gcc.rsp" || exit 1
echo stm32f7xx_ll_sdmmc.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32f7xx_ll_sdmmc.gcc.rsp" || exit 1
echo stm32f7xx_ll_spi.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32f7xx_ll_spi.gcc.rsp" || exit 1
echo stm32f7xx_ll_tim.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32f7xx_ll_tim.gcc.rsp" || exit 1
echo stm32f7xx_ll_usart.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32f7xx_ll_usart.gcc.rsp" || exit 1
echo stm32f7xx_ll_usb.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32f7xx_ll_usb.gcc.rsp" || exit 1
echo stm32f7xx_ll_utils.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32f7xx_ll_utils.gcc.rsp" || exit 1
echo system_stm32f7xx.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/system_stm32f7xx.gcc.rsp" || exit 1
echo PeripheralPins.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/PeripheralPins.gcc.rsp" || exit 1
echo system_clock.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/system_clock.gcc.rsp" || exit 1
echo AnalogIn.cpp
c:\sysgcc\arm-eabi\bin\arm-none-eabi-g++.exe @"VisualGDB/Debug/AnalogIn.gcc.rsp" || exit 1
echo BusIn.cpp
c:\sysgcc\arm-eabi\bin\arm-none-eabi-g++.exe @"VisualGDB/Debug/BusIn.gcc.rsp" || exit 1
echo BusInOut.cpp
c:\sysgcc\arm-eabi\bin\arm-none-eabi-g++.exe @"VisualGDB/Debug/BusInOut.gcc.rsp" || exit 1
echo BusOut.cpp
c:\sysgcc\arm-eabi\bin\arm-none-eabi-g++.exe @"VisualGDB/Debug/BusOut.gcc.rsp" || exit 1
echo CAN.cpp
c:\sysgcc\arm-eabi\bin\arm-none-eabi-g++.exe @"VisualGDB/Debug/CAN.gcc.rsp" || exit 1
echo Ethernet.cpp
c:\sysgcc\arm-eabi\bin\arm-none-eabi-g++.exe @"VisualGDB/Debug/Ethernet.gcc.rsp" || exit 1
echo FlashIAP.cpp
c:\sysgcc\arm-eabi\bin\arm-none-eabi-g++.exe @"VisualGDB/Debug/FlashIAP.gcc.rsp" || exit 1
echo I2C.cpp
c:\sysgcc\arm-eabi\bin\arm-none-eabi-g++.exe @"VisualGDB/Debug/I2C.gcc.rsp" || exit 1
echo I2CSlave.cpp
c:\sysgcc\arm-eabi\bin\arm-none-eabi-g++.exe @"VisualGDB/Debug/I2CSlave.gcc.rsp" || exit 1
echo InterruptIn.cpp
c:\sysgcc\arm-eabi\bin\arm-none-eabi-g++.exe @"VisualGDB/Debug/InterruptIn.gcc.rsp" || exit 1
echo InterruptManager.cpp
c:\sysgcc\arm-eabi\bin\arm-none-eabi-g++.exe @"VisualGDB/Debug/InterruptManager.gcc.rsp" || exit 1
echo MbedCRC.cpp
c:\sysgcc\arm-eabi\bin\arm-none-eabi-g++.exe @"VisualGDB/Debug/MbedCRC.gcc.rsp" || exit 1
echo RawSerial.cpp
c:\sysgcc\arm-eabi\bin\arm-none-eabi-g++.exe @"VisualGDB/Debug/RawSerial.gcc.rsp" || exit 1
echo Serial.cpp
c:\sysgcc\arm-eabi\bin\arm-none-eabi-g++.exe @"VisualGDB/Debug/Serial.gcc.rsp" || exit 1
echo SerialBase.cpp
c:\sysgcc\arm-eabi\bin\arm-none-eabi-g++.exe @"VisualGDB/Debug/SerialBase.gcc.rsp" || exit 1
echo SPI.cpp
c:\sysgcc\arm-eabi\bin\arm-none-eabi-g++.exe @"VisualGDB/Debug/SPI.gcc.rsp" || exit 1
echo SPISlave.cpp
c:\sysgcc\arm-eabi\bin\arm-none-eabi-g++.exe @"VisualGDB/Debug/SPISlave.gcc.rsp" || exit 1
echo TableCRC.cpp
c:\sysgcc\arm-eabi\bin\arm-none-eabi-g++.exe @"VisualGDB/Debug/TableCRC.gcc.rsp" || exit 1
echo Ticker.cpp
c:\sysgcc\arm-eabi\bin\arm-none-eabi-g++.exe @"VisualGDB/Debug/Ticker.gcc.rsp" || exit 1
echo Timeout.cpp
c:\sysgcc\arm-eabi\bin\arm-none-eabi-g++.exe @"VisualGDB/Debug/Timeout.gcc.rsp" || exit 1
echo Timer.cpp
c:\sysgcc\arm-eabi\bin\arm-none-eabi-g++.exe @"VisualGDB/Debug/Timer.gcc.rsp" || exit 1
echo TimerEvent.cpp
c:\sysgcc\arm-eabi\bin\arm-none-eabi-g++.exe @"VisualGDB/Debug/TimerEvent.gcc.rsp" || exit 1
echo UARTSerial.cpp
c:\sysgcc\arm-eabi\bin\arm-none-eabi-g++.exe @"VisualGDB/Debug/UARTSerial.gcc.rsp" || exit 1
echo mbed_boot.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/mbed_boot.gcc.rsp" || exit 1
echo mbed_rtx_handlers.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/mbed_rtx_handlers.gcc.rsp" || exit 1
echo cmsis_os1.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/cmsis_os1.gcc.rsp" || exit 1
echo RTX_Config.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/RTX_Config.gcc.rsp" || exit 1
echo rtx_delay.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/rtx_delay.gcc.rsp" || exit 1
echo rtx_evflags.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/rtx_evflags.gcc.rsp" || exit 1
echo rtx_evr.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/rtx_evr.gcc.rsp" || exit 1
echo rtx_kernel.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/rtx_kernel.gcc.rsp" || exit 1
echo rtx_lib.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/rtx_lib.gcc.rsp" || exit 1
echo rtx_memory.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/rtx_memory.gcc.rsp" || exit 1
echo rtx_mempool.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/rtx_mempool.gcc.rsp" || exit 1
echo rtx_msgqueue.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/rtx_msgqueue.gcc.rsp" || exit 1
echo rtx_mutex.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/rtx_mutex.gcc.rsp" || exit 1
echo rtx_semaphore.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/rtx_semaphore.gcc.rsp" || exit 1
echo rtx_system.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/rtx_system.gcc.rsp" || exit 1
echo rtx_thread.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/rtx_thread.gcc.rsp" || exit 1
echo rtx_timer.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/rtx_timer.gcc.rsp" || exit 1
echo rt_OsEventObserver.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/rt_OsEventObserver.gcc.rsp" || exit 1
echo os_systick.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/os_systick.gcc.rsp" || exit 1
echo os_tick_ptim.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/os_tick_ptim.gcc.rsp" || exit 1
echo mbed_rtx_fault_handler.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/mbed_rtx_fault_handler.gcc.rsp" || exit 1
echo ConditionVariable.cpp
c:\sysgcc\arm-eabi\bin\arm-none-eabi-g++.exe @"VisualGDB/Debug/ConditionVariable.gcc.rsp" || exit 1
echo EventFlags.cpp
c:\sysgcc\arm-eabi\bin\arm-none-eabi-g++.exe @"VisualGDB/Debug/EventFlags.gcc.rsp" || exit 1
echo Kernel.cpp
c:\sysgcc\arm-eabi\bin\arm-none-eabi-g++.exe @"VisualGDB/Debug/Kernel.gcc.rsp" || exit 1
echo Mutex.cpp
c:\sysgcc\arm-eabi\bin\arm-none-eabi-g++.exe @"VisualGDB/Debug/Mutex.gcc.rsp" || exit 1
echo RtosTimer.cpp
c:\sysgcc\arm-eabi\bin\arm-none-eabi-g++.exe @"VisualGDB/Debug/RtosTimer.gcc.rsp" || exit 1
echo Semaphore.cpp
c:\sysgcc\arm-eabi\bin\arm-none-eabi-g++.exe @"VisualGDB/Debug/Semaphore.gcc.rsp" || exit 1
echo Thread.cpp
c:\sysgcc\arm-eabi\bin\arm-none-eabi-g++.exe @"VisualGDB/Debug/Thread.gcc.rsp" || exit 1
echo mbed_rtx_idle.cpp
c:\sysgcc\arm-eabi\bin\arm-none-eabi-g++.exe @"VisualGDB/Debug/mbed_rtx_idle.gcc.rsp" || exit 1
echo SysTimer.cpp
c:\sysgcc\arm-eabi\bin\arm-none-eabi-g++.exe @"VisualGDB/Debug/SysTimer.gcc.rsp" || exit 1
echo functionalsafety.cpp
c:\sysgcc\arm-eabi\bin\arm-none-eabi-g++.exe @"VisualGDB/Debug/functionalsafety.gcc.rsp" || exit 1
echo main1.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/main1.gcc.rsp" || exit 1
echo stm32f7xx_hal_msp.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32f7xx_hal_msp.gcc.rsp" || exit 1
echo stm32f7xx_it.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32f7xx_it.gcc.rsp" || exit 1
echo stm32fxx_STLclockrun.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32fxx_STLclockrun.gcc.rsp" || exit 1
echo stm32fxx_STLclockstart.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32fxx_STLclockstart.gcc.rsp" || exit 1
echo stm32fxx_STLcrc32Run.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32fxx_STLcrc32Run.gcc.rsp" || exit 1
echo stm32fxx_STLmain.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32fxx_STLmain.gcc.rsp" || exit 1
echo stm32fxx_STLstartup.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32fxx_STLstartup.gcc.rsp" || exit 1
echo stm32fxx_STLtranspRam.c
c:\sysgcc\arm-eabi\bin\arm-none-eabi-gcc.exe @"VisualGDB/Debug/stm32fxx_STLtranspRam.gcc.rsp" || exit 1
echo startup_stm32f767xx.S
c:\sysgcc\arm-eabi\bin\arm-none-eabi-g++.exe @"VisualGDB/Debug/startup_stm32f767xx.gcc.rsp" || exit 1
