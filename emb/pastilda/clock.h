/*
 * This file is part of the pastilda project.
 * hosted at http://github.com/thirdpin/pastilda
 *
 * Copyright (C) 2016  Third Pin LLC
 * Written by Anastasiia Lazareva <a.lazareva@thirdpin.ru>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef CLOCK_H
#define CLOCK_H

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/flash.h>

static constexpr struct rcc_clock_scale rcc_hse_25mhz_to_hclk_120mhz =
{
		15 /*PLLM*/, 144 /*PLLN*/, 2 /*PLLP*/, 5 /*PLLQ*/,
		(FLASH_ACR_ICE | FLASH_ACR_DCE | FLASH_ACR_LATENCY_3WS) /*FLASH CONFIG*/,
		RCC_CFGR_HPRE_DIV_NONE /*HPRE*/,
		RCC_CFGR_PPRE_DIV_4 /*PPRE1*/, RCC_CFGR_PPRE_DIV_2 /*PPRE2*/,
		1 /*POWER SAVE*/, 120000000 /*AHB FREQ*/,
		30000000 /*APB1_FREQ*/, 60000000 /*APB2_FREQ*/
};

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
