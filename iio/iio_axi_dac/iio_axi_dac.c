/***************************************************************************//**
 *   @file   iio_axi_dac.c
 *   @brief  Implementation of iio_axi_dac.
 *   Instantiates iio_axi_adc and iio_axi_dac devices, for
 *   reading/writing and parameterization.
 *   @author Cristian Pop (cristian.pop@analog.com)
********************************************************************************
 * Copyright 2019(c) Analog Devices, Inc.
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <string.h>
#include <inttypes.h>
#include <stdlib.h>
#include "error.h"
#include "iio.h"
#include "iio_axi_dac.h"
#include "xml.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief get_dds_calibscale().
 * @param device - Physical instance of a iio_axi_dac_desc device.
 * @param buf - Where value is stored.
 * @param len - Maximum length of value to be stored in buf.
 * @param channel - Channel properties.
 * @return Length of chars written in buf, or negative value on failure.
 */
static ssize_t get_voltage_calibscale(void *device, char *buf, size_t len,
				      const struct iio_ch_info *channel)
{
	int32_t val, val2;
	struct iio_axi_dac_desc *iio_dac = (struct iio_axi_dac_desc*)device;
	ssize_t ret = axi_dac_dds_get_calib_scale(iio_dac->dac, channel->ch_num,
			&val, &val2);
	int32_t i = 0;
	if(ret < 0)
		return ret;
	if(val2 < 0 && val >= 0) {
		ret = (ssize_t) snprintf(buf, len, "-");
		if(ret < 0)
			return ret;
		i++;
	}
	ret = i + (ssize_t) snprintf(&buf[i], len, "%"PRIi32".%.6"PRIi32"", val,
				     labs(val2));

	return ret;
}

/**
 * @brief get_dds_calibphase().
 * @param device - Physical instance of a iio_axi_dac_desc device.
 * @param buf - Where value is stored.
 * @param len - Maximum length of value to be stored in buf.
 * @param channel - Channel properties.
 * @return Number of bytes written in buf, or negative value on failure.
 */
static ssize_t get_voltage_calibphase(void *device, char *buf, size_t len,
				      const struct iio_ch_info *channel)
{
	int32_t val, val2;
	int32_t i = 0;
	struct iio_axi_dac_desc* iio_dac = (struct iio_axi_dac_desc*)device;
	ssize_t ret = axi_dac_dds_get_calib_phase(iio_dac->dac, channel->ch_num,
			&val, &val2);
	if(ret < 0)
		return ret;
	if(val2 < 0 && val >= 0) {
		i++;
	}
	return i + snprintf(&buf[i], len, "%"PRIi32".%.6"PRIi32"", val, labs(val2));
}

/**
 * @brief get_dds_sampling_frequency().
 * @param device - Physical instance of a iio_axi_dac_desc device.
 * @param buf - Where value is stored.
 * @param len - Maximum length of value to be stored in buf.
 * @param channel - Channel properties.
 * @return Number of bytes written in buf, or negative value on failure.
 */
static ssize_t get_voltage_sampling_frequency(void *device, char *buf,
		size_t len,
		const struct iio_ch_info *channel)
{
	/* This function doesn't have an equivalent function in axi_dac_core,
	 * and it should be implemented there first */

	return -ENOENT;
}

/**
 * get_dds_altvoltage_phase().
 * @param device - Physical instance of a iio_axi_dac_desc device.
 * @param buf - Where value is stored.
 * @param len - Maximum length of value to be stored in buf.
 * @param channel - Channel properties.
 * @return Number of bytes written in buf, or negative value on failure.
 */
static ssize_t get_altvoltage_phase(void *device, char *buf, size_t len,
				    const struct iio_ch_info *channel)
{
	uint32_t phase;
	struct iio_axi_dac_desc* iio_dac = (struct iio_axi_dac_desc*)device;
	ssize_t ret = axi_dac_dds_get_phase(iio_dac->dac, channel->ch_num, &phase);
	if (ret < 0)
		return ret;

	return snprintf(buf, len, "%"PRIu32"", phase);
}

/**
 * @brief get_dds_altvoltage_scale().
 * @param device - Physical instance of a iio_axi_dac_desc device.
 * @param buf - Where value is stored.
 * @param len - Maximum length of value to be stored in buf.
 * @param channel - Channel properties.
 * @return Number of bytes written in buf, or negative value on failure.
 */
