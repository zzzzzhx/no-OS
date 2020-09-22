#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <math.h>
#include "error.h"
#include "iio.h"
#include "iio_ad7124.h"
#include "util.h"

static ssize_t ad1724_iio_read_configuration(void *device, char *buf, size_t len,
			const struct iio_ch_info *channel);
static ssize_t ad1724_iio_change_configuration(void *device, char *buf, size_t len,
			const struct iio_ch_info *channel);
static ssize_t ad1724_iio_read_filter(void *device, char *buf, size_t len,
			const struct iio_ch_info *channel);
static ssize_t ad1724_iio_change_filter(void *device, char *buf, size_t len,
			const struct iio_ch_info *channel);
static ssize_t ad1724_iio_read_offset(void *device, char *buf, size_t len,
			const struct iio_ch_info *channel);
static ssize_t ad1724_iio_change_offset(void *device, char *buf, size_t len,
			const struct iio_ch_info *channel);
static ssize_t ad1724_iio_read_gain(void *device, char *buf, size_t len,
			const struct iio_ch_info *channel);
static ssize_t ad1724_iio_change_gain(void *device, char *buf, size_t len,
			const struct iio_ch_info *channel);
static ssize_t ad1724_iio_read_status(void *device, char *buf, size_t len,
			const struct iio_ch_info *channel);
static ssize_t ad1724_iio_read_adc_ctrl(void *device, char *buf, size_t len,
			const struct iio_ch_info *channel);
static ssize_t ad1724_iio_change_adc_ctrl(void *device, char *buf, size_t len,
			const struct iio_ch_info *channel);
static ssize_t ad1724_iio_read_ioctrl1(void *device, char *buf, size_t len,
			const struct iio_ch_info *channel);
static ssize_t ad1724_iio_change_ioctrl1(void *device, char *buf, size_t len,
			const struct iio_ch_info *channel);
static ssize_t ad1724_iio_read_ioctrl2(void *device, char *buf, size_t len,
			const struct iio_ch_info *channel);
static ssize_t ad1724_iio_change_ioctrl2(void *device, char *buf, size_t len,
			const struct iio_ch_info *channel);
static ssize_t ad1724_iio_read_id(void *device, char *buf, size_t len,
			const struct iio_ch_info *channel);
static ssize_t ad1724_iio_read_error(void *device, char *buf, size_t len,
			const struct iio_ch_info *channel);
static ssize_t ad1724_iio_read_error_en(void *device, char *buf, size_t len,
			const struct iio_ch_info *channel);
static ssize_t ad1724_iio_change_error_en(void *device, char *buf, size_t len,
			const struct iio_ch_info *channel);
static ssize_t ad1724_iio_read_chan_setup(void *device, char *buf, size_t len,
			const struct iio_ch_info *channel);
static ssize_t ad1724_iio_change_chan_setup(void *device, char *buf, size_t len,
			const struct iio_ch_info *channel);
static ssize_t ad1724_iio_read_chan_ainp(void *device, char *buf, size_t len,
			const struct iio_ch_info *channel);
static ssize_t ad1724_iio_change_chan_ainp(void *device, char *buf, size_t len,
			const struct iio_ch_info *channel);
static ssize_t ad1724_iio_read_chan_ainm(void *device, char *buf, size_t len,
			const struct iio_ch_info *channel);
static ssize_t ad1724_iio_change_chan_ainm(void *device, char *buf, size_t len,
			const struct iio_ch_info *channel);
static ssize_t ad1724_iio_read_filter_3db(void *device, char *buf, size_t len,
			const struct iio_ch_info *channel);
static ssize_t ad1724_iio_write_filter_3db(void *device, char *buf, size_t len,
			const struct iio_ch_info *channel);
static ssize_t ad1724_iio_read_offset_chan(void *device, char *buf, size_t len,
			const struct iio_ch_info *channel);
static ssize_t ad1724_iio_change_offset_chan(void *device, char *buf, size_t len,
			const struct iio_ch_info *channel);
static ssize_t ad1724_iio_read_raw_chan(void *device, char *buf, size_t len,
			const struct iio_ch_info *channel);
static ssize_t ad1724_iio_read_odr_chan(void *device, char *buf, size_t len,
			const struct iio_ch_info *channel);
static ssize_t ad1724_iio_change_odr_chan(void *device, char *buf, size_t len,
			const struct iio_ch_info *channel);
static ssize_t ad1724_iio_read_scale_chan(void *device, char *buf, size_t len,
			const struct iio_ch_info *channel);
static ssize_t ad1724_iio_change_scale_chan(void *device, char *buf, size_t len,
			const struct iio_ch_info *channel);

//struct iio_attribute ad7124_chan_setup = {
//	.name = "ad7124_chan_setup",
//	.show = ad1724_iio_read_chan_setup,
//	.store = ad1724_iio_change_chan_setup
//};
//
//struct iio_attribute ad7124_chan_ainp = {
//	.name = "ad7124_chan_ainp",
//	.show = ad1724_iio_read_chan_ainp,
//	.store = ad1724_iio_change_chan_ainp
//};
//
//struct iio_attribute ad7124_chan_ainm = {
//	.name = "ad7124_chan_ainm",
//	.show = ad1724_iio_read_chan_ainm,
//	.store = ad1724_iio_change_chan_ainm
//};
//
/** TODO: attributes are:
 * filter_low_pass_3db_frequency = ?
 * offset = channel offset (how to get one for each?)
 * raw = channel data
 * sampling_frequency = self explanatory (how to get one for each?)
 * scale = value of LSB
 */
//struct iio_attribute *channel_attributes[] = {
//	&ad7124_chan_setup,
//	&ad7124_chan_ainp,
//	&ad7124_chan_ainm,
//	&ad7124_
//	NULL,
//};


struct iio_attribute ad7124_filter_3db = {
	.name = "filter_low_pass_3db_frequency",
	.show = ad1724_iio_read_filter_3db,
	.store = ad1724_iio_write_filter_3db
};

struct iio_attribute ad7124_chan_offset = {
	.name = "offset",
	.show = ad1724_iio_read_offset_chan,
	.store = ad1724_iio_change_offset_chan
};

struct iio_attribute ad7124_chan_raw = {
	.name = "raw",
	.show = ad1724_iio_read_raw_chan,
	.store = NULL
};

struct iio_attribute ad7124_chan_sample_freq = {
	.name = "sampling_frequency",
	.show = ad1724_iio_read_odr_chan,
	.store = ad1724_iio_change_odr_chan
};

struct iio_attribute ad7124_chan_scale = {
	.name = "scale",
	.show = ad1724_iio_read_scale_chan,
	.store = ad1724_iio_change_scale_chan
};

struct iio_attribute *channel_attributes[] = {
	&ad7124_filter_3db,
	&ad7124_chan_offset,
	&ad7124_chan_raw,
	&ad7124_chan_sample_freq,
	&ad7124_chan_scale,
	NULL
};

