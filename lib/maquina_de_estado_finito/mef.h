#ifndef MEF_H
#define MEF_H
#include <stdbool.h>
/**
 * @brief Mantiene el contexto y estado de una máquina de estado finito
 * 
 * void MEF_cambiaEstado(MEF *self,const Estado *estado);
 * 
 * bool MEF_procesaEvento(MEF *self,const Evento *evento);
 * 
 *  Las subclases deben incorporar métodos que soporten todas las acciones
 *  de entrada/salida necesarias para la implementación de los comportamientos
 *  definidos en los estados.
 */
typedef struct MEF MEF;

typedef bool EstadoMEF(MEF *contexto, int evento);

// MEF
struct MEF{
    EstadoMEF *estado;
    bool inicio;
};
inline static void MEF_cambiaEstado(MEF *self, EstadoMEF *estado)
{
    self->estado = estado;
}

/**
 * @brief Procesa un evento
 * 
 * @return true  Evento capturado
 * @return false
 */
bool MEF_procesaEvento(MEF *self, int evento);

/**
 * @brief Este evento no debe generar transicion de estado
 * 
 */
enum IdEventoBase {EID_ENTRA_ESTADO,EID_SALE_ESTADO,NUM_EID_BASE};

#endif
