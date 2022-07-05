#include <stddef.h>
#include <alarma.h>

static bool DESARMADA(Alarma *ctx, int evento);
static bool TEMPORIZA_ARMADO(Alarma *ctx, int evento);
static bool ESPERA_DISPARO(Alarma *ctx, int evento);
static bool TEMPORIZA_DISPARO(Alarma *ctx, int evento);
static bool DISPARADA(Alarma *ctx, int evento);

EstadoAlarma * const ESTADO_ALARMA_DESARMADA = &DESARMADA;


static bool DESARMADA(Alarma *ctx,int evento)
{
    bool consumeEvento = true;
    switch(evento){
    break; case EID_ENTRA_ESTADO:
        Alarma_indicaDesarmada(ctx);
    break; case EID_ALARMA_DETECCION:
        Alarma_indicaDeteccion(ctx);
    break; case EID_ALARMA_FIN_DETECCION:
        Alarma_indicaFinDeteccion(ctx);
    break; case EID_ALARMA_ARMAR:
        Alarma_cambiaEstado(ctx, TEMPORIZA_ARMADO);
    break; default:
        consumeEvento = false;
    }
    return consumeEvento;
}


static bool ARMADA(Alarma *ctx, int evento)
{
    bool consumeEvento = true;
    switch (evento)
    {
    break; case EID_ENTRA_ESTADO:
        Alarma_indicaArmada(ctx);
    break; case EID_ALARMA_DESARMAR:
        Alarma_cambiaEstado(ctx,DESARMADA);
    break; case EID_ALARMA_DETECCION:
        Alarma_indicaDeteccion(ctx);
    break; case EID_ALARMA_FIN_DETECCION:
        Alarma_indicaFinDeteccion(ctx);
    break;default:
        consumeEvento = false;
    }
    return consumeEvento;
}

static bool TEMPORIZA_ARMADO(Alarma *ctx, int evento)
{
    bool consumeEvento = true;
    switch(evento){
        /* Atender eventos */
    break; case EID_ENTRA_ESTADO:
        Alarma_indicaTemporizado(ctx);
        Alarma_temporizaArmado(ctx);
        consumeEvento = false;
    break; case EID_SALE_ESTADO:
        Alarma_indicaFinTemporizado(ctx);
        Alarma_noTemporizaArmado(ctx);
    break; case EID_ALARMA_TIEMPO_TERMINADO:
        if(ctx->deteccion)
            Alarma_cambiaEstado(ctx,TEMPORIZA_DISPARO);
        else
            Alarma_cambiaEstado(ctx,ESPERA_DISPARO);
    break; default:
        consumeEvento=false;
    }
    return consumeEvento || ARMADA(ctx,evento);
}

static bool ESPERA_DISPARO(Alarma *ctx,int evento)
{
    bool consumeEvento = true;
    switch (evento){
        /* Atender eventos */
    break; case EID_ALARMA_DETECCION:
        Alarma_cambiaEstado(ctx,TEMPORIZA_DISPARO);
        consumeEvento = false; // Continúa procesando en ARMADA
    break; default:
        consumeEvento = false;
    }
    return consumeEvento || ARMADA(ctx,evento);
} 

static bool TEMPORIZA_DISPARO(Alarma *ctx,int evento)
{
    bool consumeEvento = true;
    switch(evento){
        /* Atender eventos */
    break; case EID_ENTRA_ESTADO:
        Alarma_indicaTemporizado(ctx);
        Alarma_temporizaDisparo(ctx);
        consumeEvento = false;
    break; case EID_SALE_ESTADO:
        Alarma_indicaFinTemporizado(ctx);
        Alarma_noTemporizaDisparo(ctx);
    break; case EID_ALARMA_TIEMPO_TERMINADO:
        Alarma_cambiaEstado(ctx,DISPARADA);
    break; default:
        consumeEvento = ARMADA(ctx,evento);
    }
    return consumeEvento;
}

static bool DISPARADA(Alarma *ctx,int evento)
{
    bool consumeEvento = true;
    switch(evento){
        /* Atender eventos */
    break; case EID_ENTRA_ESTADO:
        Alarma_indicaAlerta(ctx);
        if (!ctx->deteccion)
            Alarma_temporizaAlerta(ctx);
        consumeEvento = false;
    break; case EID_SALE_ESTADO:
        Alarma_indicaFinAlerta(ctx);
        Alarma_noTemporizaAlerta(ctx);
    break; case EID_ALARMA_DETECCION:
        Alarma_noTemporizaAlerta(ctx);
        consumeEvento=false;
    break; case EID_ALARMA_FIN_DETECCION:
        Alarma_temporizaAlerta(ctx);
        consumeEvento = false;
    break; case EID_ALARMA_TIEMPO_TERMINADO:
        if (ctx->deteccion)
            consumeEvento = false;
        else
            Alarma_cambiaEstado(ctx,ESPERA_DISPARO);
    break; default:
        consumeEvento = false;
    }
    return consumeEvento || ARMADA(ctx,evento);
}