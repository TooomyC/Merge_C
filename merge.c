#include <stdio.h>
#include <string.h>
#include "merge.h"

#define TODO_OK 0
#define ERROR_ARG 1
#define ERROR_ARCH 2
#define ERROR_LINEA_LARGA 3

#define CUENTA_ARC 1
#define MOVS_ARC 2

#define TAM_LINEA 501

#define FALSO 0
#define VERDADERO 1


int generarArchivo(const char* argumento, size_t estructura, const void* contenido, int cantElem)
{
    FILE* archivo = fopen(argumento, "wb");
    if(!archivo)
    {
        puts("Error abriendo archivo.");
        return ERROR_ARCH;
    }

    fwrite(contenido, estructura, cantElem, archivo);
    fclose(archivo);

    return TODO_OK;
}

int mostrarArchivos(const char* nomCuenta, const char* nomMovimientos)
{
     FILE* fCuenta = fopen(nomCuenta, "rb");

    if(!fCuenta)
    {
        puts("Error abriendo archivo.");
        return ERROR_ARCH;
    }

    tCuenta cuenta;

    printf("\n[Codigo de Cliente | Nombre Cliente | Fecha de Inicio | Tipo de Cuenta | Saldo Actual]\n");

    fread(&cuenta, sizeof(tCuenta), 1, fCuenta);
    while(!feof(fCuenta))
    {
        printf("[%05d \t|%s\t|%d-%d-%d\t\t|%c\t\t|\t%.2f]\n", cuenta.codCliente, cuenta.nomCliente, cuenta.fechaCreacion.dia, cuenta.fechaCreacion.mes, cuenta.fechaCreacion.anio, cuenta.tipoCuenta, cuenta.saldo);
        fread(&cuenta, sizeof(tCuenta), 1, fCuenta);
    }

    FILE* fMovimientos = fopen(nomMovimientos, "rb");

    if(!fMovimientos)
    {
        puts("Error abriendo archivo.");
        return ERROR_ARCH;
    }

    tMovimiento movimiento;
    fread(&movimiento, sizeof(tMovimiento), 1, fMovimientos);

    printf("\n[Codigo de Cliente | Tipo de Operacion | Monto]\n");

    while(!feof(fMovimientos))
    {
        printf("[%05d\t|\t%c\t|\t%.2f]\n", movimiento.cod, movimiento.tipoOp, movimiento.monto);
        fread(&movimiento, sizeof(tMovimiento), 1, fMovimientos);
    }

    fclose(fCuenta);
    fclose(fMovimientos);

    return TODO_OK;
}

int actualizarArchivo(const char* archivoActualizar, const char* archivoComparador)
{
     FILE* arch1 = fopen(archivoActualizar, "rb");
    if(!arch1)
    {
        puts("ERROR ABRIENDO ARCHIVO.");
        return ERROR_ARCH;
    }

    FILE* arch2 = fopen(archivoComparador, "rb");
    if(!arch2)
    {
        puts("ERROR ABRIENDO ARCHIVO.");
        return ERROR_ARCH;
    }

    char archivoActualizarTmp[TAM_LINEA];

    cambiarExtension(archivoActualizar, archivoActualizarTmp, "tmp");

    FILE* arch1Tmp = fopen(archivoActualizarTmp, "wb"); //Abro archivo temporal para carga de datos.
    if(!arch1Tmp)
    {
        puts("ERROR ABRIENDO ARCHIVO.");
        return ERROR_ARCH;
    }

    tCuenta cuenta, mov;

    int cmp; //Comparador

    fread(&cuenta, sizeof(tCuenta), 1, arch1);
    fread(&mov, sizeof(tCuenta), 1 , arch2);

    while(!feof(arch1) && !feof(arch2))
    {
        cmp= cuenta.codCliente - mov.codCliente; ///CREAR FUNCION "LEER TRANSACCION" PARA DETERMINAR SI ES DEPOSITO O EXTRACCION

        if(cmp == 0) //SIGNIFICA QUE HUBO MOVIMIENTO DE ESA CUENTA, POR LO QUE SE ACTUALIZA.
        {
            cuenta.saldo -= mov.saldo;
            fread(&mov, sizeof(tCuenta), 1, arch2);
        }

        if(cmp < 0)
        {
            fwrite(&cuenta, sizeof(tCuenta), 1, arch1Tmp);
            fread(&cuenta, sizeof(tCuenta), 1, arch1);
        }

        if(cmp > 0) ///SIGNIFICA QUE LA CUENTA NO EXISTE O HUBO ERROR EN LA BD. HABRIA QUE GUARDAR EN UN ARCHIVO ERROR.txt
        {
            acumularDuplicados(arch2, &mov, arch1Tmp);
        }
    }

    while(!feof(arch1))
    {
        fwrite(&cuenta, sizeof(tCuenta), 1 , arch1Tmp);
        fread(&cuenta, sizeof(tCuenta), 1, arch1);
    }

    fclose(arch1);
    fclose(arch2);
    fclose(arch1Tmp);

    remove(archivoActualizarTmp);
    rename(archivoActualizarTmp, archivoActualizar); //Renombro el archivo nuevo (temporal) por el viejo.

    return TODO_OK;
}

void acumularDuplicados(FILE* arch2, tCuenta* mov, FILE* arch1Tmp)
{
    tCuenta movDup;

    fread(&movDup, sizeof(tCuenta), 1, arch2);

    while(!feof(arch2) && mov->codCliente == movDup.codCliente)
    {
        mov->saldo -= movDup.saldo;
        fread(&movDup, sizeof(tCuenta), 1, arch2);
    }

    fwrite(mov, sizeof(tCuenta), 1, arch1Tmp);

    if(!feof(arch2))
        *mov = movDup;
}

