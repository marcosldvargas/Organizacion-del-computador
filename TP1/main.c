#include "cache.h"
#include "traza.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ERROR -1
#define EXITO 0
#define LECTURA "r"
#define MODO_VERBOSO "-v"

bool argumentos_incorrectos (int argc, const char* argv[]) {

	if (argc < 5) {
		perror ("Cantidad de argumentos incorrecta.");
		return true;
	}else if ((argc > 5 && argc != 8 ) || ( argc == 8 && (strcmp(argv[5], MODO_VERBOSO) != 0))) {
		perror ("Se intentó seleccionar modo verboso pero faltan argumentos.");
		return true;
	} else if (argc == 8 && (atoi(argv[6]) < 0 || atoi(argv[7]) < 0)) {
		perror ("Los valores de extremos del modo verboso no pueden ser negativos.");
		return true;
	} else if (argc == 8 && (atoi(argv[6]) > atoi(argv[7]))) {
		perror ("El primer extremo del modo verboso no puede ser mayor que el segundo.");
		return true;
	}
	
	return false;
}

int iniciar_cache (FILE** archivo, cache_t** cache, int cant_argumentos, const char* argv[]){

	*(archivo) = fopen (argv[1], LECTURA);
	if (!(*archivo)) {
		perror ("Archivo corrupto o no encontrado.");
		return ERROR;
	}

	*(cache) = cache_crear (atoi(argv[2]), atoi(argv[3]) , atoi(argv[4]));
	if (!*(cache)) {
		perror ("Fallo la creación de la cache, verifique los parámetros.");
		fclose (*archivo);
		return ERROR;
	}

	if (cant_argumentos == 8 && establecer_modo_verboso(*cache, atoi(argv[6]), atoi(argv[7])) == ERROR) {
		cache_destruir (*cache);
		fclose (*archivo);
		return ERROR;
	}

	return EXITO;
}

int main (int argc, const char* argv[]){

	if (argumentos_incorrectos (argc, argv)) return -1;

	FILE* archivo = NULL;
	cache_t* cache = NULL;
	traza_t traza;
	
	if (iniciar_cache (&archivo, &cache, argc, argv) == ERROR) return ERROR;

	while (se_cargo_traza (archivo, &traza)) {
		realizar_operacion (cache, traza);
	}
	
	imprimir_metrica (cache);

	fclose (archivo);	
	cache_destruir (cache);

	return EXITO;
}
