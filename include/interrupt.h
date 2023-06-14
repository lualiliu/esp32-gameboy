#ifndef INTERRUPT_H
#define INTERRUPT_H

enum {
	INTR_VBLANK  = 0x01,
	INTR_LCDSTAT = 0x02,
	INTR_TIMER   = 0x04,
	INTR_SERIAL  = 0x08,
	INTR_JOYPAD  = 0x10
};

void interrupt(unsigned int);
void interrupt_disable(void);
void interrupt_enable(void);

void interrupt_set_IF(unsigned char);
void interrupt_set_mask(unsigned char);

unsigned char interrupt_get_IF(void);
unsigned char interrupt_get_mask(void);

int interrupt_pending(void);
int interrupt_flush(void);

#endif
