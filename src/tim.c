
#include "tim.h"

void TIMDelay_Config(){
	
	RCC_APB1PeriphClockCmd(TIMER_DElAY_RCC,ENABLE);
	
	TIM_TimeBaseInitTypeDef TIM_InitStruct;
	TIM_InitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_InitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_InitStruct.TIM_Period = 0xFFFF-1;
	TIM_InitStruct.TIM_Prescaler = 72-1;
	TIM_TimeBaseInit(TIM1,&TIM_InitStruct);
	TIM_Cmd(TIMER_DElAY,ENABLE);

}
void TIMPWM_Config(){
	/*Cap xung clock cho TIM2*/
    RCC_APB1PeriphClockCmd(TIMER_PWM_RCC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
	
	/* */
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
	/* */
    TIM_TimeBaseInitTypeDef TIM_TimBaseInitStructure;
    TIM_TimBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimBaseInitStructure.TIM_Period = 20000-1;
    TIM_TimBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimBaseInitStructure.TIM_Prescaler = 72-1;
    
    TIM_TimeBaseInit(TIM2, &TIM_TimBaseInitStructure);
	
    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 2000; 
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable);
    TIM_OC4Init(TIMER_PWM, &TIM_OCInitStructure);

    TIM_Cmd(TIMER_PWM, ENABLE);

}

void Delay_ms(uint32_t time){
	TIM_SetCounter(TIMER_DElAY,0);
	while(TIM_GetCounter(TIMER_DElAY) < time*10);
}

void Delay_us(uint32_t u32DelayInUs)
{
	TIM_SetCounter(TIMER_DElAY, 0);
	while(TIM_GetCounter(TIMER_DElAY) < u32DelayInUs);
}

void set_pwm(uint32_t time_pwm){
	TIM_SetCompare2(TIMER_PWM,time_pwm);
	
}
void Delay_Init(void)
{
	TIMDelay_Config();
}