static ssize_t get_altvoltage_scale(void *device, char *buf, size_t len,
				    const struct iio_ch_info *channel)
{
	int32_t scale;
	struct iio_axi_dac_desc* iio_dac = (struct iio_axi_dac_desc*)device;
	ssize_t ret = axi_dac_dds_get_scale(iio_dac->dac, channel->ch_num, &scale);
	if (ret < 0)
		return ret;

	return snprintf(buf, len, "%"PRIi32".%.6"PRIi32"", (scale / 1000000),
			(scale % 1000000));
}

/**
 * @brief get_dds_altvoltage_frequency().
 * @param device - Physical instance of a iio_axi_dac_desc device.
 * @param buf - Where value is stored.
 * @param len - Maximum length of value to be stored in buf.
 * @param channel - Channel properties.
 * @return Number of bytes written in buf, or negative value on failure.
 */
static ssize_t get_altvoltage_frequency(void *device, char *buf, size_t len,
					const struct iio_ch_info *channel)
{
	uint32_t freq;
	struct iio_axi_dac_desc* iio_dac = (struct iio_axi_dac_desc*)device;
	ssize_t ret = axi_dac_dds_get_frequency(iio_dac->dac, channel->ch_num, &freq);
	if (ret < 0)
		return ret;

	return snprintf(buf, len, "%"PRIi32"", freq);
}

/**
 * @brief get_dds_altvoltage_raw().
 * @param device - Physical instance of a iio_axi_dac_desc device.
 * @param buf - Where value is stored.
 * @param len - Maximum length of value to be stored in buf.
 * @param channel - Channel properties.
 * @return Number of bytes written in buf, or negative value on failure.
 */
static ssize_t get_altvoltage_raw(void *device, char *buf, size_t len,
				  const struct iio_ch_info *channel)
{
	/* This function doesn't have an equivalent function in axi_dac_core,
	 * and it should be implemented there first */

	return -ENOENT;
}

/**
 * @brief get_dds_altvoltage_sampling_frequency().
 * @param device - Physical instance of a iio_axi_dac_desc device.
 * @param buf - Where value is stored.
 * @param len - Maximum length of value to be stored in buf.
 * @param channel - Channel properties.
 * @return Number of bytes written in buf, or negative value on failure.
 */
static ssize_t get_altvoltage_sampling_frequency(void *device, char *buf,
		size_t len,
		const struct iio_ch_info *channel)
{
	/* This function doesn't have an equivalent function in axi_dac_core,
	 * and it should be implemented there first */

	return -ENOENT;
}

/**
 * @brief set_dds_calibscale().
 * @param device - Physical instance of a iio_axi_dac_desc device.
 * @param buf - Value to be written to attribute.
 * @param len - Length of the data in "buf".
 * @param channel - Channel properties.
 * @return Number of bytes written in buf, or negative value on failure.
 */
static ssize_t set_voltage_calibscale(void *device, char *buf, size_t len,
				      const struct iio_ch_info *channel)
{
	float calib= strtof(buf, NULL);
	int32_t val = (int32_t)calib;
	int32_t val2 = (int32_t)(calib* 1000000) % 1000000;
	struct iio_axi_dac_desc* iio_dac = (struct iio_axi_dac_desc*)device;
	ssize_t ret = axi_dac_dds_set_calib_scale(iio_dac->dac, channel->ch_num, val,
			val2);
	if (ret < 0)
		return ret;

	return len;
}

/**
 * @brief set_dds_calibphase().
 * @param device - Physical instance of a iio_axi_dac_desc device.
 * @param buf - Value to be written to attribute.
 * @param len - Length of the data in "buf".
 * @param channel - Channel properties.
 * @return Number of bytes written to device, or negative value on failure.
 */
static ssize_t set_voltage_calibphase(void *device, char *buf, size_t len,
				      const struct iio_ch_info *channel)
{
	float calib = strtof(buf, NULL);
	int32_t val = (int32_t)calib;
	int32_t val2 = (int32_t)(calib* 1000000) % 1000000;
	struct iio_axi_dac_desc* iio_dac = (struct iio_axi_dac_desc*)device;
	ssize_t ret = axi_dac_dds_set_calib_phase(iio_dac->dac, channel->ch_num, val,
			val2);
	if (ret < 0)
		return ret;

	return len;
}