#define AD7124_IIO_CHANN_DEF(nm, ch) \
	struct iio_channel ad7124_ch_##ch = { \
		.name = nm, \
		.ch_type = IIO_ALTVOLTAGE, \
		.channel = ch, \
		.channel2 = 0, \
		.scan_index = ch, \
		.scan_type = NULL, \
		.attributes = channel_attributes, \
		.ch_out = false, \
		.modified = false, \
		.indexed = true, \
	}

AD7124_IIO_CHANN_DEF("voltage0_voltage1", 0);
AD7124_IIO_CHANN_DEF("voltage2_voltage3", 1);
AD7124_IIO_CHANN_DEF("voltage4_voltage5", 2);
AD7124_IIO_CHANN_DEF("voltage6_voltage7", 3);
AD7124_IIO_CHANN_DEF("voltage8_voltage9", 4);
AD7124_IIO_CHANN_DEF("voltage10_voltage11", 5);
AD7124_IIO_CHANN_DEF("voltage12_voltage13", 6);
AD7124_IIO_CHANN_DEF("voltage14_voltage15", 7);

//struct iio_channel ad7124_ch_0 = {
//	.name = "input voltage0_voltage1",
//	.ch_type = IIO_ALTVOLTAGE,
//	.scan_index = 0,
//	.scan_type = NULL,
//	.attributes = channel_attributes,
//	.ch_out = 0
//};
//
//struct iio_channel ad7124_ch_1 = {
//	.name = "input voltage2_voltage3",
//	.ch_type = IIO_ALTVOLTAGE,
//	.scan_index = 0,
//	.scan_type = NULL,
//	.attributes = channel_attributes,
//	.ch_out = 0
//};
//
//struct iio_channel ad7124_ch_2 = {
//	.name = "input voltage4_voltage5",
//	.ch_type = IIO_ALTVOLTAGE,
//	.scan_index = 0,
//	.scan_type = NULL,
//	.attributes = channel_attributes,
//	.ch_out = 0
//};
//
//struct iio_channel ad7124_ch_3 = {
//	.name = "input voltage6_voltage7",
//	.ch_type = IIO_ALTVOLTAGE,
//	.scan_index = 0,
//	.scan_type = NULL,
//	.attributes = channel_attributes,
//	.ch_out = 0
//};
//
//struct iio_channel ad7124_ch_4 = {
//	.name = "input voltage8_voltage9",
//	.ch_type = IIO_ALTVOLTAGE,
//	.scan_index = 0,
//	.scan_type = NULL,
//	.attributes = channel_attributes,
//	.ch_out = 0
//};
//
//struct iio_channel ad7124_ch_5 = {
//	.name = "input voltage10_voltage11",
//	.ch_type = IIO_ALTVOLTAGE,
//	.scan_index = 0,
//	.scan_type = NULL,
//	.attributes = channel_attributes,
//	.ch_out = 0
//};
//
//struct iio_channel ad7124_ch_6 = {
//	.name = "input voltage12_voltage13",
//	.ch_type = IIO_ALTVOLTAGE,
//	.scan_index = 0,
//	.scan_type = NULL,
//	.attributes = channel_attributes,
//	.ch_out = 0
//};
//
//struct iio_channel ad7124_ch_7 = {
//	.name = "input voltage14_voltage15",
//	.ch_type = IIO_ALTVOLTAGE,
//	.scan_index = 0,
//	.scan_type = NULL,
//	.attributes = channel_attributes,
//	.ch_out = 0
//};
//
//struct iio_channel ad7124_ch_8 = {
//	.name = "CH_8",
//	.ch_type = IIO_ALTVOLTAGE,
//	.scan_index = 0,
//	.scan_type = NULL,
//	.attributes = channel_attributes,
//	.ch_out = 0
//};
//
//struct iio_channel ad7124_ch_9 = {
//	.name = "CH_9",
//	.ch_type = IIO_ALTVOLTAGE,
//	.scan_index = 0,
//	.scan_type = NULL,
//	.attributes = channel_attributes,
//	.ch_out = 0
//};
//
//struct iio_channel ad7124_ch_10 = {
//	.name = "CH_10",
//	.ch_type = IIO_ALTVOLTAGE,
//	.scan_index = 0,
//	.scan_type = NULL,
//	.attributes = channel_attributes,
//	.ch_out = 0
//};
//
//struct iio_channel ad7124_ch_11 = {
//	.name = "CH_11",
//	.ch_type = IIO_ALTVOLTAGE,
//	.scan_index = 0,
//	.scan_type = NULL,
//	.attributes = channel_attributes,
//	.ch_out = 0
//};
//
//struct iio_channel ad7124_ch_12 = {
//	.name = "CH_12",
//	.ch_type = IIO_ALTVOLTAGE,
//	.scan_index = 0,
//	.scan_type = NULL,
//	.attributes = channel_attributes,
//	.ch_out = 0
//};
//
//struct iio_channel ad7124_ch_13 = {
//	.name = "CH_13",
//	.ch_type = IIO_ALTVOLTAGE,
//	.scan_index = 0,
//	.scan_type = NULL,
//	.attributes = channel_attributes,
//	.ch_out = 0
//};
//
//struct iio_channel ad7124_ch_14 = {
//	.name = "CH_14",
//	.ch_type = IIO_ALTVOLTAGE,
//	.scan_index = 0,
//	.scan_type = NULL,
//	.attributes = channel_attributes,
//	.ch_out = 0
//};
//
//struct iio_channel ad7124_ch_15 = {
//	.name = "CH_15",
//	.ch_type = IIO_ALTVOLTAGE,
//	.scan_index = 0,
//	.scan_type = NULL,
//	.attributes = channel_attributes,
//	.ch_out = 0
//};

struct iio_channel *ad7124_channels[] = {
	&ad7124_ch_0,
	&ad7124_ch_1,
	&ad7124_ch_2,
	&ad7124_ch_3,
	&ad7124_ch_4,
	&ad7124_ch_5,
	&ad7124_ch_6,
	&ad7124_ch_7,
//	&ad7124_ch_8,
//	&ad7124_ch_9,
//	&ad7124_ch_10,
//	&ad7124_ch_11,
//	&ad7124_ch_12,
//	&ad7124_ch_13,
//	&ad7124_ch_14,
//	&ad7124_ch_15,
	NULL
};

struct iio_attribute ad7124_status = {
	.name = "ad7124_status",
	.show = ad1724_iio_read_status,
	.store = NULL
};

struct iio_attribute ad7124_adc_ctrl = {
	.name = "ad7124_adc_ctrl",
	.show = ad1724_iio_read_adc_ctrl,
	.store = ad1724_iio_change_adc_ctrl
};

