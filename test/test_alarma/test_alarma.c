#include <unity.h>
#include <alarma.h>
#include <timer_systick.h>
#include "comando_mascara.h"
#include "banderas.h"

static struct TestAlarma {
    Alarma alarma;
    int banderas;
}self;

#define TEST_BANDERAS(banderas_) TEST_ASSERT_MESSAGE(Banderas_sonIguales(banderas_, self.banderas),\
                                    Banderas_mensajeDiferencias(banderas_,self.banderas))

static const Comando *const cmdIDeteccion       = COMANDO_MASCARA(&self.banderas, I_DETECCION    ,true);
static const Comando *const cmdIFinDeteccion    = COMANDO_MASCARA(&self.banderas, I_DETECCION    ,false);
static const Comando *const cmdIArmada          = COMANDO_MASCARA(&self.banderas, I_ARMADO       ,true);
static const Comando *const cmdIDesarmada       = COMANDO_MASCARA(&self.banderas, I_ARMADO       ,false);
static const Comando *const cmdIAlerta          = COMANDO_MASCARA(&self.banderas, I_ALERTA       ,true);
static const Comando *const cmdIFinAlerta       = COMANDO_MASCARA(&self.banderas, I_ALERTA       ,false);
static const Comando *const cmdITemporizado     = COMANDO_MASCARA(&self.banderas, I_TEMPORIZADO  ,true);
static const Comando *const cmdIFinTemporizado  = COMANDO_MASCARA(&self.banderas, I_TEMPORIZADO  ,false);
static const Comando *const cmdTArmado          = COMANDO_MASCARA(&self.banderas, T_ARMADO       ,true);
static const Comando *const cmdNTArmado         = COMANDO_MASCARA(&self.banderas, T_ARMADO       ,false);
static const Comando *const cmdTDisparo         = COMANDO_MASCARA(&self.banderas, T_DISPARO      ,true);
static const Comando *const cmdNTDisparo        = COMANDO_MASCARA(&self.banderas, T_DISPARO      ,false);
static const Comando *const cmdTAlerta          = COMANDO_MASCARA(&self.banderas, T_ALERTA       ,true);
static const Comando *const cmdNTAlerta         = COMANDO_MASCARA(&self.banderas, T_ALERTA       ,false);

void TestAlarma_init(void)
{
    Alarma_init(&self.alarma,
        ESTADO_ALARMA_DESARMADA,
        cmdIDeteccion,      cmdIFinDeteccion,
        cmdIArmada,         cmdIDesarmada,
        cmdIAlerta,         cmdIFinAlerta,
        cmdITemporizado,    cmdIFinTemporizado,
        cmdTArmado,         cmdNTArmado,
        cmdTDisparo,        cmdNTDisparo,
        cmdTAlerta,         cmdNTAlerta);
}
void TestAlarma_deinit(void)
{
    self = (const struct TestAlarma){};
}


void setUp(void)
{
    TestAlarma_init();
}

void tearDown(void)
{
    TestAlarma_deinit();
}

/**
 * 
 * @brief Si la alarma se encuentra desarmada y los sensores detectan 
 *        actividad en la zona monitoreada entonces el indicador de actividad 
 *        debe encenderse, pero no debe establecerse la condición de alarma.
 */
static void alarma_desarmada_debe_indicar_actividad(void)
{
    TEST_BANDERAS(0);
    Alarma_procesaEvento(&self.alarma,EID_ALARMA_DETECCION);
    TEST_BANDERAS(I_DETECCION);
    Alarma_procesaEvento(&self.alarma,EID_ALARMA_FIN_DETECCION);
    TEST_BANDERAS(0);
}

