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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void busy(void);
void screenOn(char* str);

void speedUp();

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

    TRISBbits.TRISB15 = 0; // RB15 (DISP_RS) set as an output
    ANSELBbits.ANSB15 = 0; // disable analog functionality on RB15 (DISP_RS)
    TRISDbits.TRISD5 = 0; // RD5 (DISP_RW) set as an output
    TRISDbits.TRISD4 = 0; // RD4 (DISP_EN) set as an output
    //TRISEbits.TRISE0 = 1; // RE0 (DB0) set as input (change 1 to 0 for
    TRISE &= 0xff00;
    ANSELEbits.ANSE2 = 0;
    ANSELEbits.ANSE4 = 0;
    ANSELEbits.ANSE5 = 0;
    ANSELEbits.ANSE6 = 0;
    PORTBbits.RB15 = 0; //rs=0
    PORTDbits.RD5 = 0; //w=0
    ANSELEbits.ANSE7 = 0;

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
            int u = 0;
            int i = 0;
            screenOn("Mode 6:");
            clear();
            screenDown("Beep Mode");
            for (i = 0; i < 500; i++) {
                PORTBbits.RB14 = 1;
                for (u = 0; u < 400; u++);
                PORTBbits.RB14 = 0;
                for (u = 0; u < 400; u++);
            }

        }

        // determines the speed of the lights
        if (speed == 0)
            for (i = 0; i < 200000; i++);
        else {
            for (i = 0; i < 64000; i++);
        }

        // turn of the lights and exit from the program 
        if (PORTBbits.RB9) { // RB9 (SW7)
            screenOn("Mode 7:");
            clear();
            screenDown("Exit");
            PORTA = 0;
            break;
        }
        //stops the lights in current state
        if (PORTBbits.RB11) { // RB11 (SW5)
            screenOn("Mode 5:");
            clear();
            screenDown("Halt");
            continue;
        }

        // changes the speed of the lights
        if (PORTDbits.RD14) { // RD14 (SW4)
            speed = 1; //speed up the lights
        } else {
            speed = 0;
        }

        //reverses the direction of the lights
        if (PORTDbits.RD15) { // RD15 (SW3)
            if (last == 0) {// when SW0 is on
                func_SW0(-1);
                screenDown("Count Down Slow");
            }


            if (last == 1) {// when SW1 is on
                func_SW1(&j, -1);
                screenDown("Shift Left Slow ");
            }

            if (last == 2) {// when SW2 is on
                func_SW2(&x, &y, -1);
                screenDown("Swing Down Slow");
            }

            continue;
        }

        //LED lights moves from left to right
        if (PORTFbits.RF3) { // RF3 (SW0)  
            last = 0;
            screenOn("Mode 0:");
            screenDown("Count Up Slow");
            func_SW0(last);
            continue;
        }


        //LEDs light from left to right
        if (PORTFbits.RF5) { // RF5 (SW1)
            last = 1;
            screenOn("Mode 1:");
            screenDown("Shift Right Slow");
            func_SW1(&j, last);
            continue;

        }
        //LED lights moves in opposite ways
        if (PORTFbits.RF4) { // RF4 (SW2)
            last = 2;
            screenOn("Mode 2:");
            screenDown("Swing Up Slow");
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

void clear() {
    screenDown("                ");
}

void screenOn(char* str) {
    int i;
    char control[] = {0x38, 0x38, 0x38, 0xe, 0x6, 0x1};

    PORTBbits.RB15 = 0; //rs=0
    PORTDbits.RD5 = 0; //w=0
    for (i = 0; i < 6; i++) {
        PORTE = control[i];
        PORTDbits.RD4 = 1;
        PORTDbits.RD4 = 0;
        busy();
    }

    PORTBbits.RB15 = 1; //rs=0
    PORTDbits.RD5 = 0; //w=0
    for (i = 0; i < strlen(str); i++) {
        PORTE = str[i];
        PORTDbits.RD4 = 1;
        PORTDbits.RD4 = 0;
        busy();
    }
}

void screenDown(char *str) {
    int i;

    char con = 0xc0;
    PORTBbits.RB15 = 0; //rs=0
    PORTDbits.RD5 = 0; //w=0

    PORTE = con;
    PORTDbits.RD4 = 1;
    PORTDbits.RD4 = 0;
    busy();

    PORTBbits.RB15 = 1; //rs=0
    PORTDbits.RD5 = 0; //w=0

    //check SPEED
    if (PORTDbits.RD14) {
        if (PORTFbits.RF3)
            str = "Count Down Fast";
        if (PORTFbits.RF5)
            str = "Shift Left Fast";
        if (PORTFbits.RF4)
            str = "Swing Left Fast";
        if (PORTBbits.RB11)
            str = "Halt";
        if (PORTBbits.RB10)
            str = "Beep Mode";
    }

    for (i = 0; i < strlen(str); i++) {
        PORTE = str[i];
        PORTDbits.RD4 = 1;
        PORTDbits.RD4 = 0;
        busy();
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

void busy(void) {
    char RD, RS;
    int STATUS_TRISE;
    int portMap;
    RD = PORTDbits.RD5;
    RS = PORTBbits.RB15;
    STATUS_TRISE = TRISE;
    PORTDbits.RD5 = 1; //w/r
    PORTBbits.RB15 = 0; //rs 
    portMap = TRISE;
    portMap |= 0x80;
    TRISE = portMap;
    do {
        PORTDbits.RD4 = 1; //enable=1
        PORTDbits.RD4 = 0; //enable=0
    } while (PORTEbits.RE7); // BF ?????
    PORTDbits.RD5 = RD;
    PORTBbits.RB15 = RS;
    TRISE = STATUS_TRISE;
}