struct iio_attribute ad7124_ioctrl1 = {
	.name = "ad7124_ioctrl1",
	.show = ad1724_iio_read_ioctrl1,
	.store = ad1724_iio_change_ioctrl1
};

struct iio_attribute ad7124_ioctrl2 = {
	.name = "ad7124_ioctrl2",
	.show = ad1724_iio_read_ioctrl2,
	.store = ad1724_iio_change_ioctrl2
};

struct iio_attribute ad7124_id = {
	.name = "ad7124_id",
	.show = ad1724_iio_read_id,
	.store = NULL
};

struct iio_attribute ad7124_error = {
	.name = "ad7124_error",
	.show = ad1724_iio_read_error,
	.store = NULL
};

struct iio_attribute ad7124_error_en = {
	.name = "ad7124_error_en",
	.show = ad1724_iio_read_error_en,
	.store = ad1724_iio_change_error_en
};

struct iio_attribute ad7124_config_0 = {
	.name = "ad7124_config_0",
	.show = ad1724_iio_read_configuration,
	.store = ad1724_iio_change_configuration
};

struct iio_attribute ad7124_config_1 = {
	.name = "ad7124_config_1",
	.show = ad1724_iio_read_configuration,
	.store = ad1724_iio_change_configuration
};

struct iio_attribute ad7124_config_2 = {
	.name = "ad7124_config_2",
	.show = ad1724_iio_read_configuration,
	.store = ad1724_iio_change_configuration
};

struct iio_attribute ad7124_config_3 = {
	.name = "ad7124_config_3",
	.show = ad1724_iio_read_configuration,
	.store = ad1724_iio_change_configuration
};

struct iio_attribute ad7124_config_4 = {
	.name = "ad7124_config_4",
	.show = ad1724_iio_read_configuration,
	.store = ad1724_iio_change_configuration
};

struct iio_attribute ad7124_config_5 = {
	.name = "ad7124_config_5",
	.show = ad1724_iio_read_configuration,
	.store = ad1724_iio_change_configuration
};

struct iio_attribute ad7124_config_6 = {
	.name = "ad7124_config_6",
	.show = ad1724_iio_read_configuration,
	.store = ad1724_iio_change_configuration
};

struct iio_attribute ad7124_config_7 = {
	.name = "ad7124_config_7",
	.show = ad1724_iio_read_configuration,
	.store = ad1724_iio_change_configuration
};

struct iio_attribute ad7124_flt_0 = {
	.name = "ad7124_flt_0",
	.show = ad1724_iio_read_filter,
	.store = ad1724_iio_change_filter
};

struct iio_attribute ad7124_flt_1 = {
	.name = "ad7124_flt_1",
	.show = ad1724_iio_read_filter,
	.store = ad1724_iio_change_filter
};

struct iio_attribute ad7124_flt_2 = {
	.name = "ad7124_flt_2",
	.show = ad1724_iio_read_filter,
	.store = ad1724_iio_change_filter
};

struct iio_attribute ad7124_flt_3 = {
	.name = "ad7124_flt_3",
	.show = ad1724_iio_read_filter,
	.store = ad1724_iio_change_filter
};

struct iio_attribute ad7124_flt_4 = {
	.name = "ad7124_flt_4",
	.show = ad1724_iio_read_filter,
	.store = ad1724_iio_change_filter
};

struct iio_attribute ad7124_flt_5 = {
	.name = "ad7124_flt_5",
	.show = ad1724_iio_read_filter,
	.store = ad1724_iio_change_filter
};

struct iio_attribute ad7124_flt_6 = {
	.name = "ad7124_flt_6",
	.show = ad1724_iio_read_filter,
	.store = ad1724_iio_change_filter
};

struct iio_attribute ad7124_flt_7 = {
	.name = "ad7124_flt_7",
	.show = ad1724_iio_read_filter,
	.store = ad1724_iio_change_filter
};

struct iio_attribute ad7124_off_0 = {
	.name = "ad7124_off_0",
	.show = ad1724_iio_read_offset,
	.store = ad1724_iio_change_offset
};

struct iio_attribute ad7124_off_1 = {
	.name = "ad7124_off_1",
	.show = ad1724_iio_read_offset,
	.store = ad1724_iio_change_offset
};

struct iio_attribute ad7124_off_2 = {
	.name = "ad7124_off_2",
	.show = ad1724_iio_read_offset,
	.store = ad1724_iio_change_offset
};

struct iio_attribute ad7124_off_3 = {
	.name = "ad7124_off_3",
	.show = ad1724_iio_read_offset,
	.store = ad1724_iio_change_offset
};

struct iio_attribute ad7124_off_4 = {
	.name = "ad7124_off_4",
	.show = ad1724_iio_read_offset,
	.store = ad1724_iio_change_offset
};

struct iio_attribute ad7124_off_5 = {
	.name = "ad7124_off_5",
	.show = ad1724_iio_read_offset,
	.store = ad1724_iio_change_offset
};

struct iio_attribute ad7124_off_6 = {
	.name = "ad7124_off_6",
	.show = ad1724_iio_read_offset,
	.store = ad1724_iio_change_offset
};

struct iio_attribute ad7124_off_7 = {
	.name = "ad7124_off_7",
	.show = ad1724_iio_read_offset,
	.store = ad1724_iio_change_offset
};

struct iio_attribute ad7124_gain_0 = {
	.name = "ad7124_gain_0",
	.show = ad1724_iio_read_gain,
	.store = ad1724_iio_change_gain
};

struct iio_attribute ad7124_gain_1 = {
	.name = "ad7124_gain_1",
	.show = ad1724_iio_read_gain,
	.store = ad1724_iio_change_gain
};

struct iio_attribute ad7124_gain_2 = {
	.name = "ad7124_gain_2",
	.show = ad1724_iio_read_gain,
	.store = ad1724_iio_change_gain
};

struct iio_attribute ad7124_gain_3 = {
	.name = "ad7124_gain_3",
	.show = ad1724_iio_read_gain,
	.store = ad1724_iio_change_gain
};

struct iio_attribute ad7124_gain_4 = {
	.name = "ad7124_gain_4",
	.show = ad1724_iio_read_gain,
	.store = ad1724_iio_change_gain
};

struct iio_attribute ad7124_gain_5 = {
	.name = "ad7124_gain_5",
	.show = ad1724_iio_read_gain,
	.store = ad1724_iio_change_gain
};

struct iio_attribute ad7124_gain_6 = {
	.name = "ad7124_gain_6",
	.show = ad1724_iio_read_gain,
	.store = ad1724_iio_change_gain
};

struct iio_attribute ad7124_gain_7 = {
	.name = "ad7124_gain_7",
	.show = ad1724_iio_read_gain,
	.store = ad1724_iio_change_gain
};