/**
 * @brief set_dds_sampling_frequency().
 * @param device - Physical instance of a iio_axi_dac_desc device.
 * @param buf - Value to be written to attribute.
 * @param len - Length of the data in "buf".
 * @param channel - Channel properties.
 * @return: Number of bytes written to device, or negative value on failure.
 */
static ssize_t set_voltage_sampling_frequency(void *device, char *buf,
		size_t len,
		const struct iio_ch_info *channel)
{
	/* This function doesn't have an equivalent function in axi_dac_core,
	 * and it should be implemented there first */

	return -ENOENT;
}

/**
 * @brief set_dds_altvoltage_phase().
 * @param device - Physical instance of a iio_axi_dac_desc device.
 * @param buf - Value to be written to attribute.
 * @param len - Length of the data in "buf".
 * @param channel - Channel properties.
 * @return Number of bytes written to device, or negative value on failure.
 */
static ssize_t set_altvoltage_phase(void *device, char *buf, size_t len,
				    const struct iio_ch_info *channel)
{
	uint32_t phase = srt_to_uint32(buf);
	struct iio_axi_dac_desc * iio_dac = (struct iio_axi_dac_desc *)device;
	ssize_t ret = axi_dac_dds_set_phase(iio_dac->dac, channel->ch_num, phase);
	if (ret < 0)
		return ret;

	return len;
}

/**
 * @brief set_dds_altvoltage_scale().
 * @param device - Physical instance of a iio_axi_dac_desc device.
 * @param buf - Value to be written to attribute.
 * @param len - Length of the data in "buf".
 * @param channel - Channel properties.
 * @return Number of bytes written to device, or negative value on failure.
 */
static ssize_t set_altvoltage_scale(void *device, char *buf, size_t len,
				    const struct iio_ch_info *channel)
{
	float fscale = strtof(buf, NULL);
	int32_t scale = fscale * 1000000;
	struct iio_axi_dac_desc* iio_dac = (struct iio_axi_dac_desc*)device;
	ssize_t ret = axi_dac_dds_set_scale(iio_dac->dac, channel->ch_num, scale);
	if (ret < 0)
		return ret;

	return len;
}

/**
 * @brief set_dds_altvoltage_frequency().
 * @param device - Physical instance of a iio_axi_dac_desc device.
 * @param buf - Value to be written to attribute.
 * @param len - Length of the data in "buf".
 * @param channel - Channel properties.
 * @return Number of bytes written to device, or negative value on failure.
 */
static ssize_t set_altvoltage_frequency(void *device, char *buf, size_t len,
					const struct iio_ch_info *channel)
{
	uint32_t freq = srt_to_uint32(buf);
	struct iio_axi_dac_desc* iio_dac = (struct iio_axi_dac_desc*)device;
	ssize_t ret = axi_dac_dds_set_frequency(iio_dac->dac, channel->ch_num, freq);
	if (ret < 0)
		return ret;

	return len;
}

/**
 * @brief set_dds_altvoltage_raw().
 * @param device - Physical instance of a iio_axi_dac_desc device.
 * @param buf - Value to be written to attribute.
 * @param len - Length of the data in "buf".
 * @param channel - Channel properties.
 * @return: Number of bytes written to device, or negative value on failure.
 */
static ssize_t set_altvoltage_raw(void *device, char *buf, size_t len,
				  const struct iio_ch_info *channel)
{
	uint32_t dds_mode = srt_to_uint32(buf);
	struct iio_axi_dac_desc* iio_dac = (struct iio_axi_dac_desc*)device;
	ssize_t ret;
	ret = axi_dac_set_datasel(iio_dac->dac, -1,
				  dds_mode ? AXI_DAC_DATA_SEL_DDS : AXI_DAC_DATA_SEL_DMA);
	if (ret < 0)
		return ret;

	return len;
}

/**
 * @brief set_dds_altvoltage_sampling_frequency().
 * @param device - Physical instance of a iio_axi_dac_desc device.
 * @param buf - Value to be written to attribute.
 * @param len - Length of the data in "buf".
 * @param channel - Channel properties.
 * @return Number of bytes written to device, or negative value on failure.
 */
