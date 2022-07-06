/**
 * @file main.c
 * @author Fernando A. Miranda Bonomi (fmirandabonomi@herrera.unt.edu.ar)
 * @brief Esqueleto de aplicación con una función de configuración y un lazo infinito.
 */
#include <app.h>
#include <puerto_gpio.h>

int main(void)
{
    App miApp;
    Pin pinLed;
    PuertoGpio_configPinSLenta(puertoC,13,&pinLed);
    App_init(&miApp,&pinLed,APAGADO,500);
    for(;;)App_turno(&miApp);
    return 0;
}