struct iio_attribute *ad7124_attributes[] = {
	&ad7124_status,
	&ad7124_adc_ctrl,
	&ad7124_ioctrl1,
	&ad7124_ioctrl2,
	&ad7124_id,
//	&ad7124_error,
//	&ad7124_error_en,
//	&ad7124_config_0,
//	&ad7124_config_1,
//	&ad7124_config_2,
//	&ad7124_config_3,
//	&ad7124_config_4,
//	&ad7124_config_5,
//	&ad7124_config_6,
//	&ad7124_config_7,
//	&ad7124_flt_0,
//	&ad7124_flt_1,
//	&ad7124_flt_2,
//	&ad7124_flt_3,
//	&ad7124_flt_4,
//	&ad7124_flt_5,
//	&ad7124_flt_6,
//	&ad7124_flt_7,
//	&ad7124_off_0,
//	&ad7124_off_1,
//	&ad7124_off_2,
//	&ad7124_off_3,
//	&ad7124_off_4,
//	&ad7124_off_5,
//	&ad7124_off_6,
//	&ad7124_off_7,
//	&ad7124_gain_0,
//	&ad7124_gain_1,
//	&ad7124_gain_2,
//	&ad7124_gain_3,
//	&ad7124_gain_4,
//	&ad7124_gain_5,
//	&ad7124_gain_6,
//	&ad7124_gain_7,
	NULL
};

//struct iio_device iio_ad7124_device = {
//	.num_ch = 16,
//	.channels = ad7124_channels,
//	.attributes = NULL,
//	.debug_attributes = ad7124_attributes,
//	.buffer_attributes = NULL,
//	.transfer_dev_to_mem = NULL,
//	.read_data = NULL,
//	.transfer_mem_to_dev = NULL,
//	.write_data = NULL
//};

static ssize_t ad1724_iio_read_configuration(void *device, char *buf, size_t len,
			const struct iio_ch_info *channel)
{
	struct ad7124_dev	*desc = (struct ad7124_dev *)device;
	uint32_t		value;
	uint8_t			config_no;

	sscanf(buf, "%hhd", &config_no);

	ad7124_read_register(desc, &desc->regs[(AD7124_CFG0_REG + config_no)]);
	value = desc->regs[(AD7124_CFG0_REG + config_no)].value;

	return snprintf(buf, len, "%"PRIX32"", value);
}

static ssize_t ad1724_iio_change_configuration(void *device, char *buf, size_t len,
			const struct iio_ch_info *channel)
{
	struct ad7124_dev	*desc = (struct ad7124_dev *)device;
	uint8_t			chan_no;
	uint16_t		reg_val;

	sscanf(buf, "%hhd %hd", &chan_no, &reg_val);

	desc->regs[(AD7124_CFG0_REG + chan_no)].value = reg_val;

	ad7124_write_register(desc,
			desc->regs[(AD7124_CFG0_REG + chan_no)]);

	return len;
}

static ssize_t ad1724_iio_read_filter(void *device, char *buf, size_t len,
			const struct iio_ch_info *channel)
{
	struct ad7124_dev	*desc = (struct ad7124_dev *)device;
	uint32_t		value;
	uint8_t			flt_no;

	sscanf(buf, "%hhd", &flt_no);

	ad7124_read_register(desc, &desc->regs[(AD7124_FILT0_REG + flt_no)]);
	value = desc->regs[(AD7124_FILT0_REG + flt_no)].value;

	return snprintf(buf, len, "%"PRIX32"", value);
}

static ssize_t ad1724_iio_change_filter(void *device, char *buf, size_t len,
			const struct iio_ch_info *channel)
{
	struct ad7124_dev	*desc = (struct ad7124_dev *)device;
	uint8_t			flt_no;
	uint32_t		reg_val;

	sscanf(buf, "%hhd %ld", &flt_no, &reg_val);

	desc->regs[(AD7124_FILT0_REG + flt_no)].value = reg_val;

	ad7124_write_register(desc,
			desc->regs[(AD7124_FILT0_REG + flt_no)]);

	return len;
}


static ssize_t ad1724_iio_read_offset_chan(void *device, char *buf, size_t len,
			const struct iio_ch_info *channel)
{
	struct ad7124_dev	*desc = (struct ad7124_dev *)device;
	uint32_t		value;

	ad7124_read_register(desc, &desc->regs[(AD7124_OFFS0_REG + channel->ch_num)]);
	value = desc->regs[(AD7124_OFFS0_REG + channel->ch_num)].value;

	return snprintf(buf, len, "%"PRIX32"", value);
}

static ssize_t ad1724_iio_change_offset_chan(void *device, char *buf, size_t len,
			const struct iio_ch_info *channel)
{
	struct ad7124_dev	*desc = (struct ad7124_dev *)device;
	uint32_t		reg_val;

	sscanf(buf, "%ld", &reg_val);

	desc->regs[(AD7124_OFFS0_REG + channel->ch_num)].value = reg_val;

	ad7124_write_register(desc,
			desc->regs[(AD7124_OFFS0_REG + channel->ch_num)]);

	return len;
}

static ssize_t ad1724_iio_read_raw_chan(void *device, char *buf, size_t len,
			const struct iio_ch_info *channel)
{
	struct ad7124_dev	*desc = (struct ad7124_dev *)device;
	int32_t			value;

	ad7124_read_register(desc, &desc->regs[(AD7124_CH0_MAP_REG + channel->ch_num)]);
	desc->regs[(AD7124_OFFS0_REG + channel->ch_num)].value |= AD7124_CH_MAP_REG_CH_ENABLE;
	ad7124_write_register(desc, desc->regs[(AD7124_CH0_MAP_REG + channel->ch_num)]);

	ad7124_wait_for_conv_ready(desc, 10000);
	ad7124_read_data(desc, &value);

	desc->regs[(AD7124_OFFS0_REG + channel->ch_num)].value &= ~AD7124_CH_MAP_REG_CH_ENABLE;
	ad7124_write_register(desc, desc->regs[(AD7124_CH0_MAP_REG + channel->ch_num)]);

	return snprintf(buf, len, "%"PRIX32"", (uint32_t)value);
}

static ssize_t ad1724_iio_read_offset(void *device, char *buf, size_t len,
			const struct iio_ch_info *channel)
{
	struct ad7124_dev	*desc = (struct ad7124_dev *)device;
	uint32_t		value;
	uint8_t			offset_no;

	sscanf(buf, "%hhd", &offset_no);

	ad7124_read_register(desc, &desc->regs[(AD7124_OFFS0_REG + offset_no)]);
	value = desc->regs[(AD7124_OFFS0_REG + offset_no)].value;

	return snprintf(buf, len, "%"PRIX32"", value);
}

