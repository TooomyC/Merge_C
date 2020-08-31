#include <stdio.h>
#include <stdlib.h>
#include "merge.h"

#define TODO_OK 0
#define ERROR_ARG 1

#define CUENTA_ARC 1
#define MOVS_ARC 2

#define ARG_BIN1 1
#define ARG_BIN2 2
#define ARG_TXT 3
#define ARG_TIPO_TXT 4

#define TAM_CUENTAS 4
#define TAM_MOV 5


///NOMBRE DEL PROGRAMA PARAMETRO 1       PARAMETRO 2        PARAMETRO 3      PARAMETRO 4
///Merge                 cuentas.dat    movimientos.dat     movimientos.txt     v
///  argv[0]                argv[1]         argv[2]           argv[3]        argv[4]

int main(int argc, char* argv[])
{
    ///PARA DESPUES: PASAR ARCHIVOS A MAIN.

    if(argc != 5)
    {
        puts("Cantidad incorrecta de argumentos.");
        return ERROR_ARG;
    }


    static tCuenta contCuenta [TAM_CUENTAS] = {  {1, "Spielberg, Steven", {31, 12, 2020}, 'a', 50000.00},
                                                {3, "Tarantino, Quentin", {31, 12, 2020}, 'c', 75000.00},
                                                {5, "Eastwood, Clint", {31, 12, 2020}, 'c', 80000.00},
                                                {11, "Kurosawa, Akira", {31, 12, 2020}, 'c', 95000.00}
                                              };

    static tMovimiento movimiento [TAM_MOV] =   {   {1, 'e', 25000.50},
                                                    {11, 'e', 44264.25},
                                                    {5, 'd', 64975.75},
                                                    {1, 'e', 10000.50},
                                                    {3, 'd', 57848.56},
                                                };

    int ret1, ret2, ret;

    ret1 = generarArchivo(argv[CUENTA_ARC], sizeof(tCuenta), contCuenta, TAM_CUENTAS);
    if(ret1 != TODO_OK)
        return ret1;

    ret2 = generarArchivo(argv[MOVS_ARC], sizeof(tMovimiento), movimiento, TAM_MOV);
    if(ret2 != TODO_OK)
        return ret2;


    ret = mostrarArchivos(argv[CUENTA_ARC], argv[MOVS_ARC]);
    if(ret != TODO_OK)
        return ret;



    ret = binATxtMov(argv[ARG_BIN2], argv[ARG_TXT], argv[ARG_TIPO_TXT][0]);
    if(ret != TODO_OK)
        return ret;

    ret = txtABinMov(argv[ARG_TXT], argv[ARG_BIN2], argv[ARG_TIPO_TXT][0]);
    if(ret != TODO_OK)
        return ret;

    ret = actualizarArchivo(argv[CUENTA_ARC], argv[MOVS_ARC]);
    if(ret != TODO_OK)
        return ret;

    ret = mostrarArchivos(argv[CUENTA_ARC], argv[MOVS_ARC]);
    if(ret != TODO_OK)
        return ret;


    system("pause");

    return 0;
}
