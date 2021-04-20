#include "cache.h"
#include "traza.h"
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#define ERROR -1
#define EXITO 0
#define TAMANIO_PALABRA 32
#define LECTURA 'R'
#define MAX 3

typedef struct verboso {
	int indice_operacion;
	char identificador_caso[MAX];
	int set_indice; 
	int tag;
	int linea_indice;
	int tag_anterior; 
	int valid_bit;
	int dirty_bit;
	int indice_ultima_operacion;
	int tiempo_uso;
}verboso_t;

typedef struct metrica {
	int lecturas;
	int escrituras;
	float accesos_total;
	int misses_lectura;
	int misses_escritura;
	float misses_total;
	int dirty_miss_lectura;
	int dirty_miss_escritura;
	int bytes_leidos;
	int bytes_escritos;
	int tiempo_acceso_acumulado_r;
	int tiempo_acceso_acumulado_w;
	float miss_rate_total;
}metrica_t;

typedef struct distribucion {
	int cantidad_sets;
	int cantidad_lineas;
	int cantidad_bloques;
	double bits_set;
	double bits_tag;
	double bits_offset;
}distribucion_t;

typedef struct linea {
	int tag_anterior;
	int indice;
	int dirty_bit;
	int valid_bit;
	uint32_t tag;
	uint32_t* bloques;
	int indice_ultima_operacion;
	int tiempo_acumulado;
}linea_t;

typedef struct set {
	linea_t* lineas;
}set_t;

struct cache {
	int tamanio;
	int ciclos; 
	set_t* sets;
	int bits_set;
	distribucion_t distribucion;
	metrica_t* metricas;
	bool modo_verboso;
	verboso_t* verboso_metricas;
	int indice_operacion_verboso;
	int verboso_inicio;
	int verboso_fin;
	int indice_operacion;
	int penalty; 
};
 
bool es_potencia_de_dos (int x) {
	return (x & (x - 1)) == 0; 
}

bool parametros_invalidos (int C, int E, int S) {
	return (!es_potencia_de_dos(C) || !es_potencia_de_dos(E) || !es_potencia_de_dos(S));
}

int crear_sets (cache_t* cache, int S) {

	cache->sets = (set_t*) calloc ((size_t)S, sizeof(set_t));
	if (!cache->sets){
		free (cache);
		return ERROR;
	}	
	return EXITO;
}

int crear_lineas (cache_t* cache, int S, int E) {

	int i = 0;
	bool fallo = false;

	while (i < S && !fallo) {
		cache->sets[i].lineas = (linea_t*) calloc ((size_t)E, sizeof(linea_t)); 
		if (!cache->sets[i].lineas) {
			if (i != 0) for (int j = 0; j < i; j++) free (cache->sets[j].lineas);
			fallo = true;
		}
		//les pongo el indice
		for (int j = 0; j < E; j++) cache->sets[i].lineas[j].indice = j;
		i++;
	}

	if (fallo) {
		for (int j = 0; j < S; j++) free (cache->sets+j);
		free (cache);
		return ERROR;
	}
	return EXITO;
}

int crear_bloques (cache_t* cache, int S, int E, int B) {

	int i = 0;
	int j = 0;
	bool fallo = false;
	
	while (i < S && !fallo) {
		while (j < E && !fallo) {
			cache->sets[i].lineas[j].bloques = (uint32_t*) calloc ((size_t)B, sizeof(uint32_t));
			if (!cache->sets[i].lineas[j].bloques) fallo = true;
			j++;
		}
		i++;
	}

	if (fallo) {
		for (int m = 0; m < S; m++) {
			for (int n = 0; n < E; n++) {
				free (cache->sets[m].lineas[n].bloques);
			}
			free (cache->sets[m].lineas);
		}
		free (cache->sets);
		free (cache);
		return ERROR;
	}
	return EXITO;
}

int inicializar_cache (cache_t* cache, int C, int E, int S) {
	
	cache->ciclos = 0;
	cache->penalty = 100;
	cache->tamanio = C;
	cache->indice_operacion = 0;
	cache->modo_verboso = false;
	cache->distribucion.cantidad_sets = S;
	cache->distribucion.cantidad_lineas = E;
	cache->distribucion.cantidad_bloques = C / (E * S);
	cache->distribucion.bits_set = log2(S);
	cache->distribucion.bits_offset = log2(cache->distribucion.cantidad_bloques);
	cache->distribucion.bits_tag = TAMANIO_PALABRA - cache->distribucion.bits_set - cache->distribucion.bits_offset;

	if ((crear_sets(cache, S) == ERROR) || (crear_lineas(cache, S, E) == ERROR) || (crear_bloques(cache, S, E, cache->distribucion.cantidad_bloques) == ERROR)) return ERROR;

	return EXITO;
}