static ssize_t set_altvoltage_sampling_frequency(void *device, char *buf,
		size_t len,
		const struct iio_ch_info *channel)
{
	/* This function doesn't have an equivalent function in axi_dac_core,
	 * and it should be implemented there first */

	return -ENOENT;
}

/**
 * @struct iio_attr_voltage_calibphase
 * @brief Structure for "calibphase" attribute.
 */
static struct iio_attribute iio_attr_voltage_calibphase = {
	/** Attribute name */
	.name = "calibphase",
	/** Read attribute from device */
	.show = get_voltage_calibphase,
	/** Write attribute to device */
	.store = set_voltage_calibphase,
};

/**
 * @struct iio_attr_voltage_calibscale
 * @brief Structure for "calibscale" attribute.
 */
static struct iio_attribute iio_attr_voltage_calibscale = {
	/** Attribute name */
	.name = "calibscale",
	/** Read attribute from device */
	.show = get_voltage_calibscale,
	/** Write attribute to device */
	.store = set_voltage_calibscale,

};

/**
 * @struct iio_attr_voltage_sampling_frequency
 * @brief Structure for "sampling_frequency" attribute.
 */
static struct iio_attribute iio_attr_voltage_sampling_frequency = {
	/** Attribute name */
	.name = "sampling_frequency",
	/** Read attribute from device */
	.show = get_voltage_sampling_frequency,
	/** Write attribute to device */
	.store = set_voltage_sampling_frequency,
};

/**
 * @struct iio_attr_altvoltage_raw
 * @brief Structure for "raw" attribute.
 */
static struct iio_attribute iio_attr_altvoltage_raw = {
	/** Attribute name */
	.name = "raw",
	/** Read attribute from device */
	.show = get_altvoltage_raw,
	/** Write attribute to device */
	.store = set_altvoltage_raw,
};

/**
 * @struct iio_attr_altvoltage_phase
 * @brief Structure for "phase" attribute.
 */
static struct iio_attribute iio_attr_altvoltage_phase = {
	/** Attribute name */
	.name = "phase",
	/** Read attribute from device */
	.show = get_altvoltage_phase,
	/** Write attribute to device */
	.store = set_altvoltage_phase,
};

/**
 * @struct iio_attr_altvoltage_frequency
 * @brief Structure for "frequency" attribute.
 */
static struct iio_attribute iio_attr_altvoltage_frequency = {
	/** Attribute name */
	.name = "frequency",
	/** Read attribute from device */
	.show = get_altvoltage_frequency,
	/** Write attribute to device */
	.store = set_altvoltage_frequency,
};

/**
 * @struct iio_attr_altvoltage_scale
 * @brief Structure for "scale" attribute.
 */
static struct iio_attribute iio_attr_altvoltage_scale = {
	/** Attribute name */
	.name = "scale",
	/** Read attribute from device */
	.show = get_altvoltage_scale,
	/** Write attribute to device */
	.store = set_altvoltage_scale,
};

/**
 * @struct iio_attr_altvoltage_sampling_frequency
 * @brief Structure for "sampling_frequency" attribute.
 */
static struct iio_attribute iio_attr_altvoltage_sampling_frequency = {
	/** Attribute name */
	.name = "sampling_frequency",
	/** Read attribute from device */
	.show = get_altvoltage_sampling_frequency,
	/** Write attribute to device */
	.store = set_altvoltage_sampling_frequency,
};

/**
 * List containing attributes, corresponding to "voltage" channels.
 */
static struct iio_attribute *iio_voltage_attributes[] = {
	&iio_attr_voltage_calibscale,
	&iio_attr_voltage_calibphase,
	&iio_attr_voltage_sampling_frequency,
	NULL,
};

/**
 * List containing attributes, corresponding to "altvoltage" channels.
 */
static struct iio_attribute *iio_altvoltage_attributes[] = {
	&iio_attr_altvoltage_raw,
	&iio_attr_altvoltage_phase,
	&iio_attr_altvoltage_scale,
	&iio_attr_altvoltage_frequency,
	&iio_attr_altvoltage_sampling_frequency,
	NULL,
};

