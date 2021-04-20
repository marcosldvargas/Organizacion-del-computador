#ifndef __TRAZA_H__
#define __TRAZA_H__

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>

typedef struct traza {
    uint32_t instruction_pointer;
    char operacion;
    uint32_t direccion_acceso;
    int cantidad_bytes_operados;
    uint32_t data;
}traza_t;

bool se_cargo_traza (FILE* archivo, traza_t* traza);

#endif
