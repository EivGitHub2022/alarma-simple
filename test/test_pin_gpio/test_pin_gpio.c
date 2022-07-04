#include <unity.h>
#include <timer_systick.h>
#include <pin_gpio.h>
#include <stm32f1xx.h>
#include <stdbool.h>

static GPIO_TypeDef miPuerto;
#define MI_PIN 13

static Pin miPin;

void setUp(void)
{
    miPuerto = (GPIO_TypeDef){.CRH=0x44444444,.CRL=0x44444444};
    Pin_init(&miPin,&miPuerto,MI_PIN);
}
void tearDown(void){}

static void pin_debePermitirLeerEstado_0(void)
{
    const bool estado = Pin_lee(&miPin);
    TEST_ASSERT_FALSE(estado);    
}

static void pin_debePermitirLeerEstado_1(void)
{
    miPuerto.IDR = 1<<MI_PIN;
    const bool estado = Pin_lee(&miPin);
    TEST_ASSERT_TRUE(estado);
}

static void pin_debePermitirPonerSalidaEnCero(void)
{
    Pin_escribe(&miPin,0);

    const uint32_t reset = miPuerto.BRR | (miPuerto.BSRR >> 16);
    const uint32_t set = miPuerto.BSRR & ((1UL << 16) - 1UL);

    enum{RESET_ESPERADO = (1UL << MI_PIN), SET_ESPERADO = 0};
    
    TEST_ASSERT_EQUAL_HEX32(RESET_ESPERADO,reset);
    TEST_ASSERT_EQUAL_HEX32(SET_ESPERADO,set);
}

static void pin_debePermitirPonerSalidaEnUno(void)
{
    Pin_escribe(&miPin,1);

    const uint32_t reset = miPuerto.BRR | (miPuerto.BSRR >> 16);
    const uint32_t set = miPuerto.BSRR & ((1UL << 16) - 1UL);
    
    enum{RESET_ESPERADO = 0, SET_ESPERADO = (1UL << MI_PIN)};
    
    TEST_ASSERT_EQUAL_HEX32(RESET_ESPERADO,reset);
    TEST_ASSERT_EQUAL_HEX32(SET_ESPERADO,set);
}

int main(void)
{
    TimerSysTick_init();
    TimerSysTick_esperaMilisegundos(500);
    UNITY_BEGIN();
    RUN_TEST(pin_debePermitirLeerEstado_0);
    RUN_TEST(pin_debePermitirLeerEstado_1);
    RUN_TEST(pin_debePermitirPonerSalidaEnCero);
    RUN_TEST(pin_debePermitirPonerSalidaEnUno);
    UNITY_END();   
}

