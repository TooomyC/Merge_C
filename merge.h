#ifndef MERGE_H_INCLUDED
#define MERGE_H_INCLUDED

#define TAM_NOM_CLIENTE 71

typedef struct{
  int dia,
      mes,
      anio;
}Fecha;

typedef struct
{
    int codCliente;
    char nomCliente[TAM_NOM_CLIENTE];
    Fecha fechaCreacion;
    char tipoCuenta; ///Caja de ahorro 'a' o Cuenta corriente 'c'
    float saldo;

} tCuenta;

typedef struct
{
    int cod; ///Codigo de cliente
    char tipoOp; ///Extraccion o deposito
    float monto;

} tMovimiento;



int generarArchivo(const char* argumento,  size_t estructura, const void* contenido, int cantElem);
int mostrarArchivos(const char* nomCuenta, const char* nomMovimientos);

//Prototipos de Conversion
int binATxtMov(const char* argumentoAConv, const char* argumentoDestino, const char argumentoTipoDestino);
int txtABinMov(const char* argumentoAConv, const char* argumentoDestino, const char argumentoTipoDestino);
int txtVarABin(const char* Linea, tMovimiento* mov);
int txtFijoABin(const char* Linea, tMovimiento* mov);

//Merge
int actualizarArchivo(const char* archivoActualizar, const char* archivoComparador);
void cambiarExtension(const char* nombreOrig, char* nombreDest, const char* ext);
void acumularDuplicados(FILE* arch2, tCuenta* mov, FILE* arch1Tmp);


#endif // MERGE_H_INCLUDED
