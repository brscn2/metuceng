PROCESSOR 18F8722

#include <xc.inc>

; CONFIGURATION (DO NOT EDIT)
; CONFIG1H
CONFIG OSC = HSPLL      ; Oscillator Selection bits (HS oscillator, PLL enabled (Clock Frequency = 4 x FOSC1))
CONFIG FCMEN = OFF      ; Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
CONFIG IESO = OFF       ; Internal/External Oscillator Switchover bit (Oscillator Switchover mode disabled)
; CONFIG2L
CONFIG PWRT = OFF       ; Power-up Timer Enable bit (PWRT disabled)
CONFIG BOREN = OFF      ; Brown-out Reset Enable bits (Brown-out Reset disabled in hardware and software)
; CONFIG2H
CONFIG WDT = OFF        ; Watchdog Timer Enable bit (WDT disabled (control is placed on the SWDTEN bit))
; CONFIG3H
CONFIG LPT1OSC = OFF    ; Low-Power Timer1 Oscillator Enable bit (Timer1 configured for higher power operation)
CONFIG MCLRE = ON       ; MCLR Pin Enable bit (MCLR pin enabled; RE3 input pin disabled)
; CONFIG4L
CONFIG LVP = OFF        ; Single-Supply ICSP Enable bit (Single-Supply ICSP disabled)
CONFIG XINST = OFF      ; Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled (Legacy mode))
CONFIG DEBUG = OFF      ; Disable In-Circuit Debugger


GLOBAL var1
GLOBAL var2
GLOBAL var3
GLOBAL counter   
GLOBAL counter_counter
GLOBAL prev_btn_value
GLOBAL toggle_re0
GLOBAL toggle_re1
    
; Define space for the variables in RAM
PSECT udata_acs
var1:
    DS 1 ; Allocate 1 byte for var1
var2:
    DS 1 
var3:
    DS 1
counter:
    DS 1
counter_counter:
    DS 1
prev_btn_value:
    DS 1
toggle_re0:
    DS 1
toggle_re1:
    DS 1

PSECT resetVec,class=CODE,reloc=2
resetVec:
    goto       main

PSECT CODE
main:
    clrf var1	; var1 = 0		
    clrf var2   ; var2 = 0		
    clrf var3
    clrf counter
    clrf counter_counter
    clrf prev_btn_value
    clrf toggle_re0
    clrf toggle_re1
    
    clrf TRISB	; PORTB output
    clrf TRISC	; PORTC output
    clrf TRISD	; PORTD output
    setf TRISE	; PORTE input
    
    ; START OF INITIALIZATION
    ; Light all the leds and busy wait for 1000 + 50 ms
    setf LATB
    setf LATC
    setf LATD
    call busy_wait
    clrf LATB
    clrf LATC
    clrf LATD
    ; END OF INITIALIZATION
    
    movlw 153
    movwf counter_counter   ; initialize counter_counter to 153
    main_loop:
	call button_check
	call blink_update_bar
	bra main_loop
    
busy_wait:
    bw1_loop:
	bw2_loop:
	    movlw 4
	    movwf var3
	    bw3_loop:
		decfsz var3
		bra bw3_loop
	    incfsz var1
	    bra bw2_loop
	incfsz var2
	bra bw1_loop
    return

blink_update_bar:
    incfsz counter
    return
    counter_overflowed:
	incfsz counter_counter
	return
	movlw 153
	movwf counter_counter

	; update progress bars
	btfss toggle_re0, 0
	bra bar_c_full	; branch to bar_c_full so that the bar is cleared
	movlw 255
	cpfslt LATC
	bra bar_c_full
	rrncf LATC, 1
	movlw 128
	addwf LATC, 1
	bra bar_c_exit
	
	bar_c_full:
	clrf LATC
	
	bar_c_exit:
	btfss toggle_re1, 0
	bra bar_b_full
	movlw 255
	cpfslt LATB
	bra bar_b_full
	rlncf LATB, 1
	movlw 1
	addwf LATB, 1
	bra bar_update_exit
    
	bar_b_full:
	clrf LATB
	
	bar_update_exit:
	; toggle the PORTD blinking LED
	btg LATD, 0
	
	return

button_check:
    
    ; RE0 Check
    ; prev_value == 1 and current_value == 0 then toggle the flag
    ; else no toggle
    btfss prev_btn_value, 0
    bra exit_btn0
    btfsc PORTE, 0
    bra exit_btn0
    btg toggle_re0, 0
    
    exit_btn0:
    btfss prev_btn_value, 1
    bra exit_btn_check
    btfsc PORTE, 1
    bra exit_btn_check
    btg toggle_re1, 0
    
    exit_btn_check:
    movff PORTE, prev_btn_value
    return
	
end resetVec