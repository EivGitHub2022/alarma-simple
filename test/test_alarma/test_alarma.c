#include <unity.h>
#include <alarma.h>
#include <timer_systick.h>
#include <stdbool.h>
#include <string.h>
#include "comando_mascara.h"

typedef struct Flags Flags;

/**
 * @brief Banderas que representan indicadores (I_) y timers (T_)
 * 
 */
typedef enum Banderas{
    I_DETECCION     = 0x01,
    I_ARMADO        = 0x02,
    I_ALERTA        = 0x04,
    I_TEMPORIZADO   = 0x08,
    T_ARMADO        = 0x10,
    T_DISPARO       = 0x20,
    T_ALERTA        = 0x40
}Banderas;


static struct TestAlarma {
    Alarma alarma;
    int banderas;
}self;

static void copia_elem(char * restrict * restrict destino, const char * restrict * restrict origen,const char * fin_destino)
{
    const size_t capacidad = fin_destino-*destino;
    const size_t nOrigen = strlen(*origen);
    const size_t n = (nOrigen + 2 <= capacidad) ? nOrigen: capacidad - 2;
    if (!n) return;
    memcpy(*destino,*origen,n);
    *destino += n; 
    **destino=' ';
    *(++*destino)=0;
    *origen += nOrigen + 1;
}
static void salta_elem(const char *restrict *restrict origen)
{
    const size_t nOrigen = strlen(*origen);
    *origen += nOrigen + 1;
}
static void copia_mascara(char * restrict * restrict destino,size_t lDestino,const char * restrict lista,size_t lLista,unsigned int mascara)
{
    const char *destino_fin = *destino + lDestino;
    const char *lista_fin   = lista + lLista;
    if (lDestino)
        **destino = 0;

    while (*destino < destino_fin && lista < lista_fin && mascara){
        const bool copiar = mascara & 1;
        mascara >>= 1;
        if(copiar)
            copia_elem(destino,&lista,destino_fin);
        else
            salta_elem(&lista);
    } 
}

static const char *comparacion(const int esperado,const int obtenido)
{
    static const char banderas[]="I_DETECCION\000I_ARMADO\000I_ALERTA\000I_TEMPORIZADO\000T_ARMADO\000T_DISPARO\000T_ALERTA";
    static const char etiquetas[]="APAGADAS:[\000]\000ENCENDIDAS:[\000]";
    static char mensaje[sizeof(banderas)+sizeof(etiquetas)+4];

    char *pmsg = mensaje;
    char *const fmsg = mensaje + sizeof(mensaje);
    const char *petiq = etiquetas;

    const int diferencia = esperado ^ obtenido;
    const int apagadas = diferencia & esperado;
    const int encendidas = diferencia & obtenido;

    mensaje[0]=0;
    if (apagadas){
        copia_elem(&pmsg,&petiq,fmsg);
        if (fmsg>pmsg)
            copia_mascara(&pmsg,fmsg-pmsg,banderas,sizeof(banderas),apagadas);
        copia_elem(&pmsg,&petiq,fmsg);
    }else{
        salta_elem(&petiq);
        if (fmsg>pmsg)
            copia_mascara(&pmsg,fmsg-pmsg,banderas,sizeof(banderas),encendidas);
        salta_elem(&petiq);
    }
    return mensaje;
}
#define TEST_BANDERAS(banderas_) TEST_ASSERT_EQUAL_HEX8_MESSAGE(banderas_,self.banderas,comparacion(banderas_,self.banderas))

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