// ============================ //
// Do not edit this part!!!!    //
// ============================ //
// 0x300001 - CONFIG1H
#pragma config OSC = HSPLL      // Oscillator Selection bits (HS oscillator,
                                // PLL enabled (Clock Frequency = 4 x FOSC1))
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable bit
                                // (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF       // Internal/External Oscillator Switchover bit
                                // (Oscillator Switchover mode disabled)
// 0x300002 - CONFIG2L
#pragma config PWRT = OFF       // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable bits (Brown-out
                                // Reset disabled in hardware and software)
// 0x300003 - CONFIG1H
#pragma config WDT = OFF        // Watchdog Timer Enable bit
                                // (WDT disabled (control is placed on the SWDTEN bit))
// 0x300004 - CONFIG3L
// 0x300005 - CONFIG3H
#pragma config LPT1OSC = OFF    // Low-Power Timer1 Oscillator Enable bit
                                // (Timer1 configured for higher power operation)
#pragma config MCLRE = ON       // MCLR Pin Enable bit (MCLR pin enabled;
                                // RE3 input pin disabled)
// 0x300006 - CONFIG4L
#pragma config LVP = OFF        // Single-Supply ICSP Enable bit (Single-Supply
                                // ICSP disabled)
#pragma config XINST = OFF      // Extended Instruction Set Enable bit
                                // (Instruction set extension and Indexed
                                // Addressing mode disabled (Legacy mode))

#pragma config DEBUG = OFF      // Disable In-Circuit Debugger

#define KHZ 1000UL
#define MHZ (KHZ * KHZ)
#define _XTAL_FREQ (40UL * MHZ)

// ============================ //
//             End              //
// ============================ //

#include <xc.h>

// ============================ //
//        DEFINITIONS           //
// ============================ //

#define TMR_HIGH 0x67
#define TMR_LOW 0x69
#define LIMIT 150

// You can write struct definitions here...

typedef struct {
    int c,d,e,f;
} Grid;

typedef struct{
    int lux, rux, llx, rlx, luy, ruy, lly, rly;
} Shape ;


enum Shapes {DOT,SQUARE, LSHAPE};


// ============================ //
//          GLOBALS             //
// ============================ //

int prevB = 0, curB = 0;
volatile int count = 0, submit = 0, rotate = 0, debounceTimer = 0, bInterruptFired = 0;
int blink = 0, on = 0, CurShape = DOT, SubmittedBlock= 0;
int leftMov = 0, rightMov = 0, upMov = 0, downMov = 0, digitNo = 0; 
int prevA0 = 0, prevA1 = 0, prevA2 = 0, prevA3 = 0, countA0 = 0, stateA0 = 0, countA1 = 0, stateA1 = 0, countA2 = 0, stateA2 = 0, countA3 = 0, stateA3 = 0 ;
int RotationOrder = 0;
Shape shape;
Grid grid;

// You can write globals definitions here...

// ============================ //
//          FUNCTIONS           //
// ============================ //



void Init(){
    prevB = count = submit = rotate = blink = on = CurShape = SubmittedBlock = leftMov = rightMov = upMov = downMov = digitNo = RotationOrder = prevA0 = prevA1 = prevA2 = prevA3 =   0;
    
    
    
    ADCON1 = 0x0F;
    INTCONbits.GIE = 0;
    LATA = 0x00; LATB = 0x00; LATC = 0x00; LATD = 0x00; LATE = 0x00; LATF = 0x00;
    PORTA = 0x00; PORTB = 0x00; PORTC = 0x00; PORTD = 0x00; PORTE = 0x00; PORTF = 0x00;
    TRISA = 0x0F; TRISB = 0x60;  TRISC = 0x00; TRISD = 0x00; TRISE = 0x00; TRISF = 0x00;
    LATG = 0x00; PORTG = 0x00; TRISG = 0x00;
    // 7-SEGMENT
    LATH = 0x00; PORTH = 0x00; TRISH = 0x00;
    
    TRISJ = 0x00; LATJ = 0x3F;
    
    PORTH = 0x0F;
    
    grid.c = grid.d = grid.e = grid.f = 0; // initialize grid
    
}