cache_t* cache_crear(int C, int E, int S){
	
	if (parametros_invalidos(C, E, S)) return NULL;

	cache_t* cache = (cache_t*) calloc (1, sizeof(cache_t));
	if (!cache) return NULL;

	cache->metricas = (metrica_t*) calloc (1, sizeof(metrica_t));
	if (!cache->metricas) {
		free (cache);
		return NULL;
	}

	if (inicializar_cache (cache, C, E, S) == ERROR) {
		free (cache->metricas);
		free (cache);
		return NULL;
	}
	return cache;
}

int establecer_modo_verboso (cache_t* cache, int n, int m) {

	int inicio = n;
	int fin = m + 1;
	int rango = fin - inicio;

	cache->verboso_metricas = (verboso_t*) calloc ((size_t)(rango), sizeof(verboso_t));
	if (!cache->verboso_metricas) {
		cache_destruir (cache);
		return ERROR;
	}

	cache->verboso_inicio = n;
	cache->verboso_fin = m;
	cache->modo_verboso = true;

	for (int i = 0; i < rango; i++) {
		cache->verboso_metricas[i].indice_operacion = n;
		n++;
	}
	return EXITO;
}

void cargar_verboso_hit (cache_t* cache, verboso_t* verboso_linea, uint32_t set, uint32_t tag, int indice_linea, int dirty_bit_anterior) {

	strcpy (verboso_linea->identificador_caso, "1");
	verboso_linea->set_indice = set;
	verboso_linea->tag = tag;
	verboso_linea->linea_indice = indice_linea;
	verboso_linea->tag_anterior = tag;
	verboso_linea->valid_bit = 1;
	verboso_linea->dirty_bit = dirty_bit_anterior;
	if (cache->distribucion.cantidad_lineas > 1) verboso_linea->indice_ultima_operacion = cache->sets[set].lineas[indice_linea].indice_ultima_operacion;
	verboso_linea->tiempo_uso = cache->ciclos;
}

bool hubo_hit (cache_t* cache, traza_t traza, uint32_t tag, uint32_t set, verboso_t* verboso_linea) {

	int i = 0;
	int dirty_bit_anterior = 0;
	bool encontrado = false;

	while (i < cache->distribucion.cantidad_lineas && !encontrado) {
		if (cache->sets[set].lineas[i].tag == tag && cache->sets[set].lineas[i].valid_bit == 1) {
			
			dirty_bit_anterior = cache->sets[set].lineas[i].dirty_bit;
			
			if (traza.operacion == LECTURA) cache->metricas->tiempo_acceso_acumulado_r++;
			else {
				cache->sets[set].lineas[i].dirty_bit = 1;
				cache->metricas->tiempo_acceso_acumulado_w++;
			}

			//cargo verboso
			if (verboso_linea) cargar_verboso_hit (cache, verboso_linea, set, tag, i, dirty_bit_anterior);

			cache->sets[set].lineas[i].indice_ultima_operacion = cache->indice_operacion;
			cache->sets[set].lineas[i].tiempo_acumulado = cache->ciclos + 1;

			encontrado = true;
		}
		i++;
	}
	return encontrado;
}

void obtener_datos_traza (cache_t* cache, traza_t traza, uint32_t* tag, uint32_t* set, uint32_t* b_offset) {

	uint32_t b_offset_mascara = 0x1;
	for (int i = 0; i < cache->distribucion.bits_offset - 1; i++) b_offset_mascara = b_offset_mascara << 1 | 1;
	*(b_offset) = traza.direccion_acceso & b_offset_mascara;

	uint32_t set_mascara = 0x1;
	for (int i = 0; i < (cache->distribucion.bits_set + cache->distribucion.bits_offset) - 1; i++) set_mascara = set_mascara << 1 | 1;
	set_mascara = (set_mascara ^ b_offset_mascara) >> (uint32_t)cache->distribucion.bits_offset;
	*(set) = traza.direccion_acceso >> (uint32_t)cache->distribucion.bits_offset & set_mascara;    

	uint32_t tag_mascara = 0x1;
	for (int i = 0; i < (TAMANIO_PALABRA - cache->distribucion.bits_set - cache->distribucion.bits_offset) - 1; i++) tag_mascara = tag_mascara << 1 | 1;
	*(tag) = traza.direccion_acceso >> (uint32_t)(cache->distribucion.bits_set + cache->distribucion.bits_offset) & tag_mascara;

}

