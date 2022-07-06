#ifndef APP_H
#define APP_H
#include <pin_gpio.h>

typedef struct App App;

typedef enum EstadoLed{ENCENDIDO,APAGADO} EstadoLed;
struct App{
    struct Led{
        Pin *pin;
        EstadoLed estado;
    }led;
    struct{
        uint32_t referencia;
        uint32_t semiperiodo;
    }tiempo_ms;
};

void App_init(App *self,Pin *pinLed,EstadoLed estadoInicial,uint32_t semiperiodo);
void App_turno(App *self);

#endif
