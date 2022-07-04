#ifndef PIN_GPIO_H
#define PIN_GPIO_H
#include <stm32f1xx.h>
#include <stdbool.h>

typedef struct Pin{
    GPIO_TypeDef *puerto;
    int numeroDePin;
} Pin;

void Pin_init(Pin *self, GPIO_TypeDef *puerto,int numeroDePin);

bool Pin_lee(Pin *self);

void Pin_escribe(Pin *self, bool valor);

#endif