void WaitForOneSec(){
    for(int i = 0; i < 0xFF; i++){
        for(int j = 0; j < 0xA5 ; j++){
            for(int k = 0; k < 0x10 ; k++);
        }
    }
}

void Arrange_Interrupts(){
    PIE2 = 0x00;
    INTCON2 = 0x00;
    INTCON2bits.INTEDG0 = 1;
    INTCON2bits.INTEDG1 = 1;
    //INTCON2bits.RBIP = 1;
    INTCON2bits.RBPU = 1;
    
    T0CON = 0x00;
    T0CONbits.TMR0ON = 1;
    T0CONbits.T0PS2 = 1;
    T0CONbits.T0PS1 = 0;
    T0CONbits.T0PS0 = 1;
    
    RCON = 0x00;
    
    // 0x6768
    TMR0H = TMR_HIGH;
    TMR0L = TMR_LOW;
    
    INTCON = 0x00;
    INTCONbits.TMR0IE = 1;
    INTCONbits.RBIE = 1;
    INTCONbits.PEIE = 1;
}


void Fall_Down(){
    if(shape.luy == 8 || shape.ruy == 8 || shape.lly == 8 || shape.rly == 8) return;
    if(shape.luy) shape.luy++;
    if(shape.ruy) shape.ruy++;
    if(shape.lly) shape.lly++;
    if(shape.rly) shape.rly++;

}

 void Fall_Each_Two_Sec(){
     
     if(count == 8){
         
        Fall_Down();
         
        count = 0;
     }
     
 }

 
 int Find(int col){
     int res = 0;
     if(shape.lux  == col || shape.llx == col){ // col'de eleman varsa
         int first = 0, second = 0; // res = 00000001
         if(shape.luy > 0 ){ // luy coldeki pozisyonu
             first = 1 ; // first = 00000001;
             first = first << (shape.luy-1) ;
         }
         if(shape.lly > 0){ // lly col'deki pozisyonu
             second = 1;
             second = second << (shape.lly-1);
         }
         res = second | first;
     }
     if(shape.rux  == col || shape.rlx == col){ // colde eleman varsa
         int first = 0, second = 0; // res = 00000001
         if(shape.ruy > 0 ){ // luy coldeki pozisyonu
             first = 1 ; // first = 00000001;
             first = first << (shape.ruy-1) ;
         }
         if(shape.rly > 0){ // lly coldeki pozisyonu
             second = 1;
             second = second << (shape.rly-1);
         }
         res = second | first;
     }
     return res;
 }
 
 void MoveA0(){
     
     
     
     if(rightMov){
         if(shape.lux == 4 || shape.llx == 4 || shape.rlx == 4 || shape.rux == 4) return;
         if(shape.lux) shape.lux++;
         if(shape.llx) shape.llx++;
         if(shape.rux) shape.rux++;
         if(shape.rlx) shape.rlx++;
         
         rightMov = 0;
     }
    
 }
  
 void MoveA2(){
     
     if(downMov){
         if(shape.luy == 8 ||  shape.ruy == 8 ||  shape.lly == 8 || shape.rly == 8) return;
          if(shape.luy) shape.luy++;
         if(shape.lly) shape.lly++;
         if(shape.ruy) shape.ruy++;
         if(shape.rly) shape.rly++;
         downMov = 0;
     }
    
 }
     
 void MoveA1(){
   
     if(upMov){
         if(shape.luy == 1 || shape.ruy == 1) return;
         if(shape.luy) shape.luy--;
         if(shape.lly) shape.lly--;
         if(shape.ruy) shape.ruy--;
         if(shape.rly) shape.rly--;
         upMov = 0;
     }
    
 }
     
 void MoveA3(){
     
     if(leftMov){
         if(shape.lux == 1 || shape.llx == 1) return;
         if(shape.lux) shape.lux--;
         if(shape.llx) shape.llx--;
         if(shape.rux) shape.rux--;
         if(shape.rlx) shape.rlx--;
         leftMov = 0;
     }
     
 }
 

 
 void Blink(){
     
    if(blink == 1){
        if(on == 0){
            LATC = Find(1) | grid.c; // SAVE C
            LATD = Find(2) | grid.d; 
            LATE = Find(3) | grid.e;
            LATF = Find(4) | grid.f;
            
            on = 1;
        }
        else if(on == 1){
            LATC = (~Find(1)) & grid.c ; // Find(1)'in 1 oldu?u yerleri kapat
                   //00000001 -> 11111110   // 00000010 -> 00000010
            LATD = (~Find(2)) & grid.d ;
            LATE = (~Find(3)) & grid.e ;
            LATF = (~Find(4)) & grid.f ;
            on = 0;
        }
        
        blink = 0;
       
    }
    
 }
 
 void Clear_Shape(){
     shape.lux = shape.rux = shape.llx = shape.rlx = shape.luy = shape.ruy = shape.lly = shape.rly = 0;
 }
 
 void CreateNewShape(){
     
     RotationOrder = 0;
     Clear_Shape();

     if(CurShape == DOT){
         shape.lux = 1;
         shape.luy = 1;
     }
     else if( CurShape == SQUARE ){
         shape.lux = shape.luy = 1;
         shape.rux = 2; shape.ruy = 1;
         shape.llx = 1; shape.lly = 2;
         shape.rlx = shape.rly = 2;
     }
     else if(CurShape == LSHAPE){
         shape.lux = shape.luy = 1;
         shape.rux = 2; shape.ruy = 1;
         shape.rlx = shape.rly = 2;
        
     }
    
     
 }
 
 int Is_Collide(){
     if(grid.c & Find(1) ) return 1;
     if(grid.d & Find(2) ) return 1;
     if(grid.e & Find(3) ) return 1;
     if(grid.f & Find(4) ) return 1;
     return 0;
 }
 
 void SaveSevenSeg(){
     if(CurShape == DOT){
         SubmittedBlock += 1;
     }
     else if(CurShape == SQUARE){
         SubmittedBlock += 4;
     }
     else if(CurShape == LSHAPE){
         SubmittedBlock += 3;
     }
 }
 
 void Submit(){
     INTCONbits.GIE = 0;
     if(submit == 1){
         if(Is_Collide() ){
             INTCONbits.GIE = 1;
             submit = 0;
             return;
         }
         leftMov = 0; rightMov = 0; upMov = 0; downMov = 0;
         stateA0 = countA0 = 0;
         stateA1 = countA1 = 0; 
         stateA2 = countA2 = 0;
         stateA3 = countA3 = 0;
         
         grid.c = grid.c | Find(1);
         grid.d = grid.d | Find(2);
         grid.e = grid.e | Find(3);
         grid.f = grid.f | Find(4);
         
         SaveSevenSeg();
        
         CurShape = (CurShape + 1) % 3;
         CreateNewShape();
        TMR0H = TMR_HIGH;
        TMR0L = TMR_LOW;
        //on = 0;
        submit = 0;
        count = 0;
    }
     INTCONbits.GIE = 1;
 }
 
 void Rotate(){

     if(!rotate) return;
     if(CurShape == DOT || CurShape == SQUARE) return;
     if(RotationOrder == 0){
         shape.lly = shape.luy + 1; 
         shape.llx = shape.lux;
         shape.luy = 0; 
         shape.lux = 0;
     }
     else if(RotationOrder == 1){
         shape.lux = shape.rux - 1; 
         shape.luy = shape.ruy;
         shape.rux = 0; 
         shape.ruy = 0;
     }
     else if(RotationOrder == 2){
         shape.ruy = shape.rly-1;
         shape.rux = shape.rlx;
         shape.rly = 0; 
         shape.rlx = 0;
     }
     else if(RotationOrder == 3){
         shape.rly = shape.lly;
         shape.rlx = shape.llx + 1;
         shape.llx = 0;
         shape.lly = 0; 
         
     }
     
     rotate = 0;
     RotationOrder = (RotationOrder + 1) % 4;

 }

 void UpdateLeft(){
    
    int leftdigit = SubmittedBlock / 10;
    if(leftdigit == 0){
        LATJ = 0x3F; // 0011 1111
    }
    else if(leftdigit == 1){
        LATJ  = 0x06; //0000 0110 
    }
    else if(leftdigit == 2){
        LATJ = 0x5B; // 0101 1011
    }
    else if(leftdigit == 3){
        LATJ = 0x4F; // 0100 1111
    }
    LATH = 0x04;
     digitNo = (digitNo + 1) % 3;
}

