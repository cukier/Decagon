#include<18f4520.h>
#include<stdlib.h>

#FUSES H4

#use delay(clock=32MHz, crystal=8MHz)
//#USE RS232(baud=1200, INVERT, xmit=PIN_C1, STREAM=RSOUT)
#USE RS232(baud=1200, xmit=PIN_C6, rcv=PIN_C7)

static short ctrl, print;
static int rcv[100], cmd[10], contBuff, kpa[6];
static int t0Setup = T0_INTERNAL | T0_DIV_64 | 0x80;
static signed int press;

#int_rda
void serial_isr() {
	clear_interrupt(INT_RDA);
	rcv[contBuff++] = getc();
	setup_timer_0(t0Setup);
	set_timer0(0);
	clear_interrupt(INT_TIMER0);
}

#INT_TIMER0
void timer0_isr() {
	clear_interrupt(INT_TIMER0);
	print = 1;
	setup_timer_0(T0_OFF);
}

int *extrair(int *to, int *from, int start, int end) {
	int *s, *e, *p;
	for (s = from; *s != start; s++)
		;
	for (e = s; *e != end; e++)
		;
	for (p = s; p != e; p++, to++)
		*to = *p;
	return to;
}

void send_break(int *s) {
	output_high(PIN_C1);
	delay_ms(12);
	output_low(PIN_C1);
	printf("%s", s);
//	delay_ms(1000);
//	printf("\r\n");
//	delay_ms(10);
}

void addressQuery(void) {
	strcpy(cmd, "?!");
	send_break(cmd);
}

void acknowledgeActive(void) {
	strcpy(cmd, "0!");
	send_break(cmd);
}

void sendIdentification(void) {
	strcpy(cmd, "0I!");
	send_break(cmd);
}

void startMeasurement(void) {
	strcpy(cmd, "0M!");
	send_break(cmd);
}

void startMeasurementRequestCRC(void) {
	strcpy(cmd, "0MC!");
	send_break(cmd);
}

void sendData(void) {
	strcpy(cmd, "0D0!");
	send_break(cmd);
}

int main(void) {
	clear_interrupt(INT_RDA);
	enable_interrupts(INT_RDA);
	clear_interrupt(INT_TIMER0);
	enable_interrupts(INT_TIMER0);
	enable_interrupts(GLOBAL);
	output_low(PIN_D2);
	while (TRUE) {
		if (!input(PIN_A0)) {
			delay_ms(200);
			if (!input(PIN_A0) && ctrl) {
				ctrl = 0;
				output_high(PIN_D2);
			}
		} else if (!ctrl) {
			delay_ms(200);
			ctrl = 1;
		}
		if (print) {
			print = 0;
			printf("%s", rcv);
			contBuff = 0;
			output_low(PIN_D2);
		}
	}
	return 0;
}

