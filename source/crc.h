#ifndef __CRC_H__   
#define __CRC_H__

void crc_init(void);
uint32_t crc32(const uint8_t *buf, uint32_t len);

#endif