void UpdateRight(){
    
    int rdigit = SubmittedBlock % 10;
    
    if(rdigit == 0){
        LATJ = 0x3F; // 0011 1111
    }
    else if(rdigit == 1){
        LATJ  = 0x06; //0000 0110 
    }
    else if(rdigit == 2){
        LATJ = 0x5B; // 0101 1011
    }
    else if(rdigit == 3){
        LATJ = 0x4F; // 0100 1111 
    }
    else if(rdigit == 4){
        LATJ = 0x66; // 0110 0110
    }
    else if(rdigit == 5){
        LATJ = 0x6D; // 0110 1101 
    }
    else if(rdigit == 6){
        LATJ = 0x7D; // 0111 1101
    }
    else if(rdigit == 7){
        LATJ = 0x07; // 0000 0111
    }
    else if(rdigit == 8){
        LATJ = 0x7F; // 0111 1111
    }
    else if(rdigit == 9){
        LATJ = 0x6F;  // 0110 1111
    }
     LATH = 0x08;
    
    digitNo = (digitNo + 1) % 3;

}

void MakeThirdZero(){
    
    LATJ = 0x3F; 
    LATH = 0x03;
    digitNo = (digitNo + 1) % 3;
    
}
/*
void MakeFourthZero(){
    LATH = 0x00;
    LATH = 0x01;
    LATJ = 0x3F; 
    LATH  = 0x00;
    digitNo = (digitNo + 1) % 4;
}*/

