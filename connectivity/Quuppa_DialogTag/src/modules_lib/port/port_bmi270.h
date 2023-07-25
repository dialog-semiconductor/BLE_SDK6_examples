#ifndef _PORT_BMI270_H
#define _PORT_BMI270_H
#include "bmi2.h"
#include "bmi270.h"
int8_t port_bmi270_init(void);

void port_bmi270_itf_begin(void);

void port_bmi270_itf_end(void);

struct bmi2_dev* port_bmi270_dev_ptr(void);
#endif // _PORT_BMI270_H
