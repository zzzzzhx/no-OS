/***************************************************************************//**
 * @file spi_ad9250.h
 * @brief Header file of CPLD demux driver
 * @author AntoniuMiclaus (antoniu.miclaus@analog.com)
 *******************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
#ifndef SPI_AD9250_H_
#define SPI_AD9250_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "platform_drivers.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define FMC_CPLD	0x00 /* chip_select 0 */
#define FMC_AD9517	0x84 /* chip_select 1 */
#define FMC_AD9250_0	0x80 /* chip_select 2 */
#define FMC_AD9250_1	0x81 /* chip_select 3 */
#define FMC_AD9129_0	0x82 /* chip_select 4 */
#define FMC_AD9129_1	0x83 /* chip_select 5 */
#define FMC_NUM_SLAVES	6

#define FMC_DAQ1_AD9684	0x80 /* chip_select 0 */
#define FMC_DAQ1_AD9122	0x81 /* chip_select 1 */
#define FMC_DAQ1_AD9523	0x82 /* chip_select 2 */
#define FMC_DAQ1_CPLD	0x83 /* chip_select 3 */

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

static const unsigned char cs_lut[FMC_NUM_SLAVES][2] = {
	{FMC_CPLD, FMC_DAQ1_AD9684},
	{FMC_AD9517, FMC_DAQ1_AD9122},
	{FMC_AD9250_0, FMC_DAQ1_AD9523},
	{FMC_AD9250_1, FMC_DAQ1_CPLD},
	{FMC_AD9129_0, 0},
	{FMC_AD9129_1, 0},
};

static inline uint8_t cs_to_cpld(unsigned chip_select, unsigned id)
{
	return cs_lut[chip_select][id];
}

struct spi_ad9250_desc {
	spi_desc *spi_desc;
	uint8_t data;
};

struct spi_ad9250_init {
	spi_init_param spi_init;
	uint8_t cs;
	uint8_t id;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

int32_t spi_ad9520_init(struct spi_ad9250_desc **desc, const struct spi_ad9250_init *param);

int32_t spi_ad9520_transfer_one(struct spi_ad9250_desc *desc);

int32_t spi_ad9520_remove(struct spi_ad9250_desc *desc);
#endif /* SPI_AD9250_H_ */
