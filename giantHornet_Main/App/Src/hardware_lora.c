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

    SX1278_init(&SX1278, LORA_FREQ, SX1278_POWER_11DBM, SX1278_LORA_SF_7, SX1278_LORA_BW_125KHZ, SX1278_LORA_CR_4_5, SX1278_LORA_CRC_EN, LORA_PACKET_LEN);

    if(master_enable) {
        ret = SX1278_LoRaEntryTx(&SX1278, LORA_PACKET_LEN, LORA_TIMEOUT);
    } else {
        ret = SX1278_LoRaEntryRx(&SX1278, LORA_PACKET_LEN, LORA_TIMEOUT);
    }

    if(ret < 0) {
        return USER_RET_ERR_HW_FAILURE;
    } else {
        return USER_RET_OK;
    }
}

USER_StatusTypeDef lora_send(void* data, uint8_t* len) {
    int ret;
    ret = SX1278_LoRaTxPacket(&SX1278, (uint8_t*)data, (uint8_t)*len, LORA_TIMEOUT);

    if(ret < 0) {
        return USER_RET_ERR_COMMUNICATION_FAIL;
    } else {
        return USER_RET_OK;
    }
}

USER_StatusTypeDef lora_recv(void* data, uint8_t* len) {
    int ret;
    ret = SX1278_LoRaRxPacket(&SX1278);

    if(ret < 0) {
        *len = ret;
        return USER_RET_ERR_COMMUNICATION_FAIL;
    } else {
        SX1278_read(&SX1278, (uint8_t*)data, ret);
        *len = ret;
        return USER_RET_OK;
    }
}
