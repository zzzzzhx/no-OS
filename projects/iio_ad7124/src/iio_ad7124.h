#ifndef IIO_AD7124_H
#define IIO_AD7124_H

#include "iio.h"
#include "ad7124.h"

struct iio_ad7124_desc {
	struct iio_device *iio_ad7124_device;
	struct ad7124_dev *ad7124_driver_handler;
	uint8_t *ddr_base_addr;
};

struct iio_ad7124_init_param {
	struct ad7124_dev *ad7124_driver_handler;
	uint8_t *ddr_base_addr;
};

int32_t iio_ad7124_init(struct iio_ad7124_desc **desc,
		struct iio_ad7124_init_param *init_param);
int32_t iio_ad7124_remove(struct iio_ad7124_desc *desc);

#endif /** IIO_AD7124_H */
