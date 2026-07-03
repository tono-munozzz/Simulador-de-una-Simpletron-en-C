//Simulador simpletron en c
#include <stdio.h>

enum { MEMORIA = 1000 };   

enum CodigoOperacion {
    READ        = 10,
    WRITE       = 11,
    NEWLINE     = 12, 
    READSTRING  = 13, 
    WRITESTRING = 14, 
    LOAD        = 20,
    STORE       = 21,
    ADD         = 30,
    SUBSTRACT   = 31,
    DIVIDE      = 32,
    MULTIPLY    = 33,
    MODULO      = 34, 
    POWER       = 35, 
    BRANCH      = 40,
    BRANCHNEG   = 41,
    BRANCHZERO  = 42,
    HALT        = 43,
};
enum { VALOR_MINIMO = -99999, VALOR_MAXIMO = 99999 };

const char *ARCHIVO_PROGRAMA = "programa.simp";

int memory[MEMORIA];    
int accumulator;              
int instructionCounter;       
int operationCode;          
int operand;                 
int instructionRegister;      

void Bienvenida(void);
int  cargarPrograma(void);
int  cargarProgramaInteractivo(void);
int  cargarProgramaDesdeArchivo(FILE *archivo);
int  procesarValor(int valor, int *direccion, const char *origenTexto);
void ejecutarPrograma(void);
void vaciarMemoria(void);
void iniciarRegistros(void);

int main(void)
{
    Bienvenida();
    iniciarRegistros();

    if (cargarPrograma()) {
        printf("\n*** Se termino de cargar el programa ***\n");
        printf("*** Comienza la ejecucion del programa ***\n");
        ejecutarPrograma();
    }

    return 0;
}

void iniciarRegistros(void)
{
    int i;

    accumulator = 0;
    instructionCounter = 0;
    operationCode = 0;
    operand = 0;
    instructionRegister = 0;

    for (i = 0; i < MEMORIA; i++) {
        memory[i] = 0;
    }
}

void Bienvenida(void)
{
    printf("*** Bienvenido a Simpletron! ***\n");
    printf("*** Introduzca su programa una instruccion ***\n");
    printf("*** (o palabra de datos) a la vez en la linea ***\n");
    printf("*** de texto de entrada. Yo indicare el numero ***\n");
    printf("*** de posicion y una interrogacion (?). Usted ***\n");
    printf("*** teclera entonces la palabra para esa ***\n");
    printf("*** posicion. Escriba 9999 para terminar de ***\n");
    printf("*** introducir su programa. ***\n\n");
}

int cargarPrograma(void)
{
    FILE *archivo = fopen(ARCHIVO_PROGRAMA, "r");
    int resultado;

    if (archivo != NULL) {
        printf("*** Se encontro el archivo '%s' ***\n", ARCHIVO_PROGRAMA);
        printf("*** Cargando el programa automaticamente... ***\n");
        resultado = cargarProgramaDesdeArchivo(archivo);
        fclose(archivo);
        return resultado;
    }

    printf("*** No se encontro '%s', se usara carga interactiva ***\n", ARCHIVO_PROGRAMA);
    return cargarProgramaInteractivo();
}

int procesarValor(int valor, int *direccion, const char *origenTexto)
{
    if (valor == 9999) {
        return 0;
    }

    if (valor < VALOR_MINIMO || valor > VALOR_MAXIMO) {
        printf("*** Numero fuera de rango en %s: %d. Debe estar entre %d y +%d ***\n",
               origenTexto, valor, VALOR_MINIMO, VALOR_MAXIMO);
        return 2;
    }

    if (*direccion >= MEMORIA) {
        printf("*** Se ha llenado la memoria de la Simpletron ***\n");
        return -1;
    }

    memory[*direccion] = valor;
    (*direccion)++;

    return 1;
}

int cargarProgramaInteractivo(void)
{
    int direccion = 0;
    int valor;
    int estado;

    printf("%03d ? ", direccion);

    while (scanf("%d", &valor) == 1) {

        estado = procesarValor(valor, &direccion, "la entrada");

        if (estado == 0 || estado == -1) {
            break;
        }

        printf("%03d ? ", direccion);
    }

    return 1;
}