void UpdateSS(){
    
    //switch(digitNo){
        //case 0:
            UpdateRight();
            //break;
        //case 1:
            UpdateLeft();
            //break;
        //case 2:
            MakeThirdZero();
            //break;
                
    //}
                         
}

void MovA0(){
    switch(stateA0){
        case 0:
            if(PORTA  & 0x01){
                stateA0 = 1;
            }
            break;
        case 1:
            if(PORTA & 0x01){
                countA0++;
            }
            else{
                countA0 = 0;
                stateA0 = 0;
            }
            if(countA0 > LIMIT){
                stateA0 = 2;
                countA0 = 0;
                rightMov = 1;
            }
            break;
        case 2:
            if(!(PORTA & 0x01)){
                stateA0 = 3;
            }
            break;
        case 3:
            if(!(PORTA & 0x01)){
                countA0++;
            }
            else{
                countA0 = 0;
                stateA0 = 2;
            }
            if(countA0 > LIMIT){
                stateA0 = 0;
                countA0 = 0;
            }
            break;
    }
}
 

void MovA1(){
    switch(stateA1){
        case 0:
            if(PORTA  & 0x02){
                stateA1 = 1;
            }
            break;
        case 1:
            if(PORTA & 0x02){
                countA1++;
            }
            else{
                countA1 = 0;
                stateA1 = 0;
            }
            if(countA1 > LIMIT){
                stateA1 = 2;
                countA1 = 0;
                upMov = 1;
            }
            break;
        case 2:
            if(!(PORTA & 0x02)){
                stateA1 = 3;
            }
            break;
        case 3:
            if(!(PORTA & 0x02)){
                countA1++;
            }
            else{
                countA1 = 0;
                stateA1 = 2;
            }
            if(countA1 > LIMIT){
                stateA1 = 0;
                countA1 = 0;
            }
            break;
    }
}
 