void actualizar_cache (cache_t* cache, traza_t traza, bool hit, bool dirty_miss, bool modo_verboso) {

	if (traza.operacion == LECTURA) cache->metricas->lecturas++;
	else cache->metricas->escrituras++;

	cache->metricas->accesos_total = (float)(cache->metricas->escrituras + cache->metricas->lecturas);

	if (hit) cache->ciclos++;
	else {
		if (traza.operacion == LECTURA) {
			cache->metricas->misses_lectura++;
			if (dirty_miss) {
				cache->metricas->dirty_miss_lectura++;
				cache->metricas->bytes_escritos += cache->distribucion.cantidad_bloques;
			}
		} else {
			cache->metricas->misses_escritura++;
			if (dirty_miss) {
				cache->metricas->dirty_miss_escritura++;
				cache->metricas->bytes_escritos += cache->distribucion.cantidad_bloques;
				cache->ciclos += cache->penalty;
			}
		}
		cache->metricas->bytes_leidos += cache->distribucion.cantidad_bloques;
		cache->metricas->misses_total = (float) (cache->metricas->misses_escritura + cache->metricas->misses_lectura);
		cache->ciclos += cache->penalty + 1;
	}

	cache->metricas->miss_rate_total = cache->metricas->misses_total / cache->metricas->accesos_total;
	cache->indice_operacion++;
	if(modo_verboso) cache->indice_operacion_verboso++;	
}

void cargar_verboso_miss (cache_t* cache, verboso_t* verboso_linea, uint32_t set, uint32_t tag, linea_t* linea_a_desalojar, int dirty_bit_anterior, int index_linea_libre) {

	strcpy (verboso_linea->identificador_caso, dirty_bit_anterior == 1 ? "2b" : "2a");

	verboso_linea->set_indice = set;
	verboso_linea->tag = tag;
	verboso_linea->linea_indice = index_linea_libre;

	verboso_linea->tag_anterior = (linea_a_desalojar->valid_bit == 1) ? (int)linea_a_desalojar->tag : -1;

	verboso_linea->valid_bit = linea_a_desalojar->valid_bit;
	verboso_linea->dirty_bit = dirty_bit_anterior;

	if (cache->distribucion.cantidad_lineas > 1) verboso_linea->indice_ultima_operacion = linea_a_desalojar->indice_ultima_operacion;
	verboso_linea->tiempo_uso = linea_a_desalojar->tiempo_acumulado;
}

bool hay_dirty_miss (cache_t* cache, traza_t traza, uint32_t tag, uint32_t set, verboso_t* verboso_linea) {

	int index_linea_libre = 0;
	int i = 0;
	int dirty_bit_anterior = 0;
	bool dirty_miss = false;
	bool hay_linea_libre = false; 
	linea_t* linea_a_desalojar = NULL;

	//busco la primer linea libre
	while (i < cache->distribucion.cantidad_lineas && !hay_linea_libre) {
		if (cache->sets[set].lineas[i].valid_bit == 0) {
			linea_a_desalojar = &cache->sets[set].lineas[i];
			hay_linea_libre = true;
			index_linea_libre = i;
		}
		i++;
	}	
	//no habian lineas no validas disponibles, busco una valida
	if (!hay_linea_libre) {
		linea_a_desalojar = &cache->sets[set].lineas[0];
		for (int j = 1; j < cache->distribucion.cantidad_lineas; j++) {
			if (linea_a_desalojar->tiempo_acumulado > cache->sets[set].lineas[j].tiempo_acumulado)
				linea_a_desalojar = &cache->sets[set].lineas[j];
		}
	}

	dirty_bit_anterior = linea_a_desalojar->dirty_bit;
	linea_a_desalojar->tiempo_acumulado = cache->ciclos + 1 + cache->penalty;
	dirty_miss = linea_a_desalojar->dirty_bit == 1? true : false;

	if (traza.operacion == LECTURA) {
		cache->metricas->tiempo_acceso_acumulado_r += 1 + cache->penalty;
		if (dirty_miss)
			cache->metricas->tiempo_acceso_acumulado_r += cache->penalty;
		linea_a_desalojar->dirty_bit = 0;
	} else {
		linea_a_desalojar->dirty_bit = 1;
		cache->metricas->tiempo_acceso_acumulado_w += 1 + cache->penalty;
		if (dirty_miss) {
			cache->metricas->tiempo_acceso_acumulado_w += cache->penalty;
			linea_a_desalojar->tiempo_acumulado += cache->penalty;
		}
	}
 
	if (verboso_linea) cargar_verboso_miss (cache, verboso_linea, set, tag, linea_a_desalojar, dirty_bit_anterior, index_linea_libre);

	linea_a_desalojar->indice_ultima_operacion = cache->indice_operacion;
	linea_a_desalojar->valid_bit = 1;
	linea_a_desalojar->tag = tag;
	
	return dirty_miss;
}