/**
 * @brief Transfer data from RAM to device.
 * @param iio_inst - Physical instance of a iio_axi_dac_desc device.
 * @param bytes_count - Number of bytes to transfer.
 * @param ch_mask - Opened channels mask.
 * @return Number of bytes transfered, or negative value in case of failure.
 */
static ssize_t iio_axi_dac_transfer_mem_to_dev(void *iio_inst,
		size_t bytes_count,
		uint32_t ch_mask)
{
	struct iio_axi_dac_desc *iio_dac = iio_inst;
	ssize_t ret, i;

	for (i = 0; i < iio_dac->dac->num_channels; i++) {
		ret = axi_dac_set_datasel(iio_dac->dac, i,
					  (BIT(i) & ch_mask) ? AXI_DAC_DATA_SEL_DMA : AXI_DAC_DATA_SEL_DDS);
		if(ret < 0)
			return ret;
	}

	if(iio_dac->dcache_flush_range)
		iio_dac->dcache_flush_range(iio_dac->dac_ddr_base, bytes_count);

	iio_dac->dmac->flags = DMA_CYCLIC;
	ret = axi_dmac_transfer(iio_dac->dmac, iio_dac->dac_ddr_base,
				bytes_count);
	if(ret < 0)
		return ret;

	return bytes_count;
}

/**
 * @brief Write chunk of data into RAM.
 * This function is probably called multiple times by libtinyiiod before a
 * "iio_transfer_mem_to_dev" call, since we can only write "bytes_count" bytes
 * at a time.
 * @param iio_inst - Physical instance of a iio_axi_dac_desc device.
 * @param buf - Values to write.
 * @param offset - Offset in memory after the nth chunk of data.
 * @param bytes_count - Number of bytes to write.
 * @param ch_mask - Opened channels mask.
 * @return bytes_count or negative value in case of error.
 */
static ssize_t iio_axi_dac_write_dev(void *iio_inst, char *buf,
				     size_t offset,  size_t bytes_count, uint32_t ch_mask)
{
	struct iio_axi_dac_desc *iio_dac = iio_inst;

	memcpy((void *)(iio_dac->dac_ddr_base + offset), buf, bytes_count);

	return bytes_count;
}

enum ch_type {
	CH_VOLTGE,
	CH_ALTVOLTGE,
};

/**
 * @brief Delete iio_device.
 * @param iio_device - Structure describing a device, channels and attributes.
 * @return SUCCESS in case of success or negative value otherwise.
 */
static ssize_t iio_axi_dac_delete_device_descriptor(
	struct iio_device *iio_device)
{
	uint16_t i = 0;

	if (!iio_device)
		return FAILURE;

	if (iio_device->channels) {
		while (iio_device->channels[i]) {
			if (iio_device->channels[i]->name)
				free(iio_device->channels[i]->name);
			free(iio_device->channels[i]);
			i++;
		}
		free(iio_device->channels);
	}

	return SUCCESS;
}

/**
 * @brief Create structure describing a device, channels
 * and attributes.
 * @param adc - Axi adc.
 * @param iio_device - iio device.
 * @return iio_device or NULL, in case of failure.
 */
static int32_t iio_axi_dac_create_device_descriptor(
	struct axi_dac *adc, struct iio_device *iio_device)
{
	static struct scan_type scan_type = {
		.sign = 's',
		.realbits = 16,
		.storagebits = 16,
		.shift = 0,
		.is_big_endian = false
	};

	static struct iio_channel default_voltage_channel = {
		.ch_type = IIO_VOLTAGE,
		.scan_type = &scan_type,
		.attributes = iio_voltage_attributes,
		.ch_out = true,
		.indexed = true,
	};

	static struct iio_channel default_altvoltage_channel = {
		.ch_type = IIO_ALTVOLTAGE,
		.scan_type = NULL,
		.attributes = iio_altvoltage_attributes,
		.ch_out = true,
		.indexed = true,
	};

	int32_t i, altvoltage_ch, voltage_ch_no, altvoltage_ch_no;
	int32_t ret;

