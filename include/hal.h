#ifndef __HAL_H_
#define __HAL_H_
#include <stdint.h>

void initializeHal(void);

void generateInterrupt(uint8_t n); /* generate x86 interrupt */

#endif
