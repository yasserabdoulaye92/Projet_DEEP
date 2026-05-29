/**
 *******************************************************************************
 * @file	stm32g4_adc.c
 * @author	vchav / Modifié pour Mode Continu Autonome
 *******************************************************************************
 */

#include "stm32g4_adc.h"
#if USE_ADC

#include "stm32g4_utils.h"
#include "stm32g4_gpio.h"
#include "stm32g4_timer.h"
#include "stm32g4_systick.h"
#include <stdio.h>

#ifndef USE_IN10
#define USE_IN10 0
#endif

#define ADC_NB_OF_CHANNEL_USED	(USE_IN1 + USE_IN2 + USE_IN3 + USE_IN4 + USE_IN10 + USE_IN13 + USE_IN17)

static uint16_t adc_converted_value[ADC_NB_OF_CHANNEL_USED];
static int8_t adc_id[ADC_CHANNEL_NB];
static ADC_HandleTypeDef	hadc;
static DMA_HandleTypeDef	hdma;
static volatile uint16_t t = 0;

static const uint32_t ranks[16] = {
	LL_ADC_REG_RANK_1, LL_ADC_REG_RANK_2, LL_ADC_REG_RANK_3, LL_ADC_REG_RANK_4,
	LL_ADC_REG_RANK_5, LL_ADC_REG_RANK_6, LL_ADC_REG_RANK_7, LL_ADC_REG_RANK_8,
	LL_ADC_REG_RANK_9, LL_ADC_REG_RANK_10, LL_ADC_REG_RANK_11, LL_ADC_REG_RANK_12,
	LL_ADC_REG_RANK_13, LL_ADC_REG_RANK_14, LL_ADC_REG_RANK_15, LL_ADC_REG_RANK_16
};

void DEMO_adc_process_1ms(void) { if(t) t--; }

void DEMO_adc_statemachine (void)
{
	typedef enum { INIT = 0, DISPLAY } state_e;
	static state_e state = INIT;

	switch(state) {
		case INIT:
			BSP_ADC_init();
			BSP_systick_add_callback_function(DEMO_adc_process_1ms);
			state = DISPLAY;
			break;
		case DISPLAY:{
			int16_t value;
			if(!t) {
				t = 400;
				for(uint8_t channel = 0; channel < ADC_CHANNEL_NB; channel++) {
					if(adc_id[channel] != -1) {
						value = BSP_ADC_getValue(channel);
						printf("Ch%d: %4d | ", channel, value);
					}
				}
				printf("\n");
			}
			break;}
		default:
			break;
	}
}

void ADC_PORT_Init(){
	uint8_t channel;
	ADC_ChannelConfTypeDef sConfig = {0};

	for(channel=0;channel<ADC_CHANNEL_NB;channel++)
		adc_id[channel] = -1;

	int8_t index = 0;
	sConfig.SamplingTime = ADC_SAMPLETIME_247CYCLES_5;
	sConfig.SingleDiff = ADC_SINGLE_ENDED;
	sConfig.OffsetNumber = ADC_OFFSET_NONE;
	sConfig.OffsetSaturation = DISABLE;
	sConfig.OffsetSign = ADC_OFFSET_SIGN_POSITIVE;
	sConfig.Offset = 0;

	#if USE_IN1
  		BSP_GPIO_pin_config(GPIOA, GPIO_PIN_0, GPIO_MODE_ANALOG, GPIO_NOPULL, GPIO_SPEED_FREQ_MEDIUM, GPIO_NO_AF);
		adc_id[ADC_1] = index;
		sConfig.Rank = ranks[index];
		index++;
  		sConfig.Channel = ADC_CHANNEL_1;
  		HAL_ADC_ConfigChannel(&hadc, &sConfig);
	#endif

	#if USE_IN2
  		BSP_GPIO_pin_config(GPIOA, GPIO_PIN_1, GPIO_MODE_ANALOG, GPIO_NOPULL, GPIO_SPEED_FREQ_MEDIUM, GPIO_NO_AF);
  		adc_id[ADC_2] = index;
  		sConfig.Rank = ranks[index];
  		index++;
  		sConfig.Channel = ADC_CHANNEL_2;
  		HAL_ADC_ConfigChannel(&hadc, &sConfig);
	#endif

	#if USE_IN3
  		BSP_GPIO_pin_config(GPIOA, GPIO_PIN_6, GPIO_MODE_ANALOG, GPIO_NOPULL, GPIO_SPEED_FREQ_MEDIUM, GPIO_NO_AF);
  		adc_id[ADC_3] = index;
  		sConfig.Rank = ranks[index];
  		index++;
  		sConfig.Channel = ADC_CHANNEL_3;
  		HAL_ADC_ConfigChannel(&hadc, &sConfig);
	#endif

	#if USE_IN4
  		BSP_GPIO_pin_config(GPIOA, GPIO_PIN_7, GPIO_MODE_ANALOG, GPIO_NOPULL, GPIO_SPEED_FREQ_MEDIUM, GPIO_NO_AF);
  		adc_id[ADC_4] = index;
  		sConfig.Rank = ranks[index];
  		index++;
  		sConfig.Channel = ADC_CHANNEL_4;
  		HAL_ADC_ConfigChannel(&hadc, &sConfig);
	#endif

	#if USE_IN10
  		BSP_GPIO_pin_config(GPIOF, GPIO_PIN_1, GPIO_MODE_ANALOG, GPIO_NOPULL, GPIO_SPEED_FREQ_MEDIUM, GPIO_NO_AF);
  		adc_id[ADC_10] = index;
  		sConfig.Rank = ranks[index];
  		index++;
  		sConfig.Channel = ADC_CHANNEL_10;
  		HAL_ADC_ConfigChannel(&hadc, &sConfig);
	#endif

	#if USE_IN13
  		BSP_GPIO_pin_config(GPIOA, GPIO_PIN_5, GPIO_MODE_ANALOG, GPIO_NOPULL, GPIO_SPEED_FREQ_MEDIUM, GPIO_NO_AF);
  		adc_id[ADC_13] = index;
  		sConfig.Rank = ranks[index];
  		index++;
  		sConfig.Channel = ADC_CHANNEL_13;
  		HAL_ADC_ConfigChannel(&hadc, &sConfig);
	#endif

	#if USE_IN17
  		BSP_GPIO_pin_config(GPIOA, GPIO_PIN_4, GPIO_MODE_ANALOG, GPIO_NOPULL, GPIO_SPEED_FREQ_MEDIUM, GPIO_NO_AF);
  		adc_id[ADC_17] = index;
  		sConfig.Rank = ranks[index];
  		index++;
  		sConfig.Channel = ADC_CHANNEL_17;
  		HAL_ADC_ConfigChannel(&hadc, &sConfig);
	#endif
}

