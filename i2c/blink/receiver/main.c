#include <avr/io.h>

/*
 * Receive a data byte through I2C (TWI; pins SCL and SDA) and blink
 * a LED on PB0 X times, where X is the value of the data byte.
 *
 * The byte can be sent from Raspberry pi using e.g. 'i2cset -y 1 3 4'.
 * Be sure to adjust for the voltage difference (3.3V Raspi, 5V AVR) using
 * a MOSFET level shifter:
 * http://elinux.org/RPi_GPIO_Interface_Circuits#Level_Shifters
 *
 * Alternatively, the data byte can be sent from a different MCU
 * using the transmitter program in ../transmitter.
 *
 * Blinking is adjusted for 8MHz clock.
 */

void delay(unsigned int num)
{
	unsigned int i,j;
	for (j = 0; j < 1000; j++)
		for (i = 0; i < num; i++);
}

void blink(void)
{
	PORTB = 0;
	delay(100);
	PORTB = 1 << PB0;
	delay(100);
	PORTB = 0;
}

void slave_receive(void)
{
	unsigned char n;

	// Set our slave address to 3 (LSB is general call flag)
	TWAR = 3 << 1;
	TWCR = (1 << TWEN) | (1 << TWEA);
	// wait for transmission
	while (!(TWCR & (1 << TWINT)));
	// verify that we were addressed
	if (TWSR != 0x60)
		return;
	// send ACK
	TWCR |= (1 << TWINT) | (1 << TWEA);
	// wait for data byte
	while (!(TWCR & (1 << TWINT)));
	// verify byte received
	if (TWSR != 0x80)
		return;
	n = TWDR;
	// send ACK
	TWCR |= (1 << TWINT) | (1 << TWEA);
	// wait for STOP
	while (!(TWCR & (1 << TWINT)));
	// verify that STOP was sent
	if (TWSR != 0xa0)
		return;
	TWCR |= (1 << TWINT) | (1 << TWEA);
	for (; n > 0; --n)
		blink();
}

int main(void)
{
	DDRB = 1 << PB0;
	PORTB = 0;
	while (1)
		slave_receive();
	return 0;
}
