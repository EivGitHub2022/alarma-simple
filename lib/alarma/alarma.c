#include <alarma.h>

void Alarma_init(Alarma *self,
    EstadoAlarma *estadoInicial,
    const Comando *iDeteccion,      const Comando *iFinDeteccion,
    const Comando *iArmada,         const Comando *iDesarmada,
    const Comando *iAlerta,         const Comando *iFinAlerta,
    const Comando *iTemporizado,    const Comando *iFinTemporizado,
    const Comando *tArmado,         const Comando *ntArmado,
    const Comando *tDisparo,        const Comando *ntDisparo,
    const Comando *tAlerta,         const Comando *ntAlerta)
{
    self->super.estado      = (EstadoMEF*) estadoInicial;
    self->super.inicio      = true;
    self->deteccion         = false;
    self->iDeteccion        = iDeteccion;
    self->iFinDeteccion     = iFinDeteccion;
    self->iArmada           = iArmada;
    self->iDesarmada        = iDesarmada;
    self->iAlerta           = iAlerta;
    self->iFinAlerta        = iFinAlerta;
    self->iTemporizado      = iTemporizado;
    self->iFinTemporizado   = iFinTemporizado;
    self->tArmado           = tArmado;
    self->ntArmado          = ntArmado;
    self->tDisparo          = tDisparo;
    self->ntDisparo         = ntDisparo;
    self->tAlerta           = tAlerta;
    self->ntAlerta          = ntAlerta;
}

