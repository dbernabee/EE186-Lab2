#include <stdint.h>

uint32_t pause = 0; //ON (1) while button is pressed

int main(void)
{
	//clock registers configuration
	uint32_t *rcc_cr = (uint32_t *)(0x40021000 + 0x00);
	uint32_t * rcc_cfgr = (uint32_t *)(0x40021000 + 0x08);
	uint32_t *rcc_ahb2enr = (uint32_t *)(0x40021000 + 0x4C);
	uint32_t *rcc_apb1enr1 = (uint32_t *)(0x40021000 + 0x58);
	uint32_t * rcc_apb2enr = (uint32_t *)(0x40021000 + 0x60);

	*rcc_cr |= (1UL << 8);
	while(!(*rcc_cr & (1UL << 10)));
	*rcc_cfgr &= ~(3UL << 0);
	*rcc_cfgr|= (1UL << 0);
	while(((*rcc_cfgr >> 2) & 3UL) != 1UL);

	//tim4 clock
	*rcc_ahb2enr |= (1UL << 1) | (1UL << 2); //gpiob,c
	*rcc_apb1enr1 |= (1UL << 2); //tim4
	*rcc_apb2enr |= (1UL << 0); //syscfg

	//port b
	uint32_t *gpiob_moder = (uint32_t *)(0x48000400 + 0x00);
	uint32_t *gpiob_otyper = (uint32_t *)(0x48000400 + 0x04);
	uint32_t *gpiob_ospeedr = (uint32_t *)(0x48000400 + 0x08);
	uint32_t *gpiob_pupdr = (uint32_t *)(0x48000400 + 0x0C);

	//port c
	uint32_t *gpioc_moder = (uint32_t *)(0x48000800 + 0x00);
	uint32_t *gpioc_otyper = (uint32_t *)(0x48000800 + 0x04);
	uint32_t *gpioc_ospeedr = (uint32_t *)(0x48000800 + 0x08);
	uint32_t *gpioc_pupdr = (uint32_t *)(0x48000800 + 0x0C);
	uint32_t *gpioc_odr = (uint32_t *)(0x48000800 + 0x14);

	//pb7
	*gpiob_moder &= ~(3UL<<14); //clear mode bits
	*gpiob_moder |= (1UL<<14); //set mode to output
	*gpiob_otyper &= ~(1UL<<7); //push-pull output
	*gpiob_ospeedr &= ~(3UL<<14); //low speed
	*gpiob_pupdr &= ~(3UL<<14); //no pull

	//pb14
	*gpiob_moder &= ~(3UL<<28);
	*gpiob_moder |= (1UL<<28);
	*gpiob_otyper &= ~(1UL<<14);
	*gpiob_ospeedr &= ~(3UL<<28);
	*gpiob_pupdr &= ~(3UL<<28);

	//pc7
	*gpioc_moder &= ~(3UL<<14);
	*gpioc_moder |= (1UL<<14);
	*gpioc_otyper &= ~(1UL<<7);
	*gpioc_ospeedr &= ~(3UL<<14);
	*gpioc_pupdr &= ~(3UL<<14);

	//pc13 (push button)
	*gpioc_moder &= ~(3UL<<26); //clear mode bits, input

	//turning on green first :)
	*gpioc_odr |= (1UL << 7);

	//SYSCFG configuration
	//pc13 -> EXTI13[3:0], Bits 7:4, corresponds to SYSCFG_EXTICR4, offset: 0x14
	uint32_t * syscfg_exticr4 = (uint32_t *)(0x40010000 + 0x14);
	*syscfg_exticr4 &= ~(0xFUL << 4); //clear exiti13
	*syscfg_exticr4 |= (0x2UL << 4);  //port c

	//EXTI
	uint32_t *exti_imr1 = (uint32_t *)(0x40010400 + 0x00);
	uint32_t *exti_rtsr1 = (uint32_t *)(0x40010400 + 0x08);
	uint32_t *exti_ftsr1 = (uint32_t *)(0x40010400 + 0x0c); //added line
	*exti_imr1 |= (1UL << 13); //unmask
	*exti_rtsr1 |= (1UL << 13); //rising edge trig.
	*exti_ftsr1 |= (1UL << 13); //added line

	//tim4 config.
	uint32_t *tim4_cr1 = (uint32_t *)(0x40000800 + 0x00);
	uint32_t *tim4_dier = (uint32_t *)(0x40000800 + 0x0C);
	uint32_t *tim4_sr = (uint32_t *)(0x40000800 + 0x10);
	uint32_t *tim4_egr = (uint32_t *)(0x40000800 + 0x14);
	uint32_t *tim4_psc = (uint32_t *)(0x40000800 + 0x28);
	uint32_t *tim4_arr = (uint32_t *)(0x40000800 + 0x2C);

	*tim4_psc = 1599; //16Mhz
	*tim4_arr = 9999; //1s
	*tim4_egr |= (1UL << 0);
	*tim4_sr &= ~(1UL << 0);
	*tim4_dier |= (1UL << 0);

	uint32_t *nvic_iser0 = (uint32_t *)0xE000E100;
	uint32_t *nvic_iser1 = (uint32_t *)0xE000E104;
	*nvic_iser0 = (1UL << 30);
	*nvic_iser1 = (1UL << 8);


	*tim4_cr1 |= (1UL << 0);

	while(1){}
}

void EXTI15_10_IRQHandler(void){

	uint32_t *exti_pr1 = (uint32_t *)(0x40010400 + 0x14);
	uint32_t *gpioc_idr = (uint32_t *)(0x48000800 + 0x10);

	if(*exti_pr1 & (1UL << 13)){
		*exti_pr1 = (1UL << 13); //write 1 to clear
		if(*gpioc_idr & (1UL << 13)) pause = 1; //pin is high
		else pause = 0; //pin is low
	}
}

void TIM4_IRQHandler(void){

	uint32_t *tim4_sr = (uint32_t *)(0x40000800 + 0x10);
	uint32_t *gpiob_odr = (uint32_t *)(0x48000400 + 0x14);
	uint32_t *gpioc_odr = (uint32_t *)(0x48000800 + 0x14);
	static uint32_t state = 1; //green on, next is blue

	if(*tim4_sr & (1UL << 0)){
		*tim4_sr &= ~(1UL << 0); //clear

		if(pause == 0){
			*gpioc_odr &= ~(1UL << 7);
			*gpiob_odr &= ~((1UL << 7) | (1UL << 14));

			if(state == 0) *gpioc_odr |= (1UL << 7); //green
				else if(state == 1) *gpiob_odr |= (1UL << 7); //blue
				else *gpiob_odr |= (1UL << 14); //red

				state = (state + 1) % 3;
		}
	}
}
