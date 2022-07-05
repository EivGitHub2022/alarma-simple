#include "banderas.h"
#include <string.h>

#define STR_(nombre_) #nombre_
#define PLANTILLA_STRING_(nombre_,valor_) STR_(nombre_)
#define PLANTILLA_ARRAY_STRING_(nombre_,valor_) [valor_] = STR_(nombre_),

#define LONGITUD_STR_BANDERAS sizeof(LISTA_BANDERAS(PLANTILLA_STRING_))

const char *const nombresBanderas[NUM_BANDERAS] = {LISTA_BANDERAS(PLANTILLA_ARRAY_STRING_)};

static void copia_elem(char * restrict * restrict destino,const char * fin_destino, const char * restrict origen)
{
    if(!origen || !destino || !*destino || !fin_destino) return;
    const size_t capacidad = fin_destino-*destino;
    const size_t nOrigen = strlen(origen);
    const size_t n = (nOrigen + 2 <= capacidad) ? nOrigen: capacidad - 2;
    if (!n) return;
    memcpy(*destino,origen,n);
    *destino += n; 
    **destino=0;
}


#define STR_ETIQUETAS(nombre,cadena,multiplicidad) [nombre] = cadena,
#define ENUM_ETIQUETAS(nombre,cadena,multiplicidad) nombre,

#define CUENTA_L_ETIQUETAS(nombre,cadena,multiplicidad) +((multiplicidad)*(sizeof(cadena)-1))

#define LISTA_ETIQUETAS(PLANTILLA)\
            PLANTILLA(ROTULO_NO_DIF,"Sin diferencias.",0)\
            PLANTILLA(ROTULO,"Distinto a lo esperado: ",1)\
            PLANTILLA(APAGADAS,"apagadas [",1)\
            PLANTILLA(ENCENDIDAS,"encendidas [",1)\
            PLANTILLA(FIN_GRUPO,"]",2)\
            PLANTILLA(SEPARADOR,", ",(NUM_BANDERAS-1))\
            PLANTILLA(TERMINADOR,".",1)

enum Etiquetas {LISTA_ETIQUETAS(ENUM_ETIQUETAS) NUM_ETIQUETAS};

static const char *const etiquetas[NUM_ETIQUETAS]={LISTA_ETIQUETAS(STR_ETIQUETAS)};

#define LMAX_ETIQUETAS (1UL LISTA_ETIQUETAS(CUENTA_L_ETIQUETAS))

/**
 * @brief Inserta en un buffer una cadena representando las banderas activas,
 *        con terminador nulo.
 * 
 * @param p_destino Puntero a buffer de destino . Al entrar debe apuntar al
 *                  punto de inserción. Al salir apunta al terminador nulo
 * @param fin_destino Puntero a la posición siguiente al fin del buffer de
 *                  destino. Nunca es dereferenciado.
 * @param banderas  Entero conteniendo el or lógico de las banderas. 
 */
static void Banderas_string(Banderas banderas, char * restrict * p_destino,
                                const char * fin_destino)
{
    bool inicio = true;
    for(int i = 0; i<NUM_BANDERAS;++i)
    {
        if(banderas & (1<<i)){
            if (inicio)
                inicio = false;
            else
                copia_elem(p_destino,fin_destino,etiquetas[SEPARADOR]);
            copia_elem(p_destino,fin_destino,nombresBanderas[i]);
        }
    }
}
const char *Banderas_mensajeDiferencias(const Banderas esperado,const Banderas obtenido)
{
    static char mensaje[LONGITUD_STR_BANDERAS+LMAX_ETIQUETAS];
    char *p_mensaje = mensaje;
    char *const fin_mensaje = mensaje + sizeof(mensaje);

    const Banderas diferencia = (esperado ^ obtenido) & ((1ULL<<NUM_BANDERAS)-1ULL);
    const Banderas apagadas = diferencia & esperado;
    const Banderas encendidas = diferencia & obtenido;

    if (!encendidas && !apagadas)
        copia_elem(&p_mensaje,fin_mensaje,etiquetas[ROTULO_NO_DIF]);
    else{
        copia_elem(&p_mensaje,fin_mensaje,etiquetas[ROTULO]);
        if (apagadas){
            copia_elem(&p_mensaje,fin_mensaje,etiquetas[APAGADAS]);
            Banderas_string(apagadas, &p_mensaje,fin_mensaje);
            copia_elem(&p_mensaje,fin_mensaje,etiquetas[FIN_GRUPO]);
        }
        if (encendidas){
            if (apagadas)
                copia_elem(&p_mensaje,fin_mensaje,etiquetas[SEPARADOR]);
            copia_elem(&p_mensaje,fin_mensaje,etiquetas[ENCENDIDAS]);
            Banderas_string(encendidas,&p_mensaje,fin_mensaje);
        }
        copia_elem(&p_mensaje,fin_mensaje,etiquetas[TERMINADOR]);
    }
    return mensaje;
}

bool Banderas_sonDistintas(Banderas esperado, Banderas obtenido)
{
    return (esperado ^ obtenido) & ((1ULL << NUM_BANDERAS)-1ULL);
}
bool Banderas_sonIguales(Banderas esperado, Banderas obtenido)
{
    return ! Banderas_sonDistintas(esperado,obtenido);
}