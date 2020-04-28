/*
 * File:   main.c
 * Author: marczis
 *
 */

#include <xc.h>

//Config
#define F_CPU 1600000UL
#include <util/delay.h>
//
//FUSES = {
//	0xFF, // LOW{SUT_CKSEL=EXTXOSC_8MHZ_XX_16KCK_65MS, CKOUT=CLEAR, CKDIV8=CLEAR}
//	0x98, // HIGH{BOOTRST=SET, BOOTSZ=2048W_3800, EESAVE=CLEAR, WDTON=CLEAR, SPIEN=SET, JTAGEN=SET, OCDEN=CLEAR}
//	0xCB, // EXTENDED{BODLEVEL=2V6, HWBE=CLEAR}
//};
//
//LOCKBITS = {
//	0xEF, // LOCKBIT{LB=NO_LOCK, BLB0=NO_LOCK, BLB1=SPM_DISABLE}
//};
//

unsigned char ap = 0, cnt = 0;

//TIFR = _BV(TOV0); - clear T0 OF IR flag
void __interrupt(TIMER0_OVF_vect_num) timer0isr(void) //CCI syntax
{
    //Free running timer - we use this for Random number generation
    ++cnt;
    if (cnt == 255) {
        ++ap;
        if (ap == 5) ap = 0;
        cnt = 0;
    }
    return;
}

    /*
     Board pins
     * D0-D7 - Vcc for Leds - X
     * D8-D13, a0, a1 - GND for Leds - Y
     * 
     * Mapping to Ports
     * D0      - PD2 xx
     * D1      - PD3 xx
     * D2      - PD1 xx
     * D3      - PD0 xx
     * D4/A6   - PD4 xx
     * D5      - PC6 xx
     * D6/A7   - PD7 xx
     * D7      - PE6 xx
     * 
     * D8      - PB4 xa
     * D9/A8   - PB5 xa
     * D10     - PB6 xa
     * D11     - PB7 xa
     * D12/A10 - PD6 xa
     * D13     - PC7 x
     * 
     * A0 - PF7 o
     * A1 - PF6 o
     * A2 - PF5 -
     * A3 - PF4 -
     * A4 - PF1 -
     * A5 - PF0 -
     * 
     * PD5 - LED on 0
     * PB0 - LED on 0
     */
void clearAll(void) {
    //SET all in the group 0
    PORTC &= 0b10111111; //PC6 to 0 rest unchanged
    PORTD &= 0b01100000; //PD7,4,3,2,1,0 to 0 rest unchanged
    PORTE &= 0b10111111; //PE6 to 0 rest unchanged
}

void clearX(unsigned char n)
{
    //SET all in the group 0
    //PORTC &= 0b10111111; //PC6 to 0 rest unchanged
    //PORTD &= 0b01100000; //PD7,4,3,2,1,0 to 0 rest unchanged
    //PORTE &= 0b10111111; //PE6 to 0 rest unchanged
    //return;
    switch (n) {
        case 0: //PD2 high, rest low
            PORTD &= 0b11111011; 
            break;
        case 1: //PD3
            PORTD &= 0b11110111;
            break;
        case 2: //PD1
            PORTD &= 0b11111101;
            break;
        case 3: //PD0
            PORTD &= 0b11111110;
            break;
        case 4: //PD4
            PORTD &= 0b11101111;
            break;
        case 5: //PC6
            PORTC &= 0b10111111;
            break;
        case 6: //PD7
            PORTD &= 0b01111111;
            break;
        case 7: //PE6
            PORTE &= 0b10111111;
            break;
    }
}

void setX(unsigned char n)
{
    //SET all in the group 0
    //PORTC &= 0b10111111; //PC6 to 0 rest unchanged
    //PORTD &= 0b01100000; //PD7,4,3,2,1,0 to 0 rest unchanged
    //PORTE &= 0b10111111; //PE6 to 0 rest unchanged
    //return;
    switch (n) {
        case 0: //PD2 high, rest low
            PORTD |= 0b00000100;
            break;
        case 1: //PD3
            PORTD |= 0b00001000;
            break;
        case 2: //PD1
            PORTD |= 0b00000010;
            break;
        case 3: //PD0
            PORTD |= 0b00000001;
            break;
        case 4: //PD4
            PORTD |= 0b00010000;
            break;
        case 5: //PC6
            PORTC |= 0b01000000;
            break;
        case 6: //PD7
            PORTD |= 0b10000000;
            break;
        case 7: //PE6
            PORTE |= 0b01000000;
            break;
    }
}

void setLine(unsigned char n)
{
    //Set all to 1 in the group
    PORTB &= 0b00001110; //Sets LED off
    PORTC &= 0b01111111;
    PORTD &= 0b10011111; //Sets LED off
    PORTF &= 0b00111111;
    switch (n) {
        case 0: //PB4 set to low rest unchanged
            PORTB |= 0b00010000;
            break;
        case 1: //PB5
            PORTB |= 0b00100000;
            break;
        case 2: //PB6
            PORTB |= 0b01000000;
            break;
        case 3: //PB7
            PORTB |= 0b10000000;
            break;
        case 4: //PD6
            PORTD |= 0b01000000;
            break;
        case 5: //PC7
            PORTC |= 0b10000000;
            break;
        case 6: //PF7
            PORTF |= 0b10000000;
            break;
        case 7: //PF6
            PORTF |= 0b01000000;
            break;
    }
}

void draw(const unsigned char *p)
{
    for(unsigned char l = 0; l < 8; ++l) {
        _delay_ms(25);
        clearAll();
        setLine(l);
        unsigned char mask = 0b00000001;
        for (unsigned char i = 0; i < 8; ++i) {
            if ((p[l] & mask) > 0) {
                setX(i);
            } else {
                clearX(i);
            }
            mask<<=1;
            //_delay_ms(1500);
        }
    }
}

const unsigned char pics[][8] =      
{
    
    {
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111
    },
    {
    0b00000000,
    0b00100100,
    0b00100100,
    0b00000000,
    0b10000001,
    0b01111110,
    0b00000000,
    0b00000000
    },
    {
    0b00000000,
    0b00000000,
    0b01101100,
    0b00000000,
    0b00000000,
    0b01111110,
    0b10000001,
    0b00000000
    },
    {
    0b00000000,
    0b00011000,
    0b00100100,
    0b01000010,
    0b01000010,
    0b00100100,
    0b00011000,
    0b00000000
    },
    {
    0b00000000,
    0b00000000,
    0b01100110,
    0b10011001,
    0b10000001,
    0b01000010,
    0b00100100,
    0b00011000,
    },
};

int main(void) {
    //Set ports
    DDRB = 0b11110001;
    DDRC = 0b11000000;
    DDRD = 0b11111111;
    DDRE = 0b01000000;
    DDRF = 0b11000000;
    
    //Set all off
    PORTB = 0b11110001; //Sets LED off
    PORTC = 0b10000000;
    PORTD = 0b01100000; //Sets LED off
    PORTE = 0b00000000;
    PORTF = 0b11000000;
    
    //Setup timer
    TCCR0B = 0x05; //clk/1024 for Timer TCNT0
    TIMSK0 = 0x01; // interrupt on overflow
    ei(); //Enable all interrupts
    //Main loop
    clearAll();
    while (1) {
        draw(pics[ap]);
    }
    return 0;
}