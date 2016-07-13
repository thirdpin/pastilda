#ifndef CLOCK_H
#define CLOCK_H

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/flash.h>

#ifdef STM32F2
static constexpr struct rcc_clock_scale rcc_hse_25mhz_to_hclk_120mhz =
{
		20 /*PLLM*/, 192 /*PLLN*/, 2 /*PLLP*/, 5 /*PLLQ*/,
		(FLASH_ACR_ICE | FLASH_ACR_DCE | FLASH_ACR_LATENCY_3WS) /*FLASH CONFIG*/,
		RCC_CFGR_HPRE_DIV_NONE /*HPRE*/,
		RCC_CFGR_PPRE_DIV_4 /*PPRE1*/, RCC_CFGR_PPRE_DIV_2 /*PPRE2*/,
		30000000 /*APB1_FREQ*/, 60000000 /*APB2_FREQ*/
};

static constexpr struct rcc_clock_scale rcc_hse_8mhz_to_hclk_120mhz =
{
		8 /*PLLM*/, 120 /*PLLN*/, 2 /*PLLP*/, 5 /*PLLQ*/,
		FLASH_ACR_ICE | FLASH_ACR_DCE | FLASH_ACR_LATENCY_3WS /*FLASH CONFIG*/,
		RCC_CFGR_HPRE_DIV_NONE /*HPRE*/,
		RCC_CFGR_PPRE_DIV_4 /*PPRE1*/, RCC_CFGR_PPRE_DIV_2 /*PPRE2*/,
		30000000 /*APB1_FREQ*/, 60000000 /*APB2_FREQ*/
};
#endif

#ifdef STM32F4
static constexpr struct rcc_clock_scale rcc_hse_25mhz_to_hclk_120mhz =
{
		15 /*PLLM*/, 144 /*PLLN*/, 2 /*PLLP*/, 5 /*PLLQ*/,
		(FLASH_ACR_ICE | FLASH_ACR_DCE | FLASH_ACR_LATENCY_3WS) /*FLASH CONFIG*/,
		RCC_CFGR_HPRE_DIV_NONE /*HPRE*/,
		RCC_CFGR_PPRE_DIV_4 /*PPRE1*/, RCC_CFGR_PPRE_DIV_2 /*PPRE2*/,
		1 /*POWER SAVE*/, 120000000 /*AHB FREQ*/,
		30000000 /*APB1_FREQ*/, 60000000 /*APB2_FREQ*/
};

static constexpr struct rcc_clock_scale rcc_hse_8mhz_to_hclk_120mhz =
{
		4 /*PLLM*/, 120 /*PLLN*/, 2 /*PLLP*/, 5 /*PLLQ*/,
		FLASH_ACR_ICE | FLASH_ACR_DCE | FLASH_ACR_LATENCY_3WS /*FLASH CONFIG*/,
		RCC_CFGR_HPRE_DIV_NONE /*HPRE*/,
		RCC_CFGR_PPRE_DIV_4 /*PPRE1*/, RCC_CFGR_PPRE_DIV_2 /*PPRE2*/,
		1 /*POWER SAVE*/, 120000000 /*AHB FREQ*/,
		30000000 /*APB1_FREQ*/, 60000000 /*APB2_FREQ*/
};
#endif

static void clock_setup()
{
	rcc_clock_setup_hse_3v3(&rcc_hse_25mhz_to_hclk_120mhz);

	rcc_periph_clock_enable(rcc_periph_clken::RCC_GPIOA);
	rcc_periph_clock_enable(rcc_periph_clken::RCC_GPIOB);
	rcc_periph_clock_enable(rcc_periph_clken::RCC_GPIOC);
	rcc_periph_clock_enable(rcc_periph_clken::RCC_TIM6);
	rcc_periph_clock_enable(rcc_periph_clken::RCC_SPI1);
	rcc_periph_clock_enable(rcc_periph_clken::RCC_OTGFS);
	rcc_periph_clock_enable(rcc_periph_clken::RCC_OTGHS);
}

#endif