static void alarma_desarmada_debe_armarse_dando_tiempo_a_evacuar_zona_monitoreada(void)
{
    Alarma_procesaEvento(&self.alarma,EID_ALARMA_ARMAR);
    TEST_BANDERAS(I_ARMADO | I_TEMPORIZADO | T_ARMADO);
    Alarma_procesaEvento(&self.alarma,EID_ALARMA_DETECCION);
    TEST_BANDERAS(I_ARMADO | I_TEMPORIZADO | T_ARMADO | I_DETECCION);
    Alarma_procesaEvento(&self.alarma,EID_ALARMA_FIN_DETECCION);
    TEST_BANDERAS(I_ARMADO | I_TEMPORIZADO | T_ARMADO);
    Alarma_procesaEvento(&self.alarma,EID_ALARMA_TIEMPO_TERMINADO);
    TEST_BANDERAS(I_ARMADO);
}
static void probar_desarmado_y_capacidad_rearme(void)
{
    TEST_BANDERAS(0);
    alarma_desarmada_debe_indicar_actividad();
    alarma_desarmada_debe_armarse_dando_tiempo_a_evacuar_zona_monitoreada();
}
static void alarma_en_temporizacion_de_armado_debe_poder_desarmarse(void)
{
    Alarma_procesaEvento(&self.alarma,EID_ALARMA_ARMAR);
    Alarma_procesaEvento(&self.alarma,EID_ALARMA_DESARMAR);
    probar_desarmado_y_capacidad_rearme();
}
static inline void alarmaArmada(void)
{
    Alarma_procesaEvento(&self.alarma,EID_ALARMA_ARMAR);
    Alarma_procesaEvento(&self.alarma,EID_ALARMA_TIEMPO_TERMINADO);
}

static void alarma_armada_sin_deteccion_debe_ignorar_tiempo_terminado(void)
{
    alarmaArmada();
    TEST_BANDERAS(I_ARMADO);
    Alarma_procesaEvento(&self.alarma,EID_ALARMA_TIEMPO_TERMINADO);
    TEST_BANDERAS(I_ARMADO);
}
static void alarma_armada_sin_deteccion_debe_poder_desarmarse(void)
{
    alarmaArmada();
    Alarma_procesaEvento(&self.alarma,EID_ALARMA_DESARMAR);
    probar_desarmado_y_capacidad_rearme();
}
static void alarma_armada_con_deteccion_debe_poder_desarmarse_sin_disparar(void)
{
    alarmaArmada();
    TEST_BANDERAS(I_ARMADO);
    Alarma_procesaEvento(&self.alarma,EID_ALARMA_DETECCION);
    TEST_BANDERAS(I_ARMADO | I_DETECCION | I_TEMPORIZADO | T_DISPARO);
    Alarma_procesaEvento(&self.alarma,EID_ALARMA_DESARMAR);
    TEST_BANDERAS(I_DETECCION);
    Alarma_procesaEvento(&self.alarma,EID_ALARMA_FIN_DETECCION);
    probar_desarmado_y_capacidad_rearme();
}

static void alarma_armada_con_deteccion_debe_alertar_al_vencer_tiempo_de_desarme(void)
{
    alarmaArmada();
    Alarma_procesaEvento(&self.alarma,EID_ALARMA_DETECCION);
    TEST_BANDERAS(I_ARMADO | I_DETECCION | I_TEMPORIZADO | T_DISPARO);
    Alarma_procesaEvento(&self.alarma,EID_ALARMA_TIEMPO_TERMINADO);
    TEST_BANDERAS(I_ARMADO | I_DETECCION | I_ALERTA);
}

static void alarmaAlerta(void)
{
    alarmaArmada();
    Alarma_procesaEvento(&self.alarma,EID_ALARMA_DETECCION);
    Alarma_procesaEvento(&self.alarma,EID_ALARMA_TIEMPO_TERMINADO);
}

static void probar_redisparo(void)
{
    TEST_BANDERAS(I_ARMADO);
    Alarma_procesaEvento(&self.alarma,EID_ALARMA_DETECCION);
    Alarma_procesaEvento(&self.alarma,EID_ALARMA_TIEMPO_TERMINADO);
    TEST_BANDERAS(I_ARMADO | I_ALERTA | I_DETECCION);
}

