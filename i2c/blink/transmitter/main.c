#include <avr/io.h>

/*
 * Send a data byte through I2C (TWI).
 * The TWI is polled.
 * PB0 is set high after a successful transmission.
 */

void master_transmit(void)
{
	const unsigned char status_mask = 0xff & (~(1 | 2 | 4));

	// Set bit rate
	TWBR = 8;
	TWSR = 1;

	// Send a START condition
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	// Wait for it to be transmitted
	while (!(TWCR & (1 << TWINT)));
	if ((TWSR & status_mask) != 0x08)
		return;
	// Send SLA+W (slave address and write flag)
	TWDR = (3 << 1);
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));
	if ((TWSR & status_mask) != 0x18)
		return;
	// Send data byte
	TWDR = 5;
	TWCR |= 1 << TWINT;
	while (!(TWCR & (1 << TWINT)));
	if ((TWSR & status_mask) != 0x28)
		return;
	// Send a STOP condition
	TWCR |= (1 << TWINT) | (1 << TWSTO);

	// Turn on the LED, we're done
	PORTB = 1 << PB0;
}

int main(void)
{
	DDRB = 1 << PB0;
	PORTB = 0;
	master_transmit();
	while (1);
	return 0;
}
