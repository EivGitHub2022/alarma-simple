#include <puerto_gpio_inst.h>
#include <stdint.h>
#include <stm32f1xx.h>
#include <stdbool.h>

void PuertoGpio_init(PuertoGpio *self,GPIO_TypeDef *base, volatile uint32_t *regHabReloj,int bitHabReloj)
{
    self->base                          = base;
    self->habilitacionReloj.mascara     = (1UL<<bitHabReloj);
    self->habilitacionReloj.registro    = regHabReloj;
}

#define LIT_PUERTO_GPIO(base_,hab_,bitHab_) ((const PuertoGpio){\
    .base               = (base_),\
    .habilitacionReloj  = { .mascara    = (1UL<<(bitHab_)),\
                            .registro   = (hab_)}\
})

#define LISTA_PUERTOS(PLANTILLA)\
        PLANTILLA(A)\
        PLANTILLA(B)\
        PLANTILLA(C)\
        PLANTILLA(D)\
        PLANTILLA(E)

#define DEF_CONST(X_)    const PuertoGpio *const puerto##X_ =\
     &LIT_PUERTO_GPIO(GPIO##X_,&RCC->APB2ENR,RCC_APB2ENR_IOP##X_##EN_Pos);

LISTA_PUERTOS(DEF_CONST)


inline static volatile uint32_t* PuertoGpio__CR(const PuertoGpio *self, int nrPin)
{
    volatile uint32_t *cr;
    if (nrPin < 8)
        cr = &self->base->CRL;
    else
        cr = &self->base->CRH;
    return cr;
}
inline static int posicionCRPin(int nrPin)
{
    int pos;
    if (nrPin < 8)
        pos = nrPin*4;
    else
        pos = (nrPin-8)*4;
    return pos;
}
#define MASCARA_CFG 0xFUL

static inline void PuertoGpio__configPin(const PuertoGpio *self,const int nrPin, const uint32_t cfg)
{
    const int pos                   = posicionCRPin(nrPin);
    const uint32_t crMCero          = ~(MASCARA_CFG << pos); 
    const uint32_t crMUno           = ((uint32_t)(cfg & MASCARA_CFG)) << pos;
    volatile uint32_t *const cr     = PuertoGpio__CR(self,nrPin);
    const uint32_t mascaraHab       = self->habilitacionReloj.mascara;
    volatile uint32_t *const hab    = self->habilitacionReloj.registro;

    __disable_irq();
    *hab |= mascaraHab;
    *cr = (*cr & crMCero) | crMUno;
    __enable_irq();
}

void PuertoGpio_configPinEFlotante  (const PuertoGpio *self,int nrPin, Pin *pin)
{
    PuertoGpio__configPin(self,nrPin,0b0100);
    Pin_init(pin,self->base,nrPin);
}
void PuertoGpio_configPinSLenta     (const PuertoGpio *self,int nrPin, Pin *pin)
{
    PuertoGpio__configPin(self,nrPin,0b0010);
    Pin_init(pin,self->base,nrPin);
}