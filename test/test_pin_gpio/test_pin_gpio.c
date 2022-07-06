#include <unity.h>
#include <timer_systick.h>
#include <pin_gpio.h>
#include <puerto_gpio_inst.h>
#include <stm32f1xx.h>
#include <stdbool.h>

typedef struct TestPinGpio TestPinGpio;

#define MI_BIT_HAB_RELOJ 7

struct TestPinGpio{
    volatile uint32_t habReloj;
    GPIO_TypeDef gpio;
    PuertoGpio puerto;
    Pin pinE;
    Pin pinS;
};

static TestPinGpio self;

#define NUM_PINES_PUERTO 16

#define MI_PIN_E 13
#define MI_PIN_S 7

void setUp(void)
{
    PuertoGpio_init(&self.puerto,&self.gpio,&self.habReloj,MI_BIT_HAB_RELOJ);
    PuertoGpio_configPinEFlotante(&self.puerto,MI_PIN_E,&self.pinE);
    PuertoGpio_configPinSLenta(&self.puerto,MI_PIN_S,&self.pinS);
}
void tearDown(void)
{
    self = (TestPinGpio){};
}

static void puerto_debe_habilitar_reloj_periferico(void);
static void puerto_debe_configurar_pin_entrada_flotante(void);
static void puerto_debe_configurar_pin_salida_lenta_push_pull(void);

static void pin_debe_leer_entrada_0(void);
static void pin_debe_leer_entrada_1(void);
static void pin_debe_establecer_salida_0(void);
static void pin_debe_establecer_salida_1(void);

int main(void)
{
    TimerSysTick_init();
    TimerSysTick_esperaMilisegundos(500);
    UNITY_BEGIN();
    RUN_TEST(puerto_debe_habilitar_reloj_periferico);
    RUN_TEST(puerto_debe_configurar_pin_entrada_flotante);
    RUN_TEST(puerto_debe_configurar_pin_salida_lenta_push_pull);
    RUN_TEST(pin_debe_leer_entrada_0);
    RUN_TEST(pin_debe_leer_entrada_1);
    RUN_TEST(pin_debe_establecer_salida_0);
    RUN_TEST(pin_debe_establecer_salida_1);
    UNITY_END();   
}


static void puerto_debe_habilitar_reloj_periferico(void)
{
    TEST_ASSERT_EQUAL_HEX((1UL<<MI_BIT_HAB_RELOJ),self.habReloj);
}

typedef enum ConfigPin{
    E_ANALOGICA     = 0,
    E_CON_RPULL     = 0b1000,
    E_FLOTANTE      = 0b0100,
    S_GPIO          = 0,
    S_AFIO          = 0b1000,
    S_MEDIA         = 0b0001,
    S_LENTA         = 0b0010,
    S_RAPIDA        = 0b0011,
    S_PUSH_PULL     = 0,
    S_OPEN_DRAIN    = 0b0100,
    MASCARA_CFG_PIN = 0b1111
}ConfigPin;

inline static ConfigPin TestPinGpio_obtConfigPin(int pin)
{
    ConfigPin config;
    if (pin < 8){
        config = ((self.gpio.CRL >> (pin*4))        & MASCARA_CFG_PIN);
    } else {
        config = ((self.gpio.CRH >> ((pin - 8)*4))  & MASCARA_CFG_PIN);
    }
    return config;
}

static void puerto_debe_configurar_pin_entrada_flotante(void)
{
    const ConfigPin cfg = E_FLOTANTE;
    
    TEST_ASSERT_EQUAL_HEX8(cfg,TestPinGpio_obtConfigPin(MI_PIN_E));
    TEST_ASSERT_NOT_EQUAL_HEX8(cfg,TestPinGpio_obtConfigPin(MI_PIN_S));

    PuertoGpio_configPinEFlotante(&self.puerto,MI_PIN_S,&self.pinS);
    TEST_ASSERT_EQUAL_HEX8(cfg,TestPinGpio_obtConfigPin(MI_PIN_S));
}
static void puerto_debe_configurar_pin_salida_lenta_push_pull(void)
{
    const ConfigPin cfg = S_GPIO | S_LENTA | S_PUSH_PULL;

    TEST_ASSERT_NOT_EQUAL_HEX8(cfg,TestPinGpio_obtConfigPin(MI_PIN_E));
    TEST_ASSERT_EQUAL_HEX8(cfg,TestPinGpio_obtConfigPin(MI_PIN_S));
    
    PuertoGpio_configPinSLenta(&self.puerto,MI_PIN_E,&self.pinE);
    TEST_ASSERT_EQUAL_HEX8(cfg,TestPinGpio_obtConfigPin(MI_PIN_E));

}

inline static void TestPinGpio_estRegistroEntrada(uint32_t valor)
{
    self.gpio.IDR = valor & ((1UL << NUM_PINES_PUERTO)-1UL);
}

static void pin_debe_leer_entrada_0(void)
{
    TestPinGpio_estRegistroEntrada(~(1UL<<MI_PIN_E));
    TEST_ASSERT_FALSE(Pin_lee(&self.pinE));
    TestPinGpio_estRegistroEntrada(~(1UL<<MI_PIN_S));
    TEST_ASSERT_FALSE(Pin_lee(&self.pinS));    
}

static void pin_debe_leer_entrada_1(void)
{
    TestPinGpio_estRegistroEntrada(1UL<<MI_PIN_E);
    TEST_ASSERT_TRUE(Pin_lee(&self.pinE));
    TestPinGpio_estRegistroEntrada(1UL<<MI_PIN_S);
    TEST_ASSERT_TRUE(Pin_lee(&self.pinS));
}

inline static uint32_t TestPinGpio_obtMascaraSet(void)
{
    return self.gpio.BSRR & ((1UL << NUM_PINES_PUERTO) - 1UL);
}
inline static uint32_t TestPinGpio_obtMascaraReset(void)
{
    return self.gpio.BRR | (self.gpio.BSRR >> 16);
}

static void pin_debe_establecer_salida_0(void)
{
    Pin_escribe(&self.pinS,0);
    TEST_ASSERT_EQUAL_HEX32((1UL<<MI_PIN_S) ,TestPinGpio_obtMascaraReset());
    TEST_ASSERT_EQUAL_HEX32(0               ,TestPinGpio_obtMascaraSet());
}

static void pin_debe_establecer_salida_1(void)
{
    Pin_escribe(&self.pinS,1);
    TEST_ASSERT_EQUAL_HEX32(0               ,TestPinGpio_obtMascaraReset());
    TEST_ASSERT_EQUAL_HEX32((1UL<<MI_PIN_S) ,TestPinGpio_obtMascaraSet());
}