void realizar_operacion (cache_t* cache, traza_t traza) {

	uint32_t tag = 0;
	uint32_t set = 0;
	uint32_t b_offset = 0;
	bool modo_verboso = cache->modo_verboso;
	bool hit = false;
	bool dirty_miss = false;
	verboso_t* linea_verboso = NULL;

	if (cache->indice_operacion < cache->verboso_inicio || cache->indice_operacion > cache->verboso_fin) modo_verboso = false;

	obtener_datos_traza (cache, traza, &tag, &set, &b_offset);

	if (modo_verboso) linea_verboso = &cache->verboso_metricas[cache->indice_operacion_verboso];
		
	hit = hubo_hit (cache, traza, tag, set, linea_verboso);
	dirty_miss = hit ? false : hay_dirty_miss (cache, traza, tag, set, linea_verboso);
	
	if (modo_verboso) imprimir_distribucion (cache);

	actualizar_cache (cache, traza, hit, dirty_miss, modo_verboso);
}

void imprimir_metrica (cache_t* cache) {
	printf ("\n%i-way, %i sets, size = %iKB\n", cache->distribucion.cantidad_lineas, cache->distribucion.cantidad_sets, cache->tamanio/1024);
	printf ("loads %i stores %i total %i\n", cache->metricas->lecturas, cache->metricas->escrituras, (int)cache->metricas->accesos_total);
	printf ("rmiss %i wmiss %i total %i\n", cache->metricas->misses_lectura, cache->metricas->misses_escritura, (int)cache->metricas->misses_total);
	printf ("dirty rmiss %i dirty wmiss %i\n", cache->metricas->dirty_miss_lectura, cache->metricas->dirty_miss_escritura);
	printf ("bytes read %i bytes written %i\n", cache->metricas->bytes_leidos, cache->metricas->bytes_escritos);
	printf ("read time %i write time %i\n", cache->metricas->tiempo_acceso_acumulado_r, cache->metricas->tiempo_acceso_acumulado_w);
	printf ("miss rate %f\n", cache->metricas->miss_rate_total);	
}

void imprimir_distribucion (cache_t* cache) {
	verboso_t verboso = cache->verboso_metricas[cache->indice_operacion_verboso];
	if (verboso.tag_anterior == -1)
		printf ("%i %s %x %x %x %i %i %i\n", verboso.indice_operacion, verboso.identificador_caso, verboso.set_indice, verboso.tag, verboso.linea_indice, verboso.tag_anterior, verboso.valid_bit, verboso.dirty_bit);
	
	if (cache->distribucion.cantidad_lineas == 1)
		printf ("%i %s %x %x %x %x %i %i\n", verboso.indice_operacion, verboso.identificador_caso, verboso.set_indice, verboso.tag, verboso.linea_indice, verboso.tag_anterior, verboso.valid_bit, verboso.dirty_bit);
	else 
		printf ("%i %s %x %x %x %x %i %i %i\n", verboso.indice_operacion, verboso.identificador_caso, verboso.set_indice, verboso.tag, verboso.linea_indice, verboso.tag_anterior, verboso.valid_bit, verboso.dirty_bit, verboso.indice_ultima_operacion);
}

void cache_destruir (cache_t* cache) {

	if (!cache) return;

	for (int i = 0; i < cache->distribucion.cantidad_sets; i++) {
		for (int j = 0; j < cache->distribucion.cantidad_lineas; j++) {
			free (cache->sets[i].lineas[j].bloques);
		}
		free (cache->sets[i].lineas);
	}
	free (cache->sets);
	free (cache->verboso_metricas);
	free (cache->metricas);
	free (cache);
}	
