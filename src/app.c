/**
 * @file loop.c
 * @author Fernando A. Miranda Bonomi (fmirandabonomi@herrera.unt.edu.ar)
 * @brief Implementación del lazo principal sencilla para probar el 
 *      funcionamiento básico del entorno y el hardware. Simplemente hace 
 *      parpadear el LED conectado al pin 13 del puero C del bluepill.
 */
#include <app.h>
#include <stm32f1xx.h>
#include <timer_systick.h>
#include <puerto_gpio.h>






inline static void App__actualizaSalidas(App *self)
{
    if (self->led.estado == ENCENDIDO)
        Pin_escribe(self->led.pin,0);
    else
        Pin_escribe(self->led.pin,1);
}



inline static void App__actualizaEstado(App *self)
{
    switch (self->led.estado){
    break; case ENCENDIDO:
        self->led.estado = APAGADO;
    break; case APAGADO:
        self->led.estado = ENCENDIDO;
    break; default:
    break;
    }
}

void App_init(App *self, Pin *pinLed, EstadoLed estadoInicial, uint32_t semiperiodo)
{
    TimerSysTick_init();
    self->led.pin               = pinLed;
    self->led.estado            = estadoInicial;
    self->tiempo_ms.semiperiodo = semiperiodo;
    self->tiempo_ms.referencia  = TimerSysTick_getMilisegundos();
    App__actualizaSalidas(self);
}




void App_turno(App *self)
{
    const uint32_t transcurrido = TimerSysTick_getMilisegundos() - self->tiempo_ms.referencia;
    if(transcurrido >= self->tiempo_ms.semiperiodo){
        self->tiempo_ms.referencia = TimerSysTick_getMilisegundos();
        App__actualizaEstado(self);
        App__actualizaSalidas(self);
    }
}
