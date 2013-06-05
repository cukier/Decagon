#include<18f4520.h>
#include<stdlib.h>

#FUSES H4

#use delay(clock=32MHz, crystal=8MHz)
//#USE RS232(baud=1200, INVERT, xmit=PIN_C1, STREAM=RSOUT)
#USE RS232(baud=9600, xmit=PIN_C6, rcv=PIN_C7)

#define buffer_size 512

static short ctrl, print;
static int rcv[buffer_size], contBuff, aux[buffer_size], contAux, nrAchados;
static int t0Setup = T0_INTERNAL | T0_DIV_64 | 0x80;
static float nrs[buffer_size >> 4];
int *p;

#int_rda
void serial_isr() {
	clear_interrupt(INT_RDA);
	rcv[contBuff++] = getc();
	setup_timer_0(t0Setup);
}

#INT_TIMER0
void timer0_isr() {
	clear_interrupt(INT_TIMER0);
	print = 1;
	setup_timer_0(T0_OFF);
	set_timer0(0);
	rcv[contBuff] = '\0';
	contBuff = 0;
}

int main(void) {
	clear_interrupt(INT_RDA);
	enable_interrupts(INT_RDA);
	clear_interrupt(INT_TIMER0);
	enable_interrupts(INT_TIMER0);
	enable_interrupts(GLOBAL);
	printf("Hello");
	while (TRUE) {
		if (!input(PIN_A0)) {
			delay_ms(200);
			if (!input(PIN_A0) && ctrl) {
				ctrl = 0;
			}
		} else if (!ctrl) {
			delay_ms(200);
			ctrl = 1;
		}
		if (print) {
			print = 0;
			for (p = rcv; *p != '\0'; p++) {
				if ((*p == '-') || (*p >= 48 && *p <= 57) || (*p == '.')) {
					aux[contAux++] = *p;
				} else if (contAux) {
					aux[contAux] = '\0';
					nrs[nrAchados++] = atof(aux);
				}
			}
			printf("\f");
			printf("%d numeros:\n", nrAchados);
//			for (contAux = 0; contAux < nrAchados; contAux++)
//				printf("%.2f ", nrs[contAux]);
			contAux = 0;
			nrAchados = 0;
			nrs[0] = '\0';
		}
	}
	return 0;
}

