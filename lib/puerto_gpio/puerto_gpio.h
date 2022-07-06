#ifndef PUERTO_GPIO_H
#define PUERTO_GPIO_H
#include <pin_gpio.h>
typedef struct PuertoGpio PuertoGpio;

extern const PuertoGpio *const puertoA;
extern const PuertoGpio *const puertoB;
extern const PuertoGpio *const puertoC;
extern const PuertoGpio *const puertoD;
extern const PuertoGpio *const puertoE;

void PuertoGpio_configPinEFlotante  (const PuertoGpio *self,int nrPin, Pin *pin);
void PuertoGpio_configPinSLenta     (const PuertoGpio *self,int nrPin, Pin *pin);
#endif