static ssize_t ad1724_iio_change_offset(void *device, char *buf, size_t len,
			const struct iio_ch_info *channel)
{
	struct ad7124_dev	*desc = (struct ad7124_dev *)device;
	uint8_t			offset_no;
	uint32_t		reg_val;

	sscanf(buf, "%hhd %ld", &offset_no, &reg_val);

	desc->regs[(AD7124_OFFS0_REG + offset_no)].value = reg_val;

	ad7124_write_register(desc,
			desc->regs[(AD7124_OFFS0_REG + offset_no)]);

	return len;
}

static ssize_t ad1724_iio_read_gain(void *device, char *buf, size_t len,
			const struct iio_ch_info *channel)
{
	struct ad7124_dev	*desc = (struct ad7124_dev *)device;
	uint32_t		value;
	uint8_t			gain_no;

	sscanf(buf, "%hhd", &gain_no);

	ad7124_read_register(desc, &desc->regs[(AD7124_GAIN0_REG + gain_no)]);
	value = desc->regs[(AD7124_GAIN0_REG + gain_no)].value;

	return snprintf(buf, len, "%"PRIX32"", value);
}

static ssize_t ad1724_iio_change_gain(void *device, char *buf, size_t len,
			const struct iio_ch_info *channel)
{
	struct ad7124_dev	*desc = (struct ad7124_dev *)device;
	uint8_t			gain_no;
	uint32_t		reg_val;

	sscanf(buf, "%hhd %ld", &gain_no, &reg_val);

	desc->regs[(AD7124_GAIN0_REG + gain_no)].value = reg_val;

	ad7124_write_register(desc,
			desc->regs[(AD7124_GAIN0_REG + gain_no)]);

	return len;
}

static ssize_t ad1724_iio_read_status(void *device, char *buf, size_t len,
			const struct iio_ch_info *channel)
{
	struct ad7124_dev	*desc = (struct ad7124_dev *)device;
	uint32_t		value;

	ad7124_read_register(desc, &desc->regs[AD7124_STATUS_REG]);
	value = desc->regs[AD7124_STATUS_REG].value;

	return snprintf(buf, len, "%"PRIX32"", value);
}

static ssize_t ad1724_iio_read_adc_ctrl(void *device, char *buf, size_t len,
			const struct iio_ch_info *channel)
{
	struct ad7124_dev	*desc = (struct ad7124_dev *)device;
	uint32_t		value;

	ad7124_read_register(desc, &desc->regs[AD7124_ADC_CTRL_REG]);
	value = desc->regs[AD7124_ADC_CTRL_REG].value;

	return snprintf(buf, len, "%"PRIX32"", value);
}

static ssize_t ad1724_iio_change_adc_ctrl(void *device, char *buf, size_t len,
			const struct iio_ch_info *channel)
{
	struct ad7124_dev	*desc = (struct ad7124_dev *)device;
	uint16_t		reg_val;

	sscanf(buf, "%hd", &reg_val);

	desc->regs[AD7124_ADC_CTRL_REG].value = reg_val;

	ad7124_write_register(desc, desc->regs[AD7124_ADC_CTRL_REG]);

	return len;
}

static ssize_t ad1724_iio_read_ioctrl1(void *device, char *buf, size_t len,
			const struct iio_ch_info *channel)
{
	struct ad7124_dev	*desc = (struct ad7124_dev *)device;
	uint32_t		value;

	ad7124_read_register(desc, &desc->regs[AD7124_IO_CTRL1_REG]);
	value = desc->regs[AD7124_IO_CTRL1_REG].value;

	return snprintf(buf, len, "%"PRIX32"", value);
}

static ssize_t ad1724_iio_change_ioctrl1(void *device, char *buf, size_t len,
			const struct iio_ch_info *channel)
{
	struct ad7124_dev	*desc = (struct ad7124_dev *)device;
	uint32_t		reg_val;

	sscanf(buf, "%ld", &reg_val);

	desc->regs[AD7124_IO_CTRL1_REG].value = reg_val;

	ad7124_write_register(desc, desc->regs[AD7124_IO_CTRL1_REG]);

	return len;
}

static ssize_t ad1724_iio_read_ioctrl2(void *device, char *buf, size_t len,
			const struct iio_ch_info *channel)
{
	struct ad7124_dev	*desc = (struct ad7124_dev *)device;
	uint32_t		value;

	ad7124_read_register(desc, &desc->regs[AD7124_IO_CTRL2_REG]);
	value = desc->regs[AD7124_IO_CTRL2_REG].value;

	return snprintf(buf, len, "%"PRIX32"", value);
}

static ssize_t ad1724_iio_change_ioctrl2(void *device, char *buf, size_t len,
			const struct iio_ch_info *channel)
{
	struct ad7124_dev	*desc = (struct ad7124_dev *)device;
	uint16_t		reg_val;

	sscanf(buf, "%hd", &reg_val);

	desc->regs[AD7124_IO_CTRL2_REG].value = reg_val;

	ad7124_write_register(desc, desc->regs[AD7124_IO_CTRL2_REG]);

	return len;
}

static ssize_t ad1724_iio_read_id(void *device, char *buf, size_t len,
			const struct iio_ch_info *channel)
{
	struct ad7124_dev	*desc = (struct ad7124_dev *)device;
	uint32_t		value;

	ad7124_read_register(desc, &desc->regs[AD7124_ID_REG]);
	value = desc->regs[AD7124_ID_REG].value;

	return snprintf(buf, len, "%"PRIX32"", value);
}

static ssize_t ad1724_iio_read_error(void *device, char *buf, size_t len,
			const struct iio_ch_info *channel)
{
	struct ad7124_dev	*desc = (struct ad7124_dev *)device;
	uint32_t		value;

	ad7124_read_register(desc, &desc->regs[AD7124_ERR_REG]);
	value = desc->regs[AD7124_ERR_REG].value;

	return snprintf(buf, len, "%"PRIX32"", value);
}

static ssize_t ad1724_iio_read_error_en(void *device, char *buf, size_t len,
			const struct iio_ch_info *channel)
{
	struct ad7124_dev	*desc = (struct ad7124_dev *)device;
	uint32_t		value;

	ad7124_read_register(desc, &desc->regs[AD7124_ERREN_REG]);
	value = desc->regs[AD7124_ERREN_REG].value;

	return snprintf(buf, len, "%"PRIX32"", value);
}

static ssize_t ad1724_iio_change_error_en(void *device, char *buf, size_t len,
			const struct iio_ch_info *channel)
{
	struct ad7124_dev	*desc = (struct ad7124_dev *)device;
	uint32_t		reg_val;

	sscanf(buf, "%ld", &reg_val);

	desc->regs[AD7124_ERREN_REG].value = reg_val;

	ad7124_write_register(desc, desc->regs[AD7124_ERREN_REG]);

	return len;
}