int cargarProgramaDesdeArchivo(FILE *archivo)
{
    int direccion = 0;
    int valor;
    int numLinea = 0;
    int estado;
    int resultado;
    char token[256];

    while ((resultado = fscanf(archivo, "%d", &valor)) != EOF) {
        numLinea++;

        if (resultado == 0) {
            if (fscanf(archivo, "%255s", token) == EOF) {
                break;
            }
            printf("*** Linea %d invalida en '%s': '%s' no es un numero entero. Se omite ***\n",
                   numLinea, ARCHIVO_PROGRAMA, token);
            continue;
        }

        estado = procesarValor(valor, &direccion, ARCHIVO_PROGRAMA);

        if (estado == 0 || estado == -1) {
            break;
        }

        if (estado == 1) {
            printf("%03d ? %d\n", direccion - 1, valor);
        }
    }

    if (direccion == 0) {
        printf("*** Aviso: no se cargo ninguna instruccion valida desde '%s' ***\n",
               ARCHIVO_PROGRAMA);
    }

    return 1;
}

void ejecutarPrograma(void)
{
    int entrada;

    while (instructionCounter >= 0 && instructionCounter < MEMORIA) {

        instructionRegister = memory[instructionCounter];

        operationCode = instructionRegister / 1000;
        operand = instructionRegister % 1000;

        switch (operationCode) {

            case READ:
                printf("Ingrese un numero entero: ");
                scanf("%d", &entrada);

                if (entrada < VALOR_MINIMO || entrada > VALOR_MAXIMO) {
                    printf("\n*** Error: el valor leido esta fuera de rango ***\n");
                    printf("*** La ejecucion de Simpletron termino anormalmente ***\n");
                    vaciarMemoria();
                    return;
                }
                memory[operand] = entrada;
                instructionCounter++;
                break;

            case WRITE:
                printf("%+06d\n", memory[operand]);
                instructionCounter++;
                break;

            case NEWLINE:
                printf("\n"); 
                instructionCounter++;
                break;

            case LOAD:
                accumulator = memory[operand];
                instructionCounter++;
                break;

            case STORE:
                memory[operand] = accumulator;
                instructionCounter++;
                break;

            case ADD:
                accumulator += memory[operand];
                if (accumulator > VALOR_MAXIMO || accumulator < VALOR_MINIMO) {
                    printf("\n*** Desbordamiento del acumulador (overflow) ***\n");
                    printf("*** La ejecucion de Simpletron termino anormalmente ***\n");
                    vaciarMemoria();
                    return;
                }
                instructionCounter++;
                break;

            case SUBSTRACT:
                accumulator -= memory[operand];
                if (accumulator > VALOR_MAXIMO || accumulator < VALOR_MINIMO) {
                    printf("\n*** Desbordamiento del acumulador (overflow) ***\n");
                    printf("*** La ejecucion de Simpletron termino anormalmente ***\n");
                    vaciarMemoria();
                    return;
                }
                instructionCounter++;
                break;

            case MULTIPLY:
                accumulator *= memory[operand];
                if (accumulator > VALOR_MAXIMO || accumulator < VALOR_MINIMO) {
                    printf("\n*** Desbordamiento del acumulador (overflow) ***\n");
                    printf("*** La ejecucion de Simpletron termino anormalmente ***\n");
                    vaciarMemoria();
                    return;
                }
                instructionCounter++;
                break;

            case DIVIDE:
                if (memory[operand] == 0) {
                    printf("\n*** Intento de dividir entre cero ***\n");
                    printf("*** La ejecucion de Simpletron termino anormalmente ***\n");
                    vaciarMemoria();
                    return;
                }
                accumulator /= memory[operand];
                instructionCounter++;
                break;

            case BRANCH:
                instructionCounter = operand;
                break;

            case BRANCHNEG:
                if (accumulator < 0) {
                    instructionCounter = operand;
                } else {
                    instructionCounter++;
                }
                break;

            case BRANCHZERO:
                if (accumulator == 0) {
                    instructionCounter = operand;
                } else {
                    instructionCounter++;
                }
                break;

            case HALT:
                printf("\n*** Termino la ejecucion de Simpletron ***\n");
                vaciarMemoria();
                return;
            
            case MODULO:
                if (memory[operand] == 0) {
                    printf("\n*** Intento de calcular modulo entre cero ***\n");
                    printf("*** La ejecucion de Simpletron termino anormalmente ***\n");
                    vaciarMemoria();
                    return;
                }
                accumulator %= memory[operand];
                instructionCounter++;
                break;

            case POWER:
            {
                int base = accumulator;
                int exponente = memory[operand];
                long long resultado_temp = 1;

                if (exponente < 0) {
                    printf("\n*** Error: Intento de elevar a potencia negativa (fraccion no soportada) ***\n");
                    printf("*** La ejecucion de Simpletron termino anormalmente ***\n");
                    vaciarMemoria();
                    return;
                }
                for (int k = 0; k < exponente; k++) {
                    resultado_temp *= base;
                    
                    if (resultado_temp > VALOR_MAXIMO || resultado_temp < VALOR_MINIMO) {
                        printf("\n*** Desbordamiento del acumulador (overflow) en potencia ***\n");
                        printf("*** La ejecucion de Simpletron termino anormalmente ***\n");
                        vaciarMemoria();
                        return;
                    }
                }
                
                accumulator = (int)resultado_temp;
                instructionCounter++;
                break;
            }
            case READSTRING:
            {
                char buffer[256];
                int longitud = 0;
                int i;

                printf("Ingrese una cadena de texto: ");
                scanf(" %255[^\n]", buffer);

                while (buffer[longitud] != '\0') {
                    longitud++;
                }

                if (longitud > 99) {
                    printf("\n*** Error: La cadena excede el limite de 99 caracteres ***\n");
                    printf("*** La ejecucion de Simpletron termino anormalmente ***\n\n");
                    vaciarMemoria();
                    return;
                }

                if (operand + longitud >= MEMORIA) {
                    printf("\n*** Error: La cadena se sale de los limites de la memoria ***\n");
                    printf("*** La ejecucion de Simpletron termino anormalmente ***\n\n");
                    vaciarMemoria();
                    return;
                }

                memory[operand] = longitud * 1000;

                for (i = 0; i < longitud; i++) {
                    int ascii_val = (int)buffer[i];
                    memory[operand + i + 1] = ((i + 1) * 1000) + ascii_val;
                }

                instructionCounter++;
                break;
            }
            case WRITESTRING:
            {
                int longitud = memory[operand] / 1000;
                int i;

                if (longitud < 0 || longitud > 99 || operand + longitud >= MEMORIA) {
                    printf("\n*** Error: Direccion invalida o longitud de cadena corrupta ***\n");
                    printf("*** La ejecucion de Simpletron termino anormalmente ***\n\n");
                    vaciarMemoria();
                    return;
                }

                for (i = 0; i < longitud; i++) {
                    int ascii_val = memory[operand + i + 1] % 1000;
                    printf("%c", (char)ascii_val);
                }
                
                instructionCounter++;
                break;
            }

            default:
                printf("\n*** Codigo de operacion no valido (%d) ***\n", operationCode);
                printf("*** La ejecucion de Simpletron termino anormalmente ***\n\n");
                vaciarMemoria();
                return;
        }
    }

    printf("\n*** Error: el contador de instrucciones se salio del rango de memoria ***\n");
    printf("*** La ejecucion de Simpletron termino anormalmente ***\n\n");
    vaciarMemoria();
}

void vaciarMemoria(void)
{
    int i;

    printf("Registros:\n");
    printf("acumulador:                 %+06d\n", accumulator);
    printf("instructionCounter:            %03d\n", instructionCounter);
    printf("instructionRegister:        %+06d\n", instructionRegister);
    printf("operationcode:                   %02d\n", operationCode);
    printf("operand:                        %03d\n\n", operand);

    printf("MEMORIA\n");
    printf("      ");
    for (i = 0; i < 10; i++) {
        printf("%6d  ", i);
    }
    printf("\n");

    for (i = 0; i < MEMORIA; i++) {
        if (i % 10 == 0) {
            printf("%3d   ", i / 10);
        }
        printf("%+06d  ", memory[i]);
        if (i % 10 == 9) {
            printf("\n");
        }
    }
}