void MovA2(){
    switch(stateA2){
        case 0:
            if(PORTA  & 0x04){
                stateA2 = 1;
            }
            break;
        case 1:
            if(PORTA & 0x04){
                countA2++;
            }
            else{
                countA2 = 0;
                stateA2 = 0;
            }
            if(countA2 > LIMIT){
                stateA2 = 2;
                countA2 = 0;
                downMov = 1;
            }
            break;
        case 2:
            if(!(PORTA & 0x04)){
                stateA2 = 3;
            }
            break;
        case 3:
            if(!(PORTA & 0x04)){
                countA2++;
            }
            else{
                countA2 = 0;
                stateA2 = 2;
            }
            if(countA2 > LIMIT){
                stateA2 = 0;
                countA2 = 0;
            }
            break;
    }
}

void MovA3(){
    switch(stateA3){
        case 0:
            if(PORTA  & 0x08){
                stateA3 = 1;
            }
            break;
        case 1:
            if(PORTA & 0x08){
                countA3++;
            }
            else{
                countA3 = 0;
                stateA3 = 0;
            }
            if(countA3 > LIMIT){
                stateA3 = 2;
                countA3 = 0;
                leftMov = 1;
            }
            break;
        case 2:
            if(!(PORTA & 0x08)){
                stateA3 = 3;
            }
            break;
        case 3:
            if(!(PORTA & 0x08)){
                countA3++;
            }
            else{
                countA3 = 0;
                stateA3 = 2;
            }
            if(countA3 > LIMIT){
                stateA3 = 0;
                countA3 = 0;
            }
            break;
    }
}
 

void setInterruptFlag(){
    if (bInterruptFired) {
        debounceTimer--;
        if(debounceTimer) return;


        //__delay_ms(10);

         curB = PORTB;

        if((curB & 0x20) ){
            rotate = 1;  
        }
        if((curB & 0x40)){
            submit = 1;   
        }

        prevB = curB;
        bInterruptFired = 0;
    }
}
 
 
// You can write function definitions here...

// ============================ //
//   INTERRUPT SERVICE ROUTINE  //
// ============================ //

__interrupt(high_priority)
void HandleInterrupt()
{
    UpdateSS();
    // ISR ...
    if(INTCONbits.TMR0IF){
        count++;
        blink = 1;
        TMR0H = TMR_HIGH;
        TMR0L = TMR_LOW;
        INTCONbits.TMR0IF = 0;
    }
    
    if(INTCONbits.RBIF){
       
        INTCONbits.RBIF = 0;
        
        curB = PORTB;
        
        if (!bInterruptFired) {
            debounceTimer = LIMIT*3;
            bInterruptFired = 1;
        }
        
        prevB = curB;
    }
}


// ============================ //
//            MAIN              //
// ============================ //

void main()
{
    Init();
    Arrange_Interrupts();
    WaitForOneSec();
    //prevB = PORTB;
    // Main ...
    CreateNewShape();
    //PORTH = 0x00;
    INTCONbits.GIE =  1;
    while(SubmittedBlock < 32){
        Rotate();
        Fall_Each_Two_Sec();
        //MovPoll();
        Submit();
        MovA0();
        MoveA0();
        MovA1();
        MoveA1();
        MovA2();
        MoveA2();
        MovA3();
        MoveA3();
        UpdateSS();
        setInterruptFlag();
        Blink();
    }
}