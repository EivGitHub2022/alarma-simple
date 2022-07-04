#ifndef ALARMA_H
#define ALARMA_H
#include <mef.h>
#include <comando.h>

/**
 * @brief Alarma : MEF
 * 
 */
typedef struct Alarma Alarma;
/**
 * @brief EventoAlarma : Evento
 * 
 */
typedef struct EventoAlarma EventoAlarma;
/**
 * @brief EstadoAlarma : Estado
 * 
 */
typedef bool EstadoAlarma(Alarma *contexto, int evento);

struct Alarma
{
    MEF super;
    bool deteccion;
    const Comando * iDeteccion;
    const Comando * iFinDeteccion;
    const Comando * iArmada;
    const Comando * iDesarmada;
    const Comando * iAlerta;
    const Comando * iFinAlerta;
    const Comando * iTemporizado;
    const Comando * iFinTemporizado;
    const Comando * tArmado;
    const Comando * ntArmado;
    const Comando * tDisparo;
    const Comando * ntDisparo;
    const Comando * tAlerta;
    const Comando * ntAlerta;
};

void Alarma_init(Alarma *self,
    EstadoAlarma *estadoInicial,
    const Comando *iDeteccion,      const Comando *iFinDeteccion,
    const Comando *iArmada,         const Comando *iDesarmada,
    const Comando *iAlerta,         const Comando *iFinAlerta,
    const Comando *iTemporizado,    const Comando *iFinTemporizado,
    const Comando *tArmado,         const Comando *ntArmado,
    const Comando *tDisparo,        const Comando *ntDisparo,
    const Comando *tAlerta,         const Comando *ntAlerta);


inline static void Alarma_cambiaEstado(Alarma *self, EstadoAlarma *estado)
{
    MEF_cambiaEstado((MEF*) self,(const EstadoMEF*)estado);
}
inline static bool Alarma_procesaEvento(Alarma *self,int evento)
{
    return MEF_procesaEvento((MEF*)self,evento);
}

// Comandos de salida
inline static void Alarma_indicaDeteccion(Alarma *self)
{
    self->deteccion = true;
    Comando_ejecutar(self->iDeteccion);
}
inline static void Alarma_indicaFinDeteccion(Alarma *self)
{
    self->deteccion = false;
    Comando_ejecutar(self->iFinDeteccion);
}
inline static void Alarma_indicaArmada(const Alarma *self)
{
    Comando_ejecutar(self->iArmada);
}
inline static void Alarma_indicaDesarmada(const Alarma *self)
{
    Comando_ejecutar(self->iDesarmada);
}
inline static void Alarma_indicaAlerta(const Alarma *self)
{
    Comando_ejecutar(self->iAlerta);
}
inline static void Alarma_indicaFinAlerta(const Alarma *self)
{
    Comando_ejecutar(self->iFinAlerta);
}
inline static void Alarma_indicaTemporizado(const Alarma *self)
{
    Comando_ejecutar(self->iTemporizado);
}
inline static void Alarma_indicaFinTemporizado(const Alarma *self)
{
    Comando_ejecutar(self->iFinTemporizado);
}
inline static void Alarma_temporizaArmado(const Alarma *self)
{
    Comando_ejecutar(self->tArmado);
}
inline static void Alarma_noTemporizaArmado(const Alarma *self)
{
    Comando_ejecutar(self->ntArmado);
}
inline static void Alarma_temporizaDisparo(const Alarma *self)
{
    Comando_ejecutar(self->tDisparo);
}
inline static void Alarma_noTemporizaDisparo(const Alarma *self)
{
    Comando_ejecutar(self->ntDisparo);
}
inline static void Alarma_temporizaAlerta(const Alarma *self)
{
    Comando_ejecutar(self->tAlerta);
}
inline static void Alarma_noTemporizaAlerta(const Alarma *self)
{
    Comando_ejecutar(self->ntAlerta);
}

// Estados
extern EstadoAlarma *const ESTADO_ALARMA_DESARMADA;

enum IdEventoAlarma {
    EID_ALARMA_DETECCION = NUM_EID_BASE,
    EID_ALARMA_FIN_DETECCION,
    EID_ALARMA_ARMAR,
    EID_ALARMA_DESARMAR,
    EID_ALARMA_TIEMPO_TERMINADO,
    NUM_EID_ALARMA
};

#endif