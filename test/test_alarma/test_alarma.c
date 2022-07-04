#include <unity.h>
#include <alarma.h>
#include <timer_systick.h>
#include <stdbool.h>
#include <string.h>
#include "comando_cambia_bool.h"

typedef struct Flags Flags;

#define LISTAR_FLAGS(PLANTILLA) \
        PLANTILLA(iDeteccion)\
        PLANTILLA(iArmado)\
        PLANTILLA(iAlerta)\
        PLANTILLA(iTemporizado)\
        PLANTILLA(tArmado)\
        PLANTILLA(tDisparo)\
        PLANTILLA(tAlerta)

#define PF_DECLARA(nombre) bool nombre;

static struct TestAlarma {
    Alarma alarma;
    struct Flags{
        LISTAR_FLAGS(PF_DECLARA)
    }flags;
}self;

static inline bool TestAlarma_Flags_eq(const Flags * self, const Flags *other)
{
    return memcmp(self,other,sizeof(*self)) == 0;
}

#define PF_COMPARA(nombre_) TEST_ASSERT_EQUAL_MESSAGE(ref.nombre_,self.flags.nombre_,"(" #nombre_ ")");
#define TEST_FLAGS(...) do{\
    static const Flags ref =(__VA_ARGS__);\
    LISTAR_FLAGS(PF_COMPARA);\
}while(0)

static const Comando *const cmdIDeteccion       = CAMBIA_BOOL(&self.flags.iDeteccion,   true);
static const Comando *const cmdIFinDeteccion    = CAMBIA_BOOL(&self.flags.iDeteccion,   false);
static const Comando *const cmdIArmada          = CAMBIA_BOOL(&self.flags.iArmado,      true);
static const Comando *const cmdIDesarmada       = CAMBIA_BOOL(&self.flags.iArmado,      false);
static const Comando *const cmdIAlerta          = CAMBIA_BOOL(&self.flags.iAlerta,      true);
static const Comando *const cmdIFinAlerta       = CAMBIA_BOOL(&self.flags.iAlerta,      false);
static const Comando *const cmdITemporizado     = CAMBIA_BOOL(&self.flags.iTemporizado, true);
static const Comando *const cmdIFinTemporizado  = CAMBIA_BOOL(&self.flags.iTemporizado, false);
static const Comando *const cmdTArmado          = CAMBIA_BOOL(&self.flags.tArmado,      true);
static const Comando *const cmdNTArmado         = CAMBIA_BOOL(&self.flags.tArmado,      false);
static const Comando *const cmdTDisparo         = CAMBIA_BOOL(&self.flags.tDisparo,     true);
static const Comando *const cmdNTDisparo        = CAMBIA_BOOL(&self.flags.tDisparo,     false);
static const Comando *const cmdTAlerta          = CAMBIA_BOOL(&self.flags.tAlerta,      true);
static const Comando *const cmdNTAlerta         = CAMBIA_BOOL(&self.flags.tAlerta,      false);
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
    TEST_FLAGS((Flags){});
    Alarma_procesaEvento(&self.alarma,EID_ALARMA_DETECCION);
    TEST_FLAGS((Flags){.iDeteccion=true});
    Alarma_procesaEvento(&self.alarma,EID_ALARMA_FIN_DETECCION);
    TEST_FLAGS((Flags){});
}

static void alarma_desarmada_debe_armarse_dando_tiempo_a_evacuar_zona_monitoreada(void)
{
    Alarma_procesaEvento(&self.alarma,EID_ALARMA_ARMAR);
    TEST_FLAGS((Flags){.iArmado=true, .iTemporizado=true, .tArmado=true});
    Alarma_procesaEvento(&self.alarma,EID_ALARMA_DETECCION);
    TEST_FLAGS((Flags){.iArmado=true, .iTemporizado=true, .tArmado=true,.iDeteccion=true});
    Alarma_procesaEvento(&self.alarma,EID_ALARMA_FIN_DETECCION);
    TEST_FLAGS((Flags){.iArmado=true, .iTemporizado=true, .tArmado=true});
    Alarma_procesaEvento(&self.alarma,EID_ALARMA_TIEMPO_TERMINADO);
    TEST_FLAGS((Flags){.iArmado=true});
}
static void probar_desarmado_y_capacidad_rearme(void)
{
    TEST_FLAGS((Flags){});
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
    self.flags.tArmado = false;
}

static void alarma_armada_sin_deteccion_debe_ignorar_tiempo_terminado(void)
{
    alarmaArmada();
    TEST_FLAGS((Flags){.iArmado=true});
    Alarma_procesaEvento(&self.alarma,EID_ALARMA_TIEMPO_TERMINADO);
    TEST_FLAGS((Flags){.iArmado=true});
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
    TEST_FLAGS((Flags){.iArmado=true});
    Alarma_procesaEvento(&self.alarma,EID_ALARMA_DETECCION);
    TEST_FLAGS((Flags){.iArmado=true,.iDeteccion=true,.iTemporizado=true,.tDisparo=true});
    Alarma_procesaEvento(&self.alarma,EID_ALARMA_DESARMAR);
    TEST_FLAGS((Flags){.iArmado=false,.iDeteccion=true,.iTemporizado=false,.tDisparo=false});
    Alarma_procesaEvento(&self.alarma,EID_ALARMA_FIN_DETECCION);
    probar_desarmado_y_capacidad_rearme();
}