void cambiarExtension(const char* nombreOrig, char* nombreDest, const char* ext)
{
    strcpy(nombreDest, nombreOrig);
    char* punto = strchr(nombreDest, '.'); //BUSCO HASTA EL CARACTER PUNTO Y GUARDO LA POSICION DE MEMORIA

    *(punto + 1) = '\0'; //REEMPLAZO EN DICHA POSICION DE MEMORIA, EL CARACTER NULO
    strcat(nombreDest, ext); //CONCATENO AMBAS CADENAS
}


int binATxtMov(const char* argumentoAConv, const char* argumentoDestino, const char argumentoTipoDestino)
{
    FILE* archBin = fopen(argumentoAConv, "rb");
    if(!archBin)
    {
        puts("Error abriendo archivo tipo binario.");
        return ERROR_ARCH;
    }

    FILE* archTxt = fopen(argumentoDestino, "wb");
    if(!archTxt)
    {
        puts("Error abriendo archivo tipo txt.");
        return ERROR_ARCH;
    }

    tMovimiento mov;
    char formato[51];
    char tipoTxt = argumentoTipoDestino;

    strcpy(formato, tipoTxt == 'v' ? "|%05d|%c|%.2f\n" : "%05%c%.2f\n");

    fread(&mov, sizeof(tMovimiento), 1, archBin);
    while(!feof(archBin))
    {
         fprintf(archTxt, formato, mov.cod, mov.tipoOp, mov.monto);
         fread(&mov, sizeof(tMovimiento), 1, archBin);
    }

    fclose(archBin);
    fclose(archTxt);


    return TODO_OK;
}

int txtABinMov(const char* argumentoAConv, const char* argumentoDestino, const char argumentoTipoDestino)
{
     FILE* archTxt = fopen(argumentoAConv,"rt");

    if(!archTxt)
    {
        puts("Error abriendo archivo.");
        return ERROR_ARCH;
    }

    FILE* archBin = fopen(argumentoDestino,"wb");

    if(!archBin)
    {
        puts("Error abriendo archivo.");
        return ERROR_ARCH;
    }

    typedef int (*TxtABin) (const char* Linea, tMovimiento* mov); //Puntero a Funcion

    tMovimiento mov;

    char Linea[TAM_LINEA];
    int codRet = TODO_OK;
    char tipoTxt = argumentoTipoDestino; //Guardo el tipo de txt en una variable.

    TxtABin txtABin = tipoTxt == 'v' ? txtVarABin : txtFijoABin; //Se le asigna la direccion de la funcion asignada por la condicion

    while(fgets(Linea, TAM_LINEA, archTxt) && codRet != ERROR_LINEA_LARGA)
    {
        codRet = txtABin(Linea, &mov);

        if(codRet == TODO_OK)
            fwrite(&mov, sizeof(tMovimiento), 1, archBin);
        ///else
        ///Generar archivos errores?
    }

    fclose(archBin);
    fclose(archTxt);

    if(codRet == ERROR_LINEA_LARGA)
        return codRet;

    return TODO_OK;
}

int txtFijoABin(const char* Linea, tMovimiento* mov)
{
    char LineaCopia[TAM_LINEA];
    strcpy(LineaCopia, Linea); //Copio la Linea original.

    char* finCampo = strchr(LineaCopia, '\n'); //Busco la direccion del enter para determinar el fin del campo.
    if(!finCampo)
        return ERROR_LINEA_LARGA;

   ///Monto
    *finCampo = '\0'; ///VALIDACION: DELIMITAR LOS CAMPOS
    finCampo -=9; //ME DESPLAZO LOS LUGARES QUE OCUPA ESE CAMPO
    sscanf(finCampo + 1, "%f", &mov->monto); //SE GUARDA EN LA DIRECCION SIGUIENTE DEL FIN DE CAMPO

    ///Tipo de Operacion
    *finCampo = '\0';
    finCampo -=1;
    sscanf(finCampo + 1, "%c", &mov->tipoOp);

    ///Codigo de cliente
    *finCampo = '\0';
    sscanf(finCampo + 1, "%05d", &mov->cod);


    return TODO_OK;
}

int txtVarABin(const char* Linea, tMovimiento* mov)
{
    char LineaCopia[TAM_LINEA];
    strcpy(LineaCopia, Linea); //Copio la Linea original.

    char* finCampo = strchr(LineaCopia, '\n'); //Busco la direccion del enter para determinar el fin del campo.
    if(!finCampo)
        return ERROR_LINEA_LARGA;

    ///Monto
    *finCampo = '\0'; ///VALIDACION: DELIMITAR LOS CAMPOS
    finCampo = strrchr(LineaCopia,'|'); //GUARDA LA DIRECCION DE LA ULTIMA APARICION DEL "|"
    sscanf(finCampo + 1, "%f", &mov->monto); //SE GUARDA EN LA DIRECCION SIGUIENTE DEL FIN DE CAMPO

    ///Tipo de Operacion
    *finCampo = '\0';
    finCampo = strrchr(LineaCopia,'|');
    sscanf(finCampo + 1, "%c", &mov->tipoOp);

    ///Codigo de cliente
    *finCampo = '\0';
    sscanf(finCampo + 1, "%05d", &mov->cod);


    return TODO_OK;
}
