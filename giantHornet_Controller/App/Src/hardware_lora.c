#include "hardware_lora.h"
#include "gpio.h"
#include "spi.h"

SX1278_hw_t SX1278_hw;
SX1278_t SX1278;

USER_StatusTypeDef lora_init(uint8_t master_enable) {

	int ret;

	SX1278_hw.reset.pin = LORA_RST_Pin;
	SX1278_hw.reset.port = LORA_RST_GPIO_Port;
	SX1278_hw.dio0.pin = LORA_DIO0_Pin;
	SX1278_hw.dio0.port = LORA_DIO0_GPIO_Port;
	SX1278_hw.nss.pin = LORA_NSS_Pin;
	SX1278_hw.nss.port = LORA_NSS_GPIO_Port;
	SX1278_hw.spi = &hspi2;

	SX1278.hw = &SX1278_hw;

	SX1278_init(&SX1278, 434000000, SX1278_POWER_17DBM, SX1278_LORA_SF_7, SX1278_LORA_BW_125KHZ, SX1278_LORA_CR_4_5, SX1278_LORA_CRC_EN, 10);

	if(master_enable) {
		ret = SX1278_LoRaEntryTx(&SX1278, 16, 2000);
	} else {
		ret = SX1278_LoRaEntryRx(&SX1278, 16, 2000);
	}

	if(ret == 0) {
		return USER_RET_ERR_HW_FAILURE;
	} else {
		return USER_RET_OK;
	}
}

USER_StatusTypeDef lora_send(void* data, size_t* len) {
    return USER_RET_OK;
}

USER_StatusTypeDef lora_recv(void* data, size_t* len) {
    return USER_RET_OK;
}