static void alarma_armada_con_deteccion_debe_alertar_al_vencer_tiempo_de_desarme(void)
{
    alarmaArmada();
    Alarma_procesaEvento(&self.alarma,EID_ALARMA_DETECCION);
    TEST_FLAGS((Flags){.iArmado=true,.iDeteccion=true,.iTemporizado=true,.tDisparo=true});
    Alarma_procesaEvento(&self.alarma,EID_ALARMA_TIEMPO_TERMINADO);
    TEST_FLAGS((Flags){.iArmado=true,.iDeteccion=true,.iAlerta=true});
}

static void alarmaAlerta(void)
{
    alarmaArmada();
    Alarma_procesaEvento(&self.alarma,EID_ALARMA_DETECCION);
    Alarma_procesaEvento(&self.alarma,EID_ALARMA_TIEMPO_TERMINADO);
}

static void probar_redisparo(void)
{
    TEST_FLAGS((Flags){.iArmado=true});
    Alarma_procesaEvento(&self.alarma,EID_ALARMA_DETECCION);
    Alarma_procesaEvento(&self.alarma,EID_ALARMA_TIEMPO_TERMINADO);
    TEST_FLAGS((Flags){.iArmado=true,.iAlerta=true,.iDeteccion=true});
}

static void alarma_disparada_debe_rearmarse_al_pasar_tiempo_disparo_sin_deteccion(void)
{
    alarmaAlerta();
    TEST_FLAGS((Flags){.iAlerta=true,.iDeteccion=true,.iArmado=true});
    Alarma_procesaEvento(&self.alarma,EID_ALARMA_FIN_DETECCION);
    TEST_FLAGS((Flags){.iAlerta=true,.iArmado=true,.tAlerta=true});
    Alarma_procesaEvento(&self.alarma,EID_ALARMA_TIEMPO_TERMINADO);
    TEST_FLAGS((Flags){.iArmado=true});
    probar_redisparo();
}
static void alarma_disparada_sin_deteccion_debe_rearmarse_al_pasar_tiempo_disparo(void)
{
    alarmaArmada();
    Alarma_procesaEvento(&self.alarma,EID_ALARMA_DETECCION);
    Alarma_procesaEvento(&self.alarma,EID_ALARMA_FIN_DETECCION);
    TEST_FLAGS((Flags){.iArmado=true,.iTemporizado=true,.tDisparo=true});
    Alarma_procesaEvento(&self.alarma,EID_ALARMA_TIEMPO_TERMINADO);
    TEST_FLAGS((Flags){.iArmado=true,.iAlerta=true,.tAlerta=true});
    Alarma_procesaEvento(&self.alarma,EID_ALARMA_TIEMPO_TERMINADO);
    TEST_FLAGS((Flags){.iArmado=true});
    
}
static void alarma_disparada_debe_restablecer_tiempo_disparo_si_hay_nueva_deteccion(void)
{
    alarmaAlerta();
    Alarma_procesaEvento(&self.alarma,EID_ALARMA_FIN_DETECCION);
    TEST_FLAGS((Flags){.iAlerta=true,.iArmado=true,.iDeteccion=false,.tAlerta=true});
    Alarma_procesaEvento(&self.alarma,EID_ALARMA_DETECCION);
    TEST_FLAGS((Flags){.iAlerta=true,.iArmado=true,.iDeteccion=true,.tAlerta=false});
}
static void alarma_disparada_con_deteccion_debe_mantenerse_disparada(void)
{
    alarmaAlerta();
    Alarma_procesaEvento(&self.alarma,EID_ALARMA_TIEMPO_TERMINADO);
    TEST_FLAGS((Flags){.iArmado=true,.iAlerta=true,.iDeteccion=true});
}
static void alarma_disparada_debe_poder_desarmarse(void)
{
    alarmaAlerta();
    Alarma_procesaEvento(&self.alarma,EID_ALARMA_DESARMAR);
    TEST_FLAGS((Flags){.iDeteccion=true});
    Alarma_procesaEvento(&self.alarma,EID_ALARMA_FIN_DETECCION);
    TEST_FLAGS((Flags){});
    probar_desarmado_y_capacidad_rearme();
}
static void deteccion_en_desarmada_debe_persistir_y_disparar_la_alarma(void)
{
    Alarma_procesaEvento(&self.alarma,EID_ALARMA_DETECCION);
    Alarma_procesaEvento(&self.alarma,EID_ALARMA_ARMAR);
    Alarma_procesaEvento(&self.alarma,EID_ALARMA_TIEMPO_TERMINADO);
    TEST_FLAGS((Flags){.iArmado=true,.iDeteccion=true,.iTemporizado=true,.tDisparo=true});
    Alarma_procesaEvento(&self.alarma,EID_ALARMA_TIEMPO_TERMINADO);
    TEST_FLAGS((Flags){.iArmado=true,.iDeteccion=true,.iAlerta=true});
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