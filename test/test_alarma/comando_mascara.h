#ifndef COMANDO_CAMBIA_BOOL_H
#define COMANDO_CAMBIA_BOOL_H
#include <comando.h>
// ComandoMascara : Comando
typedef struct ComandoMascara
{
    Comando super;
    volatile int *objetivo;
    int mascara;
    bool valor;
}ComandoMascara;

void ComandoMascara_ejecutar(const ComandoMascara *self)
{
    if (self->valor)
        *self->objetivo |= self->mascara;
    else
        *self->objetivo &= ~self->mascara;
}

#define COMANDO_MASCARA(objetivo_,mascara_,valor_) ((const Comando*)(&(const ComandoMascara){\
    .super      = {.ejecutar = (Comando_Accion*) ComandoMascara_ejecutar},\
    .objetivo   = (objetivo_),\
    .mascara    = (mascara_),\
    .valor      = (valor_)\
}))

// fin ComandoMascara

#endif