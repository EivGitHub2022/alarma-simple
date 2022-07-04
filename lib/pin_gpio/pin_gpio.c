#include <pin_gpio.h>

void Pin_init(Pin *self, GPIO_TypeDef *puerto,int numeroDePin)
{
    self->puerto = puerto;
    self->numeroDePin = numeroDePin;
}

bool Pin_lee(Pin *self)
{
    return (bool)(self->puerto->IDR & (1 << (self->numeroDePin)));
}

void Pin_escribe(Pin *self, bool valor)
{
    if (!valor)
        self->puerto->BRR = (1<<self->numeroDePin);
    else
        self->puerto->BSRR = (1<<self->numeroDePin);
}