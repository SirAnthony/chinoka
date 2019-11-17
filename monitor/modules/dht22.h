#ifndef DHT22_H_
#define DHT22_H_

// https://github.com/LonelyWolf/stm32/blob/master/am2302/

/* DHT22_GetReadings response codes */
typedef enum {
  DHT22_RCV_OK = 0,      // Return with no error
  DHT22_RCV_NO_RESPONSE, // No response from sensor
  DHT22_RCV_BAD_ACK1,    // Bad first half length of ACK impulse
  DHT22_RCV_BAD_ACK2,    // Bad second half length of ACK impulse
  DHT22_RCV_TIMEOUT,     // It was timeout while receiving bits
  DHT22_RCV_BAD_DATA,    // Data was corrupted
  DHT22_RCV_END,
} DHT22_RCV_Stat;

void DHT_Init(void);
void DHT_Loop(uint32_t tick);

#endif /* DHT22_H_ */
