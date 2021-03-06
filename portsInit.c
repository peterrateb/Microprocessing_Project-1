void portAInit(void)
{
	SYSCTL_RCGCGPIO_R |= 0x01; //enable PORT A
	while((SYSCTL_PRGPIO_R&0x01)==0) {}
	GPIO_PORTA_LOCK_R= 0x4C4F434B; //giving write access to port A
	GPIO_PORTA_CR_R |= 0xFC; //commit register
	GPIO_PORTA_AFSEL_R &=0x03 ; // I/O
	GPIO_PORTA_PCTL_R &=0x03; //no alternative function
	GPIO_PORTA_AMSEL_R &=0x03; //Disable Analog function
	GPIO_PORTA_DIR_R |= 0xFC; //output pins
	GPIO_PORTA_DEN_R |= 0xFC; //Digital pins
	GPIO_PORTA_PUR_R &= 0x03; //pull up resitor register
}
void portBInit(void){

	SYSCTL_RCGCGPIO_R |= 0x00000002; //SYSCTL_RCGCGPIO_R1 instead but it gives an ERROR!!
  while((SYSCTL_RCGCGPIO_R & 0x00000002) == 0 ) {};
	GPIO_PORTB_LOCK_R=0x4C4F434B;
	GPIO_PORTB_CR_R=0xFF;
	GPIO_PORTB_DIR_R |= 0xFF;   //all pins are output
	GPIO_PORTB_AFSEL_R =0;
	GPIO_PORTB_PCTL_R=0;
	GPIO_PORTB_AMSEL_R=0;
	GPIO_PORTB_DEN_R=0xFF;
	GPIO_PORTB_PUR_R=0;
}
void portDInit(void) {

	SYSCTL_RCGCGPIO_R |= 0x08;
	// SYSCTL_RCGC2_R |= 0x08;
	while( (SYSCTL_PRGPIO_R & 0x08) == 0);

	// GPIO_PORTD_LOCK_R = 0x4C4F434B; // port d doesn't need it.
	// GPIO_PORTD_CR_R |= 0x0C; // allow changes to pins d3=>d2
	GPIO_PORTD_AMSEL_R &= ~0x0C; // disable analog
	GPIO_PORTD_AFSEL_R &= ~0x0C; // not alternative
	GPIO_PORTD_DIR_R &= 0xFB; // set pin 2 as input
	GPIO_PORTD_DIR_R |= 0x08; // set pin 3 as output
	GPIO_PORTD_PCTL_R = 0x00000000; // gpio clear bit pctl
	GPIO_PORTD_PDR_R |= 0x04;
	GPIO_PORTD_DEN_R |= 0x0C; // enable pins

}
void portEInit(void) {

	SYSCTL_RCGCGPIO_R |= 0x10;
	// SYSCTL_RCGC2_R |= 0x10;
	while( (SYSCTL_PRGPIO_R & 0x10) == 0);

	// GPIO_PORTE_LOCK_R = 0x4C4F434B; // port e doesn't need it.
	// GPIO_PORTE_CR_R |= 0x3F; // allow changes to pins e5=>e0
	GPIO_PORTE_AMSEL_R &= ~0x3F; // disable analog
	GPIO_PORTE_AFSEL_R &= ~0x3F; // not alternative
	GPIO_PORTE_DIR_R |= 0x3F; // set pins as output
	GPIO_PORTE_PCTL_R = 0x00000000; // gpio clear bit pctl
	GPIO_PORTE_DEN_R |= 0x3F; // enable pins

}
void portFInit(void){
	/* initiating portF with pins 0,4   digital in with pull up 
				 pins 1,2,3 digital out                */
	SYSCTL_RCGCGPIO_R|=0x20;//enable clk to port f
	while(!(SYSCTL_PRGPIO_R&0x20));// loop until clk connected to port f
	
	GPIO_PORTF_LOCK_R=0x4C4F434B;//to unlock the port f
	GPIO_PORTF_CR_R|=0x1f;// to activate unlock to port f pins
	
	GPIO_PORTF_DIR_R|=0x0e;// set pin 0,4 as in and 1:3 as out
	GPIO_PORTF_AFSEL_R=0;// reset the alternate functions for pins 0:4
	GPIO_PORTF_PUR_R =0x11;// enable pullup resistors to pin 0,4
	GPIO_PORTF_DEN_R|=0x1f;// set port f pins 0 to 4 as digital
	GPIO_PORTF_AMSEL_R|=0;// reset analog mode for pins 0:4
	
}
void PLLInit(void){
	/* at the end of this fn the clk freq will be 80M */
	SYSCTL_RCC2_R|=0X80000000;//use RCC2
	SYSCTL_RCC2_R|=0X800;//bypass disabled
	
	SYSCTL_RCC_R=(SYSCTL_RCC_R&~0X7C0)+0X540;//select crystal value 16M
	SYSCTL_RCC2_R&=~0X70;//select osc source
	SYSCTL_RCC2_R&=~0X2000;//activate PLL
	SYSCTL_RCC2_R|=0X40000000;//set divider 
	SYSCTL_RCC2_R=(SYSCTL_RCC2_R&~0X1FC00000)+(4<<22);//div=4
	while(!SYSCTL_RCC2_R&0X40);//wait for PLL to lock
	SYSCTL_RCC2_R&=~0X800;//enable PLL by clearing bypass
}
void delay_ms(int delay){
	/* using clk freq 80M will make adelay with unit of mseconds */
	int x=0;//dummy variable
	SYSCTL_RCGCTIMER_R|=1;//connect the timer to clk
	x+=5;//make sure the clk is connected
	TIMER0_CTL_R&=~1;//disable the timer to configure it
	TIMER0_CFG_R=0;//32-Bit timer mode
	TIMER0_TAMR_R=2;//periodic mood
	TIMER0_TAMR_R|=0x10;//up select
	TIMER0_TAILR_R=80000*delay-1;//interval value to count
	TIMER0_ICR_R|=1;//clera the timeout flag
	TIMER0_CTL_R|=1;//timer enabled and counting starts
	while(!TIMER0_RIS_R&1);//wait to finish the interval
}
uint64_t Timer1Init(void)//initiate timer1 as periodic with up counting to 0xffff.ffff
{
int CurrentTime;
int x=0;//dummy variable
while((GPIO_PORTD_DATA_R & 0x01)==0); //D0 is Low

	SYSCTL_RCGCTIMER_R|=1;//connect the timer to clk
	x+=5;//make sure the clk is connected
	TIMER0_CTL_R&=~1;//disable the timer to configure it
	TIMER0_CFG_R=0;//32-Bit timer mode
	TIMER0_TAMR_R=2;//periodic mood
	TIMER0_TAMR_R|=0x10;//up select
	TIMER0_TAILR_R=0xFFFFFFFF;//interval value to count
	TIMER0_ICR_R|=1;//clear the timeout flag
	TIMER0_CTL_R|=1;//timer enabled and counting starts
	
	while((GPIO_PORTD_DATA_R & 0x01)==0x01);  //D0 is High

	CurrentTime=TIMER0_TAV_R; //get the current value of the timer
	TIMER0_CTL_R&=~1; // disable the timer
	return CurrentTime;
}
