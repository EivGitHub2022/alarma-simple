#ifndef PUERTO_GPIO_INST_H
#define PUERTO_GPIO_INST_H
#include <puerto_gpio.h>
#include <stm32f1xx.h>

struct PuertoGpio
{
    struct HabReloj{
        volatile uint32_t * registro;
        uint32_t mascara;
    }habilitacionReloj;
    GPIO_TypeDef *base;
};

void PuertoGpio_init(PuertoGpio *self,
                    GPIO_TypeDef *base,
                    volatile uint32_t* registroHabilitacionReloj,
                    int nrBitHabilitacionReloj);

#endif