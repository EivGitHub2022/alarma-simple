#include "mef.h"
#include <assert.h>

bool MEF_procesaEvento(MEF *self, int evento)
{    
    if (self->inicio)
    {
        self->inicio = false;
        self->estado(self,EID_ENTRA_ESTADO);
    }
    EstadoMEF *const inicial = self->estado;
    
    const bool retval = self->estado(self,evento);
    
    EstadoMEF *const final = self->estado;

    if (inicial != final){
        inicial(self,EID_SALE_ESTADO);
        final(self,EID_ENTRA_ESTADO);
        assert(final == self->estado);
    }
    return retval;
}