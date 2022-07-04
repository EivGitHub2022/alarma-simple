#ifndef COMANDO_H
#define COMANDO_H

typedef struct Comando Comando;

typedef void Comando_Accion(const Comando *);

struct Comando
{
    Comando_Accion *ejecutar;
};

static inline void Comando_ejecutar(const Comando * self)
{
    self->ejecutar(self);
}

inline void Comando_init(Comando *self,Comando_Accion *ejecutar)
{
    self->ejecutar = ejecutar;
}

#endif