static void alarma_disparada_debe_rearmarse_al_pasar_tiempo_disparo_sin_deteccion(void)
{
    alarmaAlerta();
    TEST_BANDERAS(I_ALERTA | I_DETECCION | I_ARMADO);
    Alarma_procesaEvento(&self.alarma,EID_ALARMA_FIN_DETECCION);
    TEST_BANDERAS(I_ALERTA | I_ARMADO | T_ALERTA);
    Alarma_procesaEvento(&self.alarma,EID_ALARMA_TIEMPO_TERMINADO);
    TEST_BANDERAS(I_ARMADO);
    probar_redisparo();
}
static void alarma_disparada_sin_deteccion_debe_rearmarse_al_pasar_tiempo_disparo(void)
{
    alarmaArmada();
    Alarma_procesaEvento(&self.alarma,EID_ALARMA_DETECCION);
    Alarma_procesaEvento(&self.alarma,EID_ALARMA_FIN_DETECCION);
    TEST_BANDERAS(I_ARMADO | I_TEMPORIZADO | T_DISPARO);
    Alarma_procesaEvento(&self.alarma,EID_ALARMA_TIEMPO_TERMINADO);
    TEST_BANDERAS(I_ARMADO | I_ALERTA | T_ALERTA);
    Alarma_procesaEvento(&self.alarma,EID_ALARMA_TIEMPO_TERMINADO);
    TEST_BANDERAS(I_ARMADO);
    
}
static void alarma_disparada_debe_restablecer_tiempo_disparo_si_hay_nueva_deteccion(void)
{
    alarmaAlerta();
    Alarma_procesaEvento(&self.alarma,EID_ALARMA_FIN_DETECCION);
    TEST_BANDERAS(I_ALERTA | I_ARMADO | T_ALERTA);
    Alarma_procesaEvento(&self.alarma,EID_ALARMA_DETECCION);
    TEST_BANDERAS(I_ALERTA | I_ARMADO | I_DETECCION);
}
static void alarma_disparada_con_deteccion_debe_mantenerse_disparada(void)
{
    alarmaAlerta();
    Alarma_procesaEvento(&self.alarma,EID_ALARMA_TIEMPO_TERMINADO);
    TEST_BANDERAS(I_ARMADO | I_ALERTA | I_DETECCION);
}
static void alarma_disparada_debe_poder_desarmarse(void)
{
    alarmaAlerta();
    Alarma_procesaEvento(&self.alarma,EID_ALARMA_DESARMAR);
    TEST_BANDERAS(I_DETECCION);
    Alarma_procesaEvento(&self.alarma,EID_ALARMA_FIN_DETECCION);
    TEST_BANDERAS(0);
    probar_desarmado_y_capacidad_rearme();
}
static void deteccion_en_desarmada_debe_persistir_y_disparar_la_alarma(void)
{
    Alarma_procesaEvento(&self.alarma,EID_ALARMA_DETECCION);
    Alarma_procesaEvento(&self.alarma,EID_ALARMA_ARMAR);
    Alarma_procesaEvento(&self.alarma,EID_ALARMA_TIEMPO_TERMINADO);
    TEST_BANDERAS(I_ARMADO | I_DETECCION | I_TEMPORIZADO | T_DISPARO);
    Alarma_procesaEvento(&self.alarma,EID_ALARMA_TIEMPO_TERMINADO);
    TEST_BANDERAS(I_ARMADO | I_DETECCION | I_ALERTA);
}
int main(void)
{
    TimerSysTick_init();
    TimerSysTick_esperaMilisegundos(500);
    UNITY_BEGIN();
    RUN_TEST(alarma_desarmada_debe_indicar_actividad);
    RUN_TEST(alarma_desarmada_debe_armarse_dando_tiempo_a_evacuar_zona_monitoreada);
    RUN_TEST(alarma_en_temporizacion_de_armado_debe_poder_desarmarse);
    RUN_TEST(alarma_armada_sin_deteccion_debe_ignorar_tiempo_terminado);
    RUN_TEST(alarma_armada_sin_deteccion_debe_poder_desarmarse);
    RUN_TEST(alarma_armada_con_deteccion_debe_poder_desarmarse_sin_disparar);
    RUN_TEST(alarma_armada_con_deteccion_debe_alertar_al_vencer_tiempo_de_desarme);
    RUN_TEST(alarma_disparada_debe_rearmarse_al_pasar_tiempo_disparo_sin_deteccion);
    RUN_TEST(alarma_disparada_sin_deteccion_debe_rearmarse_al_pasar_tiempo_disparo);
    RUN_TEST(alarma_disparada_debe_restablecer_tiempo_disparo_si_hay_nueva_deteccion);
    RUN_TEST(alarma_disparada_con_deteccion_debe_mantenerse_disparada);
    RUN_TEST(alarma_disparada_debe_poder_desarmarse);
    RUN_TEST(deteccion_en_desarmada_debe_persistir_y_disparar_la_alarma);
    UNITY_END();
    for(;;);
}