//main.c for BlackJack project
//contains two State Machines, 1 for LCD and 1 for dealing cards
//Raajitha Rajkumar
//CS120B TA: David Feng

#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "io.h"
#include "io.c"
#include "keypad.h"


// TimerISR() sets this to 1. C programmer should clear to 0.
volatile unsigned char TimerFlag = 0;

//Internal variables for mapping AVR's ISR to our cleaner TimerISR model.
unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

void TimerOn(){
	TCCR1B = 0x0B;
	OCR1A = 125;
	TIMSK1 = 0x02;
	TCNT1 = 0;
	
	_avr_timer_cntcurr = _avr_timer_M;
	SREG |= 0x80;
	
}

void TimerOff(){
	TCCR1B = 0x00;
}

void TimerISR(){
	TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect){
	
	_avr_timer_cntcurr--;
	if(_avr_timer_cntcurr == 0){
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

void TimerSet(unsigned long M){
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}
enum LCD_states{Begin, init, player1bet, player2bet, player1score, player2score, result, endgame} lcdstate;
	
unsigned char playeronebet = '/';
unsigned char playertwobet = '/';
unsigned p1bet = 0;
unsigned p2bet = 0;
unsigned totalbet = 0;
unsigned p1score = 0;
unsigned p2score = 0;
unsigned char drawrecieve = '0';
unsigned char draw = '0';
unsigned rounds = 3;
unsigned count = 5;
unsigned char endg = 0x00;
unsigned char p1score1 = '0';
unsigned char p1score2 = '0'; 
unsigned char p1score3 = '0';
unsigned char p2score1 = '0';
unsigned char p2score2 = '0';
unsigned char p2score3 = '0';

void LCD_Run(){
	endg = ~PINA & 0x02;
	unsigned char x = 0;
	
	switch(lcdstate){
		case Begin:
			lcdstate = init;
			break;
		case init:
			lcdstate = player1bet;
			break;
		case player1bet:
			if(endg){
				lcdstate = init;
			}
			if(playeronebet > '/'){
				if(playeronebet == '4'){
					p1bet = 4;
				}else if(playeronebet == '5'){
					p1bet = 5;
				}else if(playeronebet == '7'){
					p1bet = 7;
				}else if(playeronebet == '8'){
					p1bet = 8;
				}else if(playeronebet == '0'){
					p1bet = 0;
				}
				lcdstate = player2bet;
			}
			break;
		case player2bet:
			if(endg){
				lcdstate = init;
			}
			if(playertwobet > '/'){
				if(playertwobet == '4'){
					p2bet = 4;
				}else if(playertwobet == '5'){
					p2bet = 5;
				}else if(playertwobet == '7'){
					p2bet = 7;
				}else if(playertwobet == '8'){
					p2bet = 8;
				}else if(playertwobet == '0'){
					p2bet = 0;
				}
				lcdstate = player1score;
			}
			
			break;
		case player1score:
			if(rounds == 3){
				if(p1score1 > '0'){
					count--;
					if(count == 0){
						count = 5;
						lcdstate = player2score;
					}
				}
			}else if(rounds == 2){
				if(p1score2 > '0'){
					if(p1score == 21){
						count--;
						if(count == 0){
							lcdstate = init;
						}
					}else{
						count--;
						if(count == 0){
							count = 5;
							lcdstate = player2score;
						}
					}
				}else if(draw == '*'){
					p1score2 = 'X';
					p1score3 = 'X';
					draw = '0';
					lcdstate = player2score;
					
				}
			}else if(rounds == 1){
					if(p1score3 > '0'){
						if(p1score == 21){
							count--;
							if(count == 0){
								lcdstate = init;
							}
						}else{
							count--;
							if(count == 0){
								count = 5;
								lcdstate = player2score;
							}
						}
					}else if(draw == '*'){
						p1score3 = 'X';
						draw = '0';
						lcdstate = player2score;	
					}		
			}
			
			break;
		case player2score:
			if(rounds == 3){
				if(p2score1 > '0'){
					count--;
					if(count == 0){
						count = 5;
						rounds--;
						lcdstate = player1score;
					}
				}
			}else if(rounds == 2){
				if(p2score2 > '0'){
					if(p2score == 21){
						count--;
						if(count == 0){
							lcdstate = init;
						}
					}else{
						count--;
						if(count == 0){
							count = 5;
							rounds--;
							lcdstate = player1score;
						}
					}
				}else if(draw == '*'){
					draw = '0';
					p2score2 = 'X';
					p2score3 = 'X';
					rounds--;
					lcdstate = player1score;
				}
			}else if(rounds == 1){
				if(p2score3 > '0'){
					if(p2score == 21){
						count--;
						if(count == 0){
							lcdstate = init;
						}
					}else{
						count--;
						if(count == 0){
							count = 5;
							lcdstate = result;
						}
					}
				}else if(draw == '*'){
					draw = '0';
					p2score3 = 'X';
					lcdstate = result;
				}
			}		
			break;
		case result:
			count --;
			if(endg){
				lcdstate = init;
			}else if (count == 0){
				lcdstate = init;
				rounds = 3;
			}
			break;
		
		case endgame:
			if(!endg){
				lcdstate = init;
			}
			lcdstate = init;
			break; 
		
	}
	
	switch(lcdstate){
			case Begin:
				break;
			case init:
				playeronebet = '/';
				playertwobet = '/';
				p1bet = 0;
				p2bet = 0;
				totalbet = 0;
				p1score = 0;
				p2score = 0;
				drawrecieve = '0';
				draw = '0';
				rounds = 3;
				count = 5;
				p1score1 = '0';
				p1score2 = '0';
				p1score3 = '0';
				p2score1 = '0';
				p2score2 = '0';
				p2score3 = '0';
				LCD_ClearScreen();
				LCD_Cursor(1);
				LCD_DisplayString(1, "starting new game" );
				delay_ms(500);
				break;
			case player1bet:
				LCD_ClearScreen();
				LCD_DisplayString(1, "what is player 1's bet? ");
				
				playeronebet = GetKeypadKey();
				
				LCD_WriteData(playeronebet);
				
				break;
			case player2bet:
				
				LCD_ClearScreen();
				LCD_DisplayString(1, "what is player2's bet? ");
				
				playertwobet = GetKeypadKey();
				LCD_WriteData(playertwobet);
				
				break;
			case player1score:
				drawrecieve = GetKeypadKey();
				if(drawrecieve == '*'){
					draw = '*';
				}
				LCD_ClearScreen();
				if(rounds == 3){
					LCD_WriteData(p1score1);
				}else if(rounds == 2){
					if(draw == '*'){
						LCD_DisplayString(1, "player1 is done");
						delay_ms(100);
					}else if(p1score == 21){
						LCD_DisplayString(1, "player 1 got 21!");
						LCD_DisplayString(1, "player 1 wins!");
					}else{
						LCD_WriteData(p1score2);
					}
				}else if(rounds == 1){	
					LCD_WriteData(p1score3);
					if(draw == '*'){
						LCD_DisplayString(1, "player1 is done");
						delay_ms(100);
					}else if(p1score == 21){
						LCD_DisplayString(1, "player 1 got 21!");
						LCD_DisplayString(1, "player 1 wins!");
					}
				}
				
				break;
			case player2score:
				drawrecieve = GetKeypadKey();
				if(drawrecieve == '*'){
					draw = '*';
				}
				LCD_ClearScreen();
				if(rounds == 3){
					LCD_WriteData(p2score1);
				}else if(rounds == 2){
					LCD_WriteData(p2score2);
					if(draw == '*'){
						LCD_DisplayString(1, "player2 is done");
						delay_ms(100);
					}else if(p2score == 21){
						LCD_DisplayString(1, "player 2 got 21!");
						delay_ms(300);
						LCD_DisplayString(1, "player 2 wins!");
						delay_ms(300);
					}
				}else if(rounds == 1){
					LCD_WriteData(p2score3);
					if(draw == '*'){
						LCD_DisplayString(1, "player2 is done");
						delay_ms(100);
					}else if(p2score == 21){
						LCD_DisplayString(1, "player 2 got 21!");
						delay_ms(300);
						LCD_DisplayString(1, "player 2 wins!");
						delay_ms(300);
					}
				}
				
				break;
			case result:
				totalbet = p1bet + p2bet;
				if(totalbet == 8){
					LCD_DisplayString(1, "winner gets 800 dollars!");
				}else if(totalbet == 9){
					LCD_DisplayString(1, "winner gets 900 dollars!");
				}else if(totalbet == 11){
					LCD_DisplayString(1, "winner gets 1100 dollars!");
				}else if(totalbet == 12){
					LCD_DisplayString(1, "winner gets 1200 dollars!");
				}else if(totalbet == 4){
					LCD_DisplayString(1, "winner gets 400 dollars!");
				}else if(totalbet == 13){
					LCD_DisplayString(1, "winner gets 1300 dollars!");
				}else if(totalbet == 5){
					LCD_DisplayString(1, "winner gets 500 dollars!");
				}else if(totalbet == 15){
					LCD_DisplayString(1, "winner gets 1500 dollars!");
			    }else if(totalbet == 7){
					LCD_DisplayString(1, "winner gets 700 dollars!");
				}else if(totalbet == 14){
					LCD_DisplayString(1, "winner gets 1400 dollars!");
				}else if(totalbet == 16){
					LCD_DisplayString(1, "winner gets 1600 dollars!");
				}else if(totalbet == 10){
					LCD_DisplayString(1, "winner gets 1000 dollars!");
				}
				delay_ms(300);
				LCD_ClearScreen();
				if(p1score < 22 && p2score < 22){
					if(p1score > p2score){
						LCD_DisplayString(1, "player 1 wins!");
						delay_ms(300);
					}else if(p2score > p1score){
						LCD_DisplayString(1, "player 2 wins!");
						delay_ms(300);
					}else{
						LCD_DisplayString(1, "tie!");
						delay_ms(300);
					}
				}else if(p1score > 21 && p2score < 22){
					LCD_DisplayString(1, "player 2 wins!");
					delay_ms(300);
				}else if(p2score > 21 && p1score < 22){
					LCD_DisplayString(1, "player 1 wins!");
					delay_ms(300);
				}else{
					LCD_DisplayString(1, "no one wins");
					delay_ms(300);
				}
				
				break;
			case endgame:
				break;
			default:
				break;
		}
}

enum CardStates{Start, Init, hitmep1, hitmep2, hitmep1pressed, hitmep2pressed, ResetPressed, Reset} cardstate;

unsigned char HitMeP1Pressed = 0x00;
unsigned char HitMeP2Pressed = 0x00;
unsigned char resetPressed = 0x00;
unsigned char output = 0x00;
unsigned randomint = 0;


void DealCard(){
	HitMeP1Pressed = ~PINA & 0x01;
	HitMeP2Pressed = ~PINA & 0x04;
	resetPressed = ~PINA & 0x02;	
	
	switch(cardstate){
		case Start:
			cardstate = Init;
			break;
		case Init:
			if(HitMeP1Pressed){
				cardstate = hitmep1pressed;
			}else if(HitMeP2Pressed){
				cardstate = hitmep2pressed;
			}else if(resetPressed){
				cardstate = ResetPressed;
			}
			break;
		case hitmep1pressed:
			cardstate = hitmep1;
			if(!HitMeP1Pressed){
				cardstate = hitmep1;
			}
			break;
		case hitmep2pressed:
			cardstate = hitmep2;
			if(!HitMeP2Pressed){
				cardstate = hitmep2;
			}
			break;
		case ResetPressed:
			if(!resetPressed){
				cardstate = Reset;
			}
			break;
		case hitmep1:
			if(resetPressed){
				cardstate = ResetPressed;
			}
			cardstate = Init;
			break;
		case hitmep2:
			if(resetPressed){
				cardstate = ResetPressed;
			}
			cardstate = Init;
			break;
		case Reset:
			if(HitMeP1Pressed){
				cardstate = hitmep1pressed;
			}else if(HitMeP2Pressed){
				cardstate = hitmep2pressed;
			}else if(resetPressed){
				cardstate = ResetPressed;
			}
			break;
	}
	switch(cardstate){
		case Start:
			break;
		case Init:
			break;
		case hitmep1pressed:
			break;
		case hitmep2pressed:
			break;
		case ResetPressed:
			break;
		case hitmep1:
			randomint = rand() % 12;
			
			if(randomint == 0){
				output = 1<<2; //ace
				if(rounds == 3){
					p1score1 = 'a';
					p1score = 1;
				}else if(rounds == 2){
					p1score2 = 'a';
					if(p1score > 10){
						p1score = 1 + p1score1;
					}else{
						p1score = 11 + p1score1;
					}
				}else if(rounds == 1){
					p1score3 = 'a';
					if(p1score > 10){
						p1score = p1score + 1;
					}else{
						p1score = p1score + 11;
					}
				}	
			}else if(randomint == 1){
				output = (1<<6) | (1<<1); // 2
				if(rounds == 3){
					p1score1 = '2';
					p1score = 2;
				}else if(rounds == 2){
					p1score2 = '2';
					p1score = p1score + 2;
				}else if(rounds == 1){
					p1score3 = '2';
					p1score = p1score + 2;
				}
			}else if(randomint == 2){
				output = (1<<6) | (1<<0); //3
				if(rounds == 3){
					p1score1 = '3';
					p1score = 3;
				}else if(rounds == 2){
					p1score2 = '3';
					p1score = p1score + 3;
				}else if(rounds == 1){
					p1score3 = '3';
					p1score = p1score + 3;
				}
		    }else if(randomint == 3){
				output = (1<<0) | (1<<5) | (1<<2); //4
				if(rounds == 3){
					p1score1 = '4';
					p1score = 4;
				}else if(rounds == 2){
					p1score2 = '4';
					p1score = p1score + 4;
				}else if(rounds == 1){
					p1score3 = '4';
					p1score = p1score + 4;
				}
			}else if(randomint == 4){
				output = (1<<4) | (1<<0); //5
				if(rounds == 3){
					p1score1 = '5';
					p1score = 5;
				}else if(rounds == 2){
					p1score2 = '5';
					p1score = p1score + 5;
				}else if(rounds == 1){
					p1score3 = '5';
					p1score = p1score + 5;
				}
			}else if(randomint == 5){
				output = (1<<4) | (1<<5); //6
				if(rounds == 3){
					p1score1 = '6';
					p1score = 6;
				}else if(rounds == 2){
					p1score2 = '6';
					p1score = p1score + 6;
				}else if(rounds == 1){
					p1score3 = '6';
					p1score = p1score + 6;
				}
			}else if(randomint == 6){
				output = (1<<2) | (1<<0) | (1<<6) | (1<<7); //7
				if(rounds == 3){
					p1score1 = '7';
					p1score = 7;
				}else if(rounds == 2){
					p1score2 = '7';
					p1score = p1score + 7;
				}else if(rounds == 1){
					p1score3 = '7';
					p1score = p1score + 7;
				}
			}else if(randomint == 7){
				output = 1<<8; //8
				if(rounds == 3){
					p1score1 = '8';
					p1score = 8;
				}else if(rounds == 2){
					p1score2 = '8';
					p1score = p1score + 8;
				}else if(rounds == 1){
					p1score3 = '8';
					p1score = p1score + 8;
				}
			}else if(randomint == 8){
				output = 1<<0; //9
				if(rounds == 3){
					p1score1 = '9';
					p1score = 9;
				}else if(rounds == 2){
					p1score2 = '9';
					p1score = p1score + 9;
				}else if(rounds == 1){
					p1score3 = '9';
					p1score = p1score + 9;
				}
			}else if(randomint == 9){
				output = (1<<5) | (1<<6) | (1<<7); //10
				if(rounds == 3){
					p1score1 = 'J';
					p1score = 10;
				}else if(rounds == 2){
					p1score2 = 'J';
					p1score = p1score + 10;
				}else if(rounds == 1){
					p1score3 = 'J';
					p1score = p1score + 10;
				}
			}else if(randomint == 10){
				output = (1<<5) | (1<<6) | (1<<7); //J
				if(rounds == 3){
					p1score1 = 'J';
					p1score = 10;
				}else if(rounds == 2){
					p1score2 = 'J';
					p1score = p1score + 10;
				}else if(rounds == 1){
					p1score3 = 'J';
					p1score = p1score + 10;
				}
			}else if(randomint == 11){
				output =  (1<<2) | (1<<0); //Q
				if(rounds == 3){
					p1score1 = 'Q';
					p1score = 10;
				}else if(rounds == 2){
					p1score2 = 'Q';
					p1score = p1score + 10;
				}else if(rounds == 1){
					p1score3 = 'Q';
					p1score = p1score + 10;
				}
			}else {
				output = (1<<2) | (1<<5); //K
				if(rounds == 3){
					p1score1 = 'K';
					p1score = 10;
				}else if(rounds == 2){
					p1score2 = 'K';
					p1score = p1score + 10;
				}else if(rounds == 1){
					p1score3 = 'K';
					p1score = p1score + 10;
				}
			}
			break;
		case hitmep2:
			randomint = rand() % 12;
			
			if(randomint == 0){
				output = 1<<2; //ace
				if(rounds == 3){
					p2score1 = 'a';
					p2score = 1;
					
				}else if(rounds == 2){
					p2score2 = 'a';
					if(p2score > 10){
						p2score = 1 + p2score;
					}else{
						p2score = 11 + p2score;
					}
					
				}else if(rounds == 1){
					p2score3 = 'a';
					if(p2score > 10){
						p2score = 1 + p2score;
					}else{
						p2score = 11 + p2score;
					}
				}
			}else if(randomint == 1){
				output = (1<<1) | (1<<6); // 2
				if(rounds == 3){
					p2score1 = '2';
					p2score = 2;
					
				}else if(rounds == 2){
					p2score2 = '2';
					p2score = p2score + 2;
					
				}else if(rounds == 1){
					p2score3 = '2';
					p2score = p2score + 2;
				}
			}else if(randomint == 2){
				output = (1<<0) | (1<<6); //3
				if(rounds == 3){
					p2score1 = '3';
					p2score = 3;
					
				}else if(rounds == 2){
					p2score2 = '3';
					p2score = p2score + 3;
					
				}else if(rounds == 1){
					p2score3 = '3';
					p2score = p2score + 3;
				}				
			}else if(randomint == 3){
				output = (1<<0) | (1<<5) | (1<<2); //4
				if(rounds == 3){
					p2score1 = '4';
					p2score = 4;
					
				}else if(rounds == 2){
					p2score2 = '4';
					p2score = p2score + 4;
					
				}else if(rounds == 1){
					p2score3 = '4';
					p2score = p2score + 4;
				}
			}else if(randomint == 4){
				output = (1<<4) | (1<<0); //5
				if(rounds == 3){
					p2score1 = '5';
					p2score = 5;
					
				}else if(rounds == 2){
					p2score2 = '5';
					p2score = p2score + 5;
					
				}else if(rounds == 1){
					p2score3 = '5';
					p2score = p2score + 5;
				}
			}else if(randomint == 5){
				output = (1<<4); //6
				if(rounds == 3){
					p2score1 = '6';
					p2score = 6;
					
				}else if(rounds == 2){
					p2score2 = '6';
					p2score = p2score + 6;
					
				}else if(rounds == 1){
					p2score3 = '6';
					p2score = p2score + 6;
				}
			}else if(randomint == 6){
				output = (1<<2) | (1<<0) | (1<<6) | (1<<7); //7
				if(rounds == 3){
					p2score1 = '7';
					p2score = 7;
					
				}else if(rounds == 2){
					p2score2 = '7';
					p2score = p2score + 7;
					
				}else if(rounds == 1){
					p2score3 = '7';
					p2score = p2score + 7;
				}
			}else if(randomint == 7){
				output = (1<<8); //8
				if(rounds == 3){
					p2score1 = '8';
					p2score = 8;
					
				}else if(rounds == 2){
					p2score2 = '8';
					p2score = p2score + 8;
					
				}else if(rounds == 1){
					p2score3 = '8';
					p2score = p2score + 8;
				}
			}else if(randomint == 8){
				output = (1<<0); //9
				if(rounds == 3){
					p2score1 = '9';
					p2score = 9;
					
				}else if(rounds == 2){
					p2score2 = '9';
					p2score = p2score + 9;
					
				}else if(rounds == 1){
					p2score3 = '9';
					p2score = p2score + 9;
				}
			}else if(randomint == 9){
				output = (1<<5) | (1<<6) | (1<<7); //10
				if(rounds == 3){
					p2score1 = 'J';
					p2score = 10;
				
				}else if(rounds == 2){
					p2score2 = 'J';
					p2score = p2score + 10;
					
				}else if(rounds == 1){
					p2score3 = 'J';
					p2score = p2score + 10;
				}
			}else if(randomint == 10){
				output = (1<<5) | (1<<6) | (1<<7); //J
				if(rounds == 3){
					p2score1 = 'J';
					p2score = 10;
					
				}else if(rounds == 2){
					p2score2 = 'J';
					p2score = p2score + 10;
					
				}else if(rounds == 1){
					p2score3 = 'J';
					p2score = p2score + 10;
				}
			}else if(randomint == 11){
				output =  (1<<2) | (1<<0); //Q
				if(rounds == 3){
					p2score1 = 'Q';
					p2score = 10;
					
				}else if(rounds == 2){
					p2score2 = 'Q';
					p2score = p2score + 10;
					
				}else if(rounds == 1){
					p2score3 = 'Q';
					p2score = p2score + 10;
				}
			}else {
				output = (1<<2) | (1<<5); //K
				if(rounds == 3){
					p2score1 = 'K';
					p2score = 10;
					
				}else if(rounds == 2){
					p2score2 = 'K';
					p2score = p2score + 10;
					
				}else if(rounds == 1){
					p2score3 = 'K';
					p2score = p2score + 10;
				}
			}
			break;
		case Reset:
			output = (1<<1) | (1<<2) | (1<<0) | (1<<5) | (1<<7) | (1<<6) | (1<<4);
			break;
		default:
			break;
	}
	PORTB = output;
			
			
}

int main(void){
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	DDRC = 0xF0; PORTC = 0x0F;
	DDRD = 0xFF; PORTD = 0x00;
	
	LCD_init();
	LCD_ClearScreen();

	cardstate = Start;
	lcdstate = Begin;
	
	TimerSet(100);
	TimerOn();
	
	
	while (1){
		DealCard();
		LCD_Run();
		
		while(!TimerFlag);
		TimerFlag = 0;
		
	}
	return 0;
}
