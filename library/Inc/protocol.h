extern uint8_t calculate_checksum(uint8_t *data, uint8_t length);
extern void create_packet_log(packet_log_t *packet, uint8_t type, uint8_t action, uint8_t *data, uint8_t length);
extern void create_packet_Tx(packet_log_t *packet, uint8_t type, uint8_t action, uint8_t *data, uint8_t length);