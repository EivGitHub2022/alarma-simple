#ifndef BANDERAS_H
#define BANDERAS_H
#include <stdbool.h>

#define LISTA_BANDERAS(PLANTILLA)\
    PLANTILLA(I_DETECCION     , 0)\
    PLANTILLA(I_ARMADO        , 1)\
    PLANTILLA(I_ALERTA        , 2)\
    PLANTILLA(I_TEMPORIZADO   , 3)\
    PLANTILLA(T_ARMADO        , 4)\
    PLANTILLA(T_DISPARO       , 5)\
    PLANTILLA(T_ALERTA        , 6)

#define PLANTILLA_ENUM(nombre_,posicion_)  nombre_ = (1<<(posicion_)),
/**
 * @brief Banderas que representan indicadores (I_) y timers (T_)
 * 
 */
typedef enum Banderas{
    LISTA_BANDERAS(PLANTILLA_ENUM)
}Banderas;

#undef PLANTILLA_ENUM

#define PLANTILLA_CONTEO(nombre_,posicion_) + 1UL
#define NUM_BANDERAS (0UL LISTA_BANDERAS(PLANTILLA_CONTEO))


const char *Banderas_mensajeDiferencias(const Banderas esperado,const Banderas obtenido);
bool Banderas_sonDistintas(Banderas esperado, Banderas obtenido);
bool Banderas_sonIguales(Banderas esperado, Banderas obtenido);
#endif