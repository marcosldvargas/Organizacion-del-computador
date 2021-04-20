#ifndef __CACHE_H__
#define __CACHE_H__

#include <stddef.h>
#include <stdbool.h>
#include "traza.h"

typedef struct cache cache_t;

cache_t* cache_crear (int cantidad_bloques, int cantidad_lineas, int cantidad_sets);

int establecer_modo_verboso (cache_t* cache, int n, int m);

void realizar_operacion (cache_t* cache, traza_t traza);

void imprimir_distribucion (cache_t* cache);

void imprimir_metrica (cache_t* cache);

void obtener_datos_traza (cache_t* cache, traza_t traza, uint32_t* tag, uint32_t* set, uint32_t* b_offset);

void cache_destruir (cache_t* cache);

#endif