void BSP_ADC_init(void)
{
	__HAL_RCC_ADC12_CLK_ENABLE();
	__HAL_RCC_GPIOF_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();

	RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC12;
	PeriphClkInit.Adc12ClockSelection = RCC_ADC12CLKSOURCE_SYSCLK;
	HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit);

	hadc.Instance = ADC2;
	hadc.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
	hadc.Init.Resolution = ADC_RESOLUTION_12B;
	hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hadc.Init.GainCompensation = 0;
	hadc.Init.ScanConvMode = ADC_SCAN_ENABLE;
	hadc.Init.EOCSelection = ADC_EOC_SEQ_CONV;
	hadc.Init.LowPowerAutoWait = DISABLE;

    /* --- LE FIX EST ICI : MODE CONTINU ET LOGICIEL (On court-circuite le Timer !) --- */
	hadc.Init.ContinuousConvMode = ENABLE;
	hadc.Init.NbrOfConversion = ADC_NB_OF_CHANNEL_USED;
	hadc.Init.DiscontinuousConvMode = DISABLE;
	hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    /* --------------------------------------------------------------------------------- */

	hadc.Init.DMAContinuousRequests = ENABLE;
	hadc.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
	hadc.Init.OversamplingMode = DISABLE;
	HAL_ADC_Init(&hadc);

    /* On ne démarre PLUS le Timer 6 ici, il est 100% libre pour ton Buzzer ! */

	ADC_PORT_Init();

	__HAL_RCC_DMAMUX1_CLK_ENABLE();
	__HAL_RCC_DMA1_CLK_ENABLE();
	hdma.Instance = DMA1_Channel1;
	hdma.Init.Request = DMA_REQUEST_ADC2;
	hdma.Init.Direction = DMA_PERIPH_TO_MEMORY;
	hdma.Init.PeriphInc = DMA_PINC_DISABLE;
	hdma.Init.MemInc = DMA_MINC_ENABLE;
	hdma.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
	hdma.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
	hdma.Init.Mode = DMA_CIRCULAR;
	hdma.Init.Priority = DMA_PRIORITY_HIGH;
	HAL_DMA_Init(&hdma);
	__HAL_LINKDMA(&hadc,DMA_Handle,hdma);

	HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
	HAL_NVIC_SetPriority(DMAMUX_OVR_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMAMUX_OVR_IRQn);

	HAL_NVIC_SetPriority(ADC1_2_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(ADC1_2_IRQn);

	HAL_ADC_Start_DMA(&hadc,(uint32_t*)adc_converted_value,ADC_NB_OF_CHANNEL_USED);
}

void ADC1_2_IRQHandler(void)
{
	HAL_ADC_IRQHandler(&hadc);
}

uint16_t BSP_ADC_getValue(adc_id_e channel)
{
	if(adc_id[channel] == -1 || channel >= ADC_CHANNEL_NB) return 0;
	return adc_converted_value[adc_id[channel]];
}

static callback_fun_t callback_function;
static volatile bool flag_new_sample_available = false;

bool BSP_ADC_is_new_sample_available(void)
{
	bool ret = flag_new_sample_available;
	if(ret) flag_new_sample_available = false;
	return ret;
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
	UNUSED(hadc);
	flag_new_sample_available = true;
	if(callback_function) callback_function();
}

void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *hadc)
{
	UNUSED(hadc);
}

void BSP_ADC_set_callback_function(callback_fun_t callback)
{
	callback_function = callback;
}

void DMA1_Channel1_IRQHandler(void) {
	HAL_DMA_IRQHandler(&hdma);
}

#endif //USE_ADC
