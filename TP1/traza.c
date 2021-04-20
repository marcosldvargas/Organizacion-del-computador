#include "traza.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#define FORMATO_LECTURA "%x: %c %x %i %x\n"

bool se_cargo_traza (FILE* archivo, traza_t* traza) {
    return fscanf (archivo, FORMATO_LECTURA, &traza->instruction_pointer, &traza->operacion, &traza->direccion_acceso, &traza->cantidad_bytes_operados, &traza->data) != EOF;
}