	voltage_ch_no = adc->num_channels;
	altvoltage_ch_no = adc->num_channels * 2;
	iio_device->num_ch = voltage_ch_no + altvoltage_ch_no;
	iio_device->attributes = NULL; /* no device attribute */
	iio_device->channels = calloc(iio_device->num_ch + 1,
				      sizeof(struct iio_channel *));
	if (!iio_device->channels)
		goto error;

	for (i = 0; i < voltage_ch_no; i++) {
		iio_device->channels[i] = calloc(1, sizeof(struct iio_channel));
		if (!iio_device->channels[i])
			goto error;
		default_voltage_channel.channel = i;
		*(iio_device->channels[i]) = default_voltage_channel;

		iio_device->channels[i]->name = calloc(5, 1);
		if (!iio_device->channels[i]->name)
			goto error;

		iio_device->channels[i]->scan_index = i;
		ret = sprintf(iio_device->channels[i]->name, "voltage%"PRIi32"", i);
		if (ret < 0)
			goto error;
	}

	for (i = voltage_ch_no; i < voltage_ch_no + altvoltage_ch_no; i++) {
		iio_device->channels[i] = calloc(1, sizeof(struct iio_channel));
		if (!iio_device->channels[i])
			goto error;
		altvoltage_ch = i - voltage_ch_no;
		default_altvoltage_channel.channel = altvoltage_ch;
		*(iio_device->channels[i]) = default_altvoltage_channel;

		iio_device->channels[i]->name = calloc(5, 1);
		if (!iio_device->channels[i]->name)
			goto error;

		iio_device->channels[i]->scan_index = altvoltage_ch;
		ret = sprintf(iio_device->channels[i]->name, "altvoltage%"PRIi32"",
			      altvoltage_ch);
		if (ret < 0)
			goto error;
	}
	iio_device->channels[i] = NULL;

	iio_device->transfer_dev_to_mem = NULL;
	iio_device->transfer_mem_to_dev = iio_axi_dac_transfer_mem_to_dev;
	iio_device->read_data = NULL;
	iio_device->write_data = iio_axi_dac_write_dev;

	return SUCCESS;

error:
	iio_axi_dac_delete_device_descriptor(iio_device);

	return FAILURE;
}

/**
 * @brief Create structure describing a device, channels
 * and attributes.
 * @param desc - Descriptor.
 * @param dev_descriptor - device descriptor.
 * @return None.
 */
void iio_axi_dac_get_dev_descriptor(struct iio_axi_dac_desc *desc,
				    struct iio_device **dev_descriptor)
{
	*dev_descriptor = &desc->dev_descriptor;
}

/**
 * @brief Registers a iio_axi_dac_desc for reading/writing and parameterization
 * of axi_dac device.
 * @param desc - Descriptor.
 * @param init - Configuration structure.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t iio_axi_dac_init(struct iio_axi_dac_desc **desc,
			 struct iio_axi_dac_init_param *init)
{
	struct iio_axi_dac_desc *iio_axi_dac_inst;
	int32_t status;

	if (!init)
		return FAILURE;

	if (!init->tx_dac || !init->tx_dmac)
		return FAILURE;

	iio_axi_dac_inst = (struct iio_axi_dac_desc *)calloc(1,
			   sizeof(struct iio_axi_dac_desc));
	if (!iio_axi_dac_inst)
		return FAILURE;

	iio_axi_dac_inst->dac = init->tx_dac;
	iio_axi_dac_inst->dmac = init->tx_dmac;
	iio_axi_dac_inst->dac_ddr_base = init->dac_ddr_base;
	iio_axi_dac_inst->dcache_flush_range = init->dcache_flush_range;

	status = iio_axi_dac_create_device_descriptor(iio_axi_dac_inst->dac,
			&iio_axi_dac_inst->dev_descriptor);
	if (IS_ERR_VALUE(status)) {
		free(iio_axi_dac_inst);
		return status;
	}

	*desc = iio_axi_dac_inst;

	return SUCCESS;
}

/**
 * @brief Release resources.
 * @param desc - Descriptor.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t iio_axi_dac_remove(struct iio_axi_dac_desc *desc)
{
	int32_t status;

	if (!desc)
		return FAILURE;

	status = iio_axi_dac_delete_device_descriptor(&desc->dev_descriptor);
	if (status < 0)
		return status;

	free(desc);

	return SUCCESS;
}
