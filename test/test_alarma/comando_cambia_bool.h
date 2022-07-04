#ifndef COMANDO_CAMBIA_BOOL_H
#define COMANDO_CAMBIA_BOOL_H
#include <comando.h>
// CambiaBool : Comando
typedef struct CambiaBool
{
    Comando super;
    volatile bool *objetivo;
    bool valor;
}CambiaBool;

void CambiaBool_ejecutar(const CambiaBool *self)
{
    *self->objetivo = self->valor;
}

#define CAMBIA_BOOL(objetivo_,valor_) ((const Comando*)(&(const CambiaBool){\
    .super  = {.ejecutar = (Comando_Accion*) CambiaBool_ejecutar},\
    .objetivo = (objetivo_),\
    .valor  = (valor_),\
}))

// fin CambiaBool

#endif