/**
 * @brief Get the AD7124 reference clock.
 * @param [in] dev - Pointer to the application handler.
 * @param [out] f_clk - Pointer to the clock frequency container.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
static int32_t ad7124_8pmdz_fclk_logic_get(struct ad7124_dev *dev,
		float *f_clk)
{
	int32_t ret;
	const float	f_clk_fp = 614400,
			f_clk_mp = 153600,
			f_clk_lp = 76800;
	struct ad7124_st_reg *reg_map;

	reg_map = dev->regs;

	ret = ad7124_read_register(dev, &reg_map[AD7124_ADC_Control]);
	if (ret != SUCCESS)
		return FAILURE;

	switch ((reg_map[AD7124_ADC_Control].value &
		 AD7124_ADC_CTRL_REG_POWER_MODE(3)) >> 6) {
	case 0:
		*f_clk = f_clk_lp;
		break;
	case 1:
		*f_clk = f_clk_mp;
		break;
	case 2:
	case 3:
		*f_clk = f_clk_fp;
		break;
	default:
		return FAILURE;
	}

	return SUCCESS;
}

/**
 * @brief Get the filter coefficient for the sample rate.
 * @param [in] dev - Pointer to the application handler.
 * @param [out] flt_coff - Pointer to the filter coefficient container.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
static int32_t ad7124_8pmdz_fltcoff_logic_get(struct ad7124_dev *dev,
		int16_t ch_no, uint16_t *flt_coff)
{
	struct ad7124_st_reg *reg_map;
	uint16_t power_mode;

	reg_map = dev->regs;

	ad7124_read_register(dev, &reg_map[AD7124_ADC_Control]);

	power_mode = (reg_map[AD7124_ADC_Control].value &
		      AD7124_ADC_CTRL_REG_POWER_MODE(3)) >> 6;

	ad7124_read_register(dev, &reg_map[AD7124_Filter_0 + ch_no]);

	*flt_coff = 32;
	if (reg_map[AD7124_Filter_0 + ch_no].value & AD7124_FILT_REG_SINGLE_CYCLE) {
		if ((reg_map[AD7124_Filter_0 + ch_no].value &
		     AD7124_FILT_REG_FILTER(7)) ==
		    AD7124_FILT_REG_FILTER(0))
			*flt_coff *= 4;
		if ((reg_map[AD7124_Filter_0 + ch_no].value &
		     AD7124_FILT_REG_FILTER(7)) ==
		    AD7124_FILT_REG_FILTER(2))
			*flt_coff *= 3;
	}
	if ((reg_map[AD7124_Filter_0 + ch_no].value & AD7124_FILT_REG_FILTER(7)) ==
	    AD7124_FILT_REG_FILTER(4)) {
		if (power_mode == 0)
			*flt_coff *= 11;
		else
			*flt_coff *= 19;
	}
	if ((reg_map[AD7124_Filter_0 + ch_no].value & AD7124_FILT_REG_FILTER(7)) ==
	    AD7124_FILT_REG_FILTER(5)) {
		if (power_mode == 0)
			*flt_coff *= 10;
		else
			*flt_coff *= 18;
	}

	return SUCCESS;
}

static float _ad7124_iio_get_odr(struct ad7124_dev *dev, int16_t ch_no)
{
	float f_clk;
	struct ad7124_st_reg *reg_map;
	uint16_t fs_value, flt_coff;

	reg_map = dev->regs;

	ad7124_8pmdz_fclk_logic_get(dev, &f_clk);

	ad7124_read_register(dev, &reg_map[AD7124_Filter_0 + ch_no]);

	fs_value = reg_map[AD7124_Filter_0 + ch_no].value & AD7124_FILT_REG_FS(0x7FF);

	if ((reg_map[AD7124_Filter_0 + ch_no].value & AD7124_FILT_REG_FILTER(7)) ==
	    AD7124_FILT_REG_FILTER(7)) {
		switch ((reg_map[AD7124_Filter_0 + ch_no].value &
			 AD7124_FILT_REG_POST_FILTER(7)) >> 17) {
		case 2:
			return 27.27;
		case 3:
			return 25;
		case 5:
			return 20;
		case 6:
			return 16.7;
		default:
			return FAILURE;
		}
	}

	ad7124_8pmdz_fltcoff_logic_get(dev, ch_no, &flt_coff);

	return (f_clk / (float)(flt_coff * fs_value));
}

static float _ad7124_iio_set_odr(struct ad7124_dev *dev, float odr,
		int16_t ch_no)
{

	float f_clk;
	uint16_t flt_coff, fs_value;
	struct ad7124_st_reg *reg_map;

	reg_map = dev->regs;

	ad7124_8pmdz_fclk_logic_get(dev, &f_clk);

	ad7124_8pmdz_fltcoff_logic_get(dev, ch_no, &flt_coff);

	fs_value = (uint16_t)(f_clk / (flt_coff * odr));
	if (fs_value == 0)
		fs_value = 1;
	if (fs_value > 2047)
		fs_value = 2047;

	ad7124_read_register(dev, &reg_map[AD7124_Filter_0 + ch_no]);
	reg_map[AD7124_Filter_0 + ch_no].value &= ~AD7124_FILT_REG_FS(0x7FF);
	reg_map[AD7124_Filter_0 + ch_no].value |= AD7124_FILT_REG_FS(fs_value);

	ad7124_write_register(dev, reg_map[AD7124_Filter_0 + ch_no]);

	return SUCCESS;
}

static ssize_t ad1724_iio_read_filter_3db(void *device, char *buf, size_t len,
			const struct iio_ch_info *channel)
{
	struct ad7124_dev	*desc = (struct ad7124_dev *)device;
	uint32_t		value, odr;

	odr = (uint32_t)_ad7124_iio_get_odr(desc, channel->ch_num);
	ad7124_read_register(desc,
			&desc->regs[(AD7124_FILT0_REG + channel->ch_num)]);
	value = (desc->regs[(AD7124_FILT0_REG + channel->ch_num)].value >> 21) & 0x7;
	switch (value) {
	case 0:
	case 4:
		value = odr * 262 / 1000;
		break;
	case 2:
	case 5:
		value = odr * 230 / 1000;
		break;
	default:
		return -EINVAL;
	}

	return snprintf(buf, len, "%"PRId32"", value);
}

static ssize_t ad1724_iio_write_filter_3db(void *device, char *buf, size_t len,
			const struct iio_ch_info *channel)
{
	struct ad7124_dev	*desc = (struct ad7124_dev *)device;
	uint32_t		sinc4_3db_odr, sinc3_3db_odr, freq;
	uint32_t		new_filter, new_odr;

	sscanf(buf, "%ld", &freq);

	sinc4_3db_odr = (freq * 1000) / 230;
	sinc3_3db_odr = (freq * 1000) / 262;

	if (sinc4_3db_odr > sinc3_3db_odr) {
		new_filter = 2;
		new_odr = sinc3_3db_odr;
	} else {
		new_filter = 0;
		new_odr = sinc4_3db_odr;
	}

	desc->regs[(AD7124_FILT0_REG + channel->ch_num)].value &= ~AD7124_FILT_REG_FILTER(~0);
	desc->regs[(AD7124_FILT0_REG + channel->ch_num)].value |= AD7124_FILT_REG_FILTER(new_filter);
	ad7124_write_register(desc,
			desc->regs[(AD7124_FILT0_REG + channel->ch_num)]);

	_ad7124_iio_set_odr(desc, (float)new_odr, channel->ch_num);

	return len;
}

static ssize_t ad1724_iio_read_odr_chan(void *device, char *buf, size_t len,
			const struct iio_ch_info *channel)
{
	struct ad7124_dev	*desc = (struct ad7124_dev *)device;
	uint32_t		odr;

	odr = (uint32_t)_ad7124_iio_get_odr(desc, channel->ch_num);

	return snprintf(buf, len, "%"PRId32"", odr);
}

static ssize_t ad1724_iio_change_odr_chan(void *device, char *buf, size_t len,
			const struct iio_ch_info *channel)
{
	struct ad7124_dev	*desc = (struct ad7124_dev *)device;
	uint32_t		new_odr;

	sscanf(buf, "%ld", &new_odr);

	_ad7124_iio_set_odr(desc, (float)new_odr, channel->ch_num);

	return len;
}

static ssize_t ad1724_iio_read_scale_chan(void *device, char *buf, size_t len,
			const struct iio_ch_info *channel)
{
	struct ad7124_dev	*desc = (struct ad7124_dev *)device;
	uint32_t		vref_mv = 2500, adc_bit_no = 24;
	uint32_t		pga_bits, bipolar;
	float			lsb_val;

	ad7124_read_register(desc, &desc->regs[AD7124_CFG0_REG + channel->ch_num]);
	pga_bits = desc->regs[AD7124_CFG0_REG + channel->ch_num].value & 0x7;
	bipolar = (desc->regs[AD7124_CFG0_REG + channel->ch_num].value >> 11) & 0x1;
	if (bipolar != 0)
		lsb_val = vref_mv / pow(2, (adc_bit_no + pga_bits - 1));
	else
		lsb_val = vref_mv / pow(2, (adc_bit_no + pga_bits));

	return snprintf(buf, len, "%f", lsb_val);
}

static uint32_t ad7124_iio_find_closest_gain(uint32_t new_gain)
{
	uint32_t new_pga_bits;
	uint32_t old_diff = 0xFFFFFFFF, new_diff;
	uint32_t i;

	for (i = 0; i < 8; i++) {
		new_diff = abs(pow(2, i) - new_gain);
		if (new_diff < old_diff) {
			old_diff = new_diff;
			new_pga_bits = i;
		}
	}

	return new_pga_bits;
}

static ssize_t ad1724_iio_change_scale_chan(void *device, char *buf, size_t len,
			const struct iio_ch_info *channel)
{
	struct ad7124_dev	*desc = (struct ad7124_dev *)device;
	float			new_scale;
	uint32_t		vref_mv = 2500, adc_bit_no = 24;
	uint32_t		bipolar;
	float			lsb_val;
	uint32_t		new_gain;

	sscanf(buf, "%f", &new_scale);

	ad7124_read_register(desc, &desc->regs[AD7124_CFG0_REG + channel->ch_num]);
	bipolar = (desc->regs[AD7124_CFG0_REG + channel->ch_num].value >> 11) & 0x1;
	if (bipolar != 0)
		lsb_val = vref_mv / pow(2, (adc_bit_no - 1));
	else
		lsb_val = vref_mv / pow(2, adc_bit_no);

	new_gain = lsb_val / new_scale;

	new_gain = ad7124_iio_find_closest_gain(new_gain);
	desc->regs[AD7124_CFG0_REG + channel->ch_num].value &= ~AD7124_CFG_REG_PGA(~0);
	desc->regs[AD7124_CFG0_REG + channel->ch_num].value |= AD7124_CFG_REG_PGA(new_gain);
	ad7124_write_register(desc, desc->regs[AD7124_CFG0_REG + channel->ch_num]);

	return len;
}

static ssize_t ad1724_iio_read_chan_setup(void *device, char *buf, size_t len,
			const struct iio_ch_info *channel)
{
	struct ad7124_dev	*desc = (struct ad7124_dev *)device;
	uint32_t		value;

	ad7124_read_register(desc,
			&desc->regs[(AD7124_CH0_MAP_REG + channel->ch_num)]);
	value = desc->regs[(AD7124_CH0_MAP_REG + channel->ch_num)].value;

	return snprintf(buf, len, "%"PRIX32"", ((value >> 12) & 0x7));
}

static ssize_t ad1724_iio_change_chan_setup(void *device, char *buf, size_t len,
			const struct iio_ch_info *channel)
{
	struct ad7124_dev	*desc = (struct ad7124_dev *)device;
	uint16_t		reg_val;

	sscanf(buf, "%hd", &reg_val);

	desc->regs[AD7124_CH0_MAP_REG + channel->ch_num].value &=
		~AD7124_CH_MAP_REG_SETUP(0x7);
	desc->regs[AD7124_CH0_MAP_REG + channel->ch_num].value |=
		AD7124_CH_MAP_REG_SETUP(reg_val);

	ad7124_write_register(desc,
			desc->regs[AD7124_CH0_MAP_REG + channel->ch_num]);

	return len;
}

static ssize_t ad1724_iio_read_chan_ainp(void *device, char *buf, size_t len,
			const struct iio_ch_info *channel)
{
	struct ad7124_dev	*desc = (struct ad7124_dev *)device;
	uint32_t		value;

	ad7124_read_register(desc,
			&desc->regs[(AD7124_CH0_MAP_REG + channel->ch_num)]);
	value = desc->regs[(AD7124_CH0_MAP_REG + channel->ch_num)].value;

	return snprintf(buf, len, "%"PRIX32"", ((value >> 5) & 0x1f));
}

static ssize_t ad1724_iio_change_chan_ainp(void *device, char *buf, size_t len,
			const struct iio_ch_info *channel)
{
	struct ad7124_dev	*desc = (struct ad7124_dev *)device;
	uint16_t		reg_val;

	sscanf(buf, "%hd", &reg_val);

	desc->regs[AD7124_CH0_MAP_REG + channel->ch_num].value &=
		~AD7124_CH_MAP_REG_AINP(0x1F);
	desc->regs[AD7124_CH0_MAP_REG + channel->ch_num].value |=
		AD7124_CH_MAP_REG_AINP(reg_val);

	ad7124_write_register(desc,
			desc->regs[AD7124_CH0_MAP_REG + channel->ch_num]);

	return len;
}

static ssize_t ad1724_iio_read_chan_ainm(void *device, char *buf, size_t len,
			const struct iio_ch_info *channel)
{
	struct ad7124_dev	*desc = (struct ad7124_dev *)device;
	uint32_t		value;

	ad7124_read_register(desc,
			&desc->regs[(AD7124_CH0_MAP_REG + channel->ch_num)]);
	value = desc->regs[(AD7124_CH0_MAP_REG + channel->ch_num)].value;

	return snprintf(buf, len, "%"PRIX32"", (value & 0x1f));
}

static ssize_t ad1724_iio_change_chan_ainm(void *device, char *buf, size_t len,
			const struct iio_ch_info *channel)
{
	struct ad7124_dev	*desc = (struct ad7124_dev *)device;
	uint16_t		reg_val;

	sscanf(buf, "%hd", &reg_val);

	desc->regs[AD7124_CH0_MAP_REG + channel->ch_num].value &=
		~AD7124_CH_MAP_REG_AINM(0x1F);
	desc->regs[AD7124_CH0_MAP_REG + channel->ch_num].value |=
		AD7124_CH_MAP_REG_AINM(reg_val);

	ad7124_write_register(desc,
			desc->regs[AD7124_CH0_MAP_REG + channel->ch_num]);

	return len;
}

/**
 * @brief Transfer data from device into RAM.
 * @param iio_inst - Physical instance of a iio_axi_adc device.
 * @param bytes_count - Number of bytes to transfer.
 * @param ch_mask - Opened channels mask.
 * @return bytes_count or negative value in case of error.
 */
