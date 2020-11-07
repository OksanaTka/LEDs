#include <xc.h>
#pragma config JTAGEN = OFF
#pragma config FWDTEN = OFF
#pragma config FNOSC = FRCPLL
#pragma config FSOSCEN = OFF
#pragma config POSCMOD = EC
#pragma config OSCIOFNC = ON
#pragma config FPBDIV = DIV_1
#pragma config FPLLIDIV = DIV_2
#pragma config FPLLMUL = MUL_20
#pragma config FPLLODIV = DIV_1

void main() {

    TRISFbits.TRISF3 = 1; // RF3 (SW0) configured as input
    TRISFbits.TRISF5 = 1; // RF5 (SW1) configured as input
    TRISFbits.TRISF4 = 1; // RF4 (SW2) configured as input
    TRISDbits.TRISD15 = 1; // RD15 (SW3) configured as input
    TRISDbits.TRISD14 = 1; // RD14 (SW4) configured as input
    TRISBbits.TRISB11 = 1; // RB11 (SW5) configured as input
    ANSELBbits.ANSB11 = 0; // RB11 (SW5) disabled analog
    TRISBbits.TRISB10 = 1; // RB10 (SW6) configured as input
    ANSELBbits.ANSB10 = 0; // RB10 (SW6) disabled analog
    TRISBbits.TRISB9 = 1; // RB9 (SW7) configured as input
    ANSELBbits.ANSB9 = 0; // RB9 (SW7) disabled analog
    TRISBbits.TRISB14 = 0;
    ANSELBbits.ANSB14 = 0;

    int i;
    int j = 1;
    int* x = 8;
    int* y = 16;
    int last = -1;
    int speed = 0;
    TRISA &= 0xff00;
    PORTA = 0;

    while (1) {

        if (PORTBbits.RB10) { // RB10 (SW6)
            PORTBbits.RB14 = 0;
            for (i = 0; i < 1000; i++);
            PORTBbits.RB14 = 1;
        }

        if (speed == 0)
            for (i = 0; i < 200000; i++);
        else
            for (i = 0; i < 64000; i++);

        if (PORTBbits.RB9) { // RB9 (SW7)
            PORTA = 0;
            break;
        }

        if (PORTBbits.RB11) { // RB11 (SW5)
            continue;
        }
        if (PORTDbits.RD14) { // RD14 (SW4)
            speed = 1;
        } else {
            speed = 0;
        }

        if (PORTDbits.RD15) { // RD15 (SW3)
            if (last == 0)
                func_SW0(-1);
            if (last == 1)
                func_SW1(&j, -1);
            if (last == 2) {
                func_SW2(&x, &y, -1);
            }

            continue;
        }

        if (PORTFbits.RF3) { // RF3 (SW0) configured as input 
            last = 0;
            func_SW0(last);
            continue;
        }
        if (PORTFbits.RF5) { // RF5 (SW1)
            last = 1;
            func_SW1(&j, last);
            continue;

        }
        if (PORTFbits.RF4) { // RF4 (SW2)
            last = 2;
            func_SW2(&x, &y, last);

            continue;

        } else {
            PORTA = 0;
            j = 1;
            x = 8;
            y = 16;
        }

    }
}

void func_SW0(int last) {
    if (last == 0)
        PORTA++;
    else
        PORTA--;
}

void func_SW1(int* j, int last) {
    if (last == 1) {
        PORTA = (*j);
        (*j) = (*j) << 1;
        if ((*j) == 256)
            (*j) = 1;
    } else {
        (*j) = (*j) >> 1;
        PORTA = (*j);
        if ((*j) == 0)
            (*j) = 256;
    }

}

void func_SW2(int* x, int* y, int last) {
    if (last == 2) {
        PORTA = (*x) + (*y);
        (*x) = (*x) >> 1;
        (*y) = (*y) << 1;
        if ((*x) == 0 && (*y) == 256) {
            (*x) = 8;
            (*y) = 16;
        }
    } else {
        if ((*x) == 16 && (*y) == 8) {
            (*x) = (*x) >> 4;
            (*y) = (*y) << 4;
        }
        PORTA = (*x) + (*y);
        (*x) = (*x) << 1;
        (*y) = (*y) >> 1;
    }



}
