#define F_CPU 16000000UL  
#include <avr/io.h>  
#include <util/delay.h>
#include <avr/interrupt.h>

// Banderas
char SUBIR = 0;
char BAJAR = 0;
char SUMAR = 0;
char RESTAR = 0;
char ENTER = 0;

// Variables
uint8_t unidades = 0;
uint8_t decenas = 0;
uint8_t guardadou = 0;
uint8_t guardadod = 0;
uint8_t mate = 0; // 1 = resta, 2 = suma
int resultadou = 0;
int resultadod = 0;

void config_PCI(void) {
 PCICR |= 0x02;//habilitar en puertoc

PCMSK1 |= 0x01; //pinc0
DDRC &= ~0x01;  
PORTC |= 0x01;  

PCMSK1 |= 0x02; //pin c1
DDRC &= ~0x02;
PORTC |= 0x02;

PCMSK1 |= 0x04; //pinc2
DDRC &= ~0x04;
PORTC |= 0x04;

PCMSK1 |= 0x08; //pinc3
DDRC &= ~0x08;
PORTC |= 0x08;

PCMSK1 |= 0x10; //pinc4
DDRC &= ~0x10;
PORTC |= 0x10;    
}

ISR(PCINT1_vect) {
    _delay_ms(100); // antirrebote

    if (!(PINC & 0x01)) {     
        SUBIR = 1;
        BAJAR = SUMAR = RESTAR = ENTER = 0;
    }
    else if (!(PINC & 0x02)) { //BAJAR
        BAJAR = 1;
        SUBIR = SUMAR = RESTAR = ENTER = 0;
    }
    else if (!(PINC & 0x04)) { //RESTAR
        RESTAR = 1;
        SUBIR = BAJAR = SUMAR = ENTER = 0;
    }
    else if (!(PINC & 0x08)) { // SUMAR
        SUMAR = 1;
        SUBIR = BAJAR = RESTAR = ENTER = 0;
    }
    else if (!(PINC & 0x10)) { // ENTER
        ENTER = 1;
        SUBIR = BAJAR = SUMAR = RESTAR = 0;
    }
}

int main(void) {
    DDRD |= 0xF0; // PD4–PD7 salida (decenas)
    DDRB |= 0x0F; // PB0–PB3 salida (unidades)

    config_PCI();
    sei(); // 

    while (1) {
        if (SUBIR == 1) {
            if (!(decenas == 9 && unidades == 9)) {
                if (unidades >= 9) {
                    unidades = 0;
                    decenas++;
                } else {
                    unidades++;
                }
            }
            SUBIR = 0;
        }
        else if (BAJAR == 1) {
            if (!(unidades == 0 && decenas == 0)) {
                if (unidades == 0) {
                    unidades = 9;
                    if (decenas > 0) {
                        decenas--;
                    }
                } else {
                    unidades--;
                }
            }
            BAJAR = 0;
        }
        else if (RESTAR == 1) {
            guardadou = unidades;
            guardadod = decenas;
            unidades = 0;
            decenas = 0;
            mate = 1;
            RESTAR = 0;
        }
        else if (SUMAR == 1) {
            guardadou = unidades;
            guardadod = decenas;
            unidades = 0;
            decenas = 0;
            mate = 2;
            SUMAR = 0;
        }
        else if (ENTER == 1) {
            uint8_t finalu = unidades;
            uint8_t finald = decenas;
            
            if (mate == 1) {
                resultadou = guardadou - finalu;
                resultadod = guardadod - finald;
            }
            else if (mate == 2) {
                resultadou = guardadou + finalu;
                resultadod = guardadod + finald;
            }

            if (resultadou < 0) {
                if (resultadod > 0) {
                    resultadod--;
                    resultadou += 10;
                } else {
                    resultadod = 0;
                    resultadou = 0;
                }
            }

            if (resultadou > 9) {
                resultadou -= 10;
                resultadod++;
            }

            if (resultadod > 9) resultadod = 9;

            unidades = resultadou;
            decenas = resultadod;
            mate = 0;
            ENTER = 0;
        }

        // Mostrar valores en display
        PORTD = (PORTD & 0x0F) | (decenas << 4);
        PORTB = (PORTB & 0xF0) | (unidades & 0x0F);
    }
}