static ssize_t iio_ad7124_transfer_dev_to_mem(void *iio_inst,
		size_t bytes_count,
		uint32_t ch_mask)
{
//	struct iio_ad7124_desc	*iio_desc = (struct iio_ad7124_desc *)iio_inst;
//	struct ad7124_dev	*desc = (struct ad7124_dev *)iio_desc->ad7124_driver_handler;
//	ssize_t ret, sample_no;
//	uint8_t ch_bulk = 3 * hweight8(ch_mask);
//	int32_t data_temp;
//	uint32_t i, j = 0;
//
//	if ((bytes_count % ch_bulk) != 0)
//		return -EINVAL;
//
//	sample_no = bytes_count / ch_bulk;
//
//	while (sample_no--) {
//		for (i = 0; i < hweight8(ch_mask); i++) {
//			ret = ad7124_wait_for_conv_ready(desc, 100000);
//			if (ret != SUCCESS)
//				return -ETIMEDOUT;
//			ret = ad7124_read_data(desc, &data_temp);
//			if (ret != SUCCESS)
//				return -EIO;
//			chan_idx = desc->regs[AD7124_Status].value &
//				   AD7124_STATUS_REG_CH_ACTIVE(0xF);
//			if (chan_idx != 0) {
//				i--;
//				continue;
//			}
//			desc->ddr_base_addr[j + i * 3] = (uint8_t *)data_temp + 1;
//			desc->ddr_base_addr[j + i * 3 + 1] = (uint8_t *)data_temp + 2;
//			desc->ddr_base_addr[j + i * 3 + 2] = (uint8_t *)data_temp + 3;
//		}
//		j += i * 3;
//	}

	return bytes_count;
}

