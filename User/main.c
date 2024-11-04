/**
 * CH32V003 74HC595 Shift Register Driver Test
 */

#include "debug.h"

/* Global define */
#define DATA_OUT GPIO_Pin_4
#define SHIFT_CLOCK GPIO_Pin_6
#define LATCH GPIO_Pin_5

// Global Variables
//char patterns[4] = {0b01010101, 0b10101010, 0b11110000, 0b00001111};
char patterns[8] = {0b00000001, 0b00000010, 0b00000100, 0b00001000, 0b00010000,
  				    0b00100000, 0b01000000, 0b10000000};
int delayTimeMillis = 50;
int numPatterns = 8;

// Function predefinitions
void shiftOutByte(char x);
void shiftRegisterClockPulse();
void shiftRegisterLatchPulse();

/**
 * Initializes GPIOs
 * This program uses three GPIOs, all set to push-pull outputs
 */
void GPIO_Toggle_INIT(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);

    GPIO_InitStructure.GPIO_Pin = DATA_OUT;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = SHIFT_CLOCK;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = LATCH;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
}

/**
 * Toggles the 'shift register clock', causing the shift register to
 * accept a new input bit.
 */
void shiftRegisterClockPulse() {
	GPIO_WriteBit(GPIOD, SHIFT_CLOCK, SET);
	GPIO_WriteBit(GPIOD, SHIFT_CLOCK, RESET);
	return;
}

/**
 * Toggles the 'shift register latch', aka storage register clock, which
 * causes all shift register values to be written to the storage register.
 */
void shiftRegisterLatchPulse() {
	GPIO_WriteBit(GPIOD, LATCH, SET);
	GPIO_WriteBit(GPIOD, LATCH, RESET);
	return;
}

/**
 * Shifts the param byte out the DATA_OUT line, one bit at a time, cycling
 * the shift register clock after each bit.
 *
 * @param byteToSend Byte that should be sent on the DATA_OUT line.
 */
void shiftOutByte(char byteToSend) {
	for(int j = 0; j < 8; j++) {
		int toggle = ((byteToSend >> j) & (0x01));

		if (toggle == 0) {
			GPIO_WriteBit(GPIOD, DATA_OUT, RESET);
		} else {
			GPIO_WriteBit(GPIOD, DATA_OUT, SET);
		}

		shiftRegisterClockPulse();
	}
	return;
}

/**
 * Set initial values for all GPIOs.
 */
void initialize() {
	GPIO_WriteBit(GPIOD, DATA_OUT, RESET);
	GPIO_WriteBit(GPIOD, SHIFT_CLOCK, RESET);
	GPIO_WriteBit(GPIOD, LATCH, RESET);
}

/**
 * Program main function.
 */
int main(void)
{
	// Boilerplate from template, pretty sure this could just be removed
	// since we're not using the UART anywhere.
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    SystemCoreClockUpdate();
    Delay_Init();
#if (SDI_PRINT == SDI_PR_OPEN)
    SDI_Printf_Enable();
#else
    USART_Printf_Init(115200);
#endif

    // Begin non-boilerplate code
    GPIO_Toggle_INIT();
    initialize();

    while(1)
    {
		for(int i = 1; i < numPatterns; i++) {
			shiftOutByte(patterns[i]);
			shiftRegisterLatchPulse();
			Delay_Ms(delayTimeMillis);
		}

		for (int i = numPatterns-2; i>=0; i--) {
			shiftOutByte(patterns[i]);
			shiftRegisterLatchPulse();
			Delay_Ms(delayTimeMillis);
		}
    }

    return 0;
}