/**
 * @brief Read chunk of data from RAM to pbuf.
 * Call "iio_axi_adc_transfer_dev_to_mem" first.
 * This function is probably called multiple times by libtinyiiod after a
 * "iio_axi_adc_transfer_dev_to_mem" call, since we can only read "bytes_count"
 * bytes at a time.
 * @param iio_inst - Physical instance of a iio_axi_adc device.
 * @param pbuf - Buffer where value is stored.
 * @param offset - Offset to the remaining data after reading n chunks.
 * @param bytes_count - Number of bytes to read.
 * @param ch_mask - Opened channels mask.
 * @return bytes_count or negative value in case of error.
 */
static ssize_t iio_ad7124_read_dev(void *iio_inst, char *pbuf, size_t offset,
				    size_t bytes_count, uint32_t ch_mask)
{
//	struct iio_ad7124_desc	*iio_desc = (struct iio_ad7124_desc *)iio_inst;
//	struct ad7124_dev	*desc = (struct ad7124_dev *)iio_desc->ad7124_driver_handler;
//	uint32_t i, j = 0, current_ch = 0;
//	uint32_t *pbuf32;
//	size_t samples;
//	uint8_t ch_bulk = 3 * hweight8(ch_mask);
//
//	if (!iio_inst)
//		return FAILURE;
//
//	if (!pbuf)
//		return FAILURE;
//
//	if ((bytes_count % ch_bulk) != 0)
//		return -EINVAL;
//
//	pbuf32 = (uint32_t*)pbuf;
//	samples = bytes_count / ch_bulk;
//
//	//offset = (offset * iio_adc->adc->num_channels) / hweight8(ch_mask);
//
//	for (i = 0; i < samples * iio_desc->iio_ad7124_device->num_ch; i++) {
//		if (ch_mask & BIT(current_ch)) {
//			pbuf32[j] = *(uint16_t*)(iio_adc->adc_ddr_base + offset + i * 2);
//			j++;
//		}
//
//		if (current_ch + 1 < iio_adc->adc->num_channels)
//			current_ch++;
//		else
//			current_ch = 0;
//	}

	return bytes_count;
}

int32_t iio_ad7124_init(struct iio_ad7124_desc **desc,
		struct iio_ad7124_init_param *init_param)
{
	struct iio_ad7124_desc *ldesc;

//	if (!init_param->ad7124_driver_handler)
//		return -EINVAL;

	ldesc = calloc(1, sizeof(*ldesc));
	if (!ldesc)
		return FAILURE;

	ldesc->iio_ad7124_device = calloc(1,
			sizeof(*ldesc->iio_ad7124_device));
	if (!ldesc->iio_ad7124_device) {
		free(ldesc);
		return -ENOMEM;
	}

	ldesc->iio_ad7124_device->num_ch = 2;
	ldesc->iio_ad7124_device->channels = ad7124_channels;
	ldesc->iio_ad7124_device->attributes = ad7124_attributes;
	ldesc->iio_ad7124_device->debug_attributes = NULL;
	ldesc->iio_ad7124_device->buffer_attributes = NULL;
	ldesc->iio_ad7124_device->transfer_dev_to_mem = iio_ad7124_transfer_dev_to_mem;
	ldesc->iio_ad7124_device->read_data = NULL;
	ldesc->iio_ad7124_device->transfer_mem_to_dev = NULL;
	ldesc->iio_ad7124_device->write_data = NULL;

	ldesc->ad7124_driver_handler =
		init_param->ad7124_driver_handler;
	ldesc->ddr_base_addr = init_param->ddr_base_addr;

	*desc = ldesc;

	return SUCCESS;
}

int32_t iio_ad7124_remove(struct iio_ad7124_desc *desc)
{
	if (!desc)
		return FAILURE;

	free(desc->iio_ad7124_device);
	free(desc);

	return SUCCESS;
}

