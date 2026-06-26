//Simulador simpletron en c
#include <stdio.h>

enum { MEMORIA = 100 };

enum CodigoOperacion {
    READ        = 10,
    WRITE       = 11,
    LOAD        = 20,
    STORE       = 21,
    ADD         = 30,
    SUBSTRACT   = 31,
    DIVIDE      = 32,
    MULTIPLY    = 33,
    BRANCH      = 40,
    BRANCHNEG   = 41,
    BRANCHZERO  = 42,
    HALT        = 43,
};


int memory[MEMORIA];    
int accumulator;              
int instructionCounter;       
int operationCode;          
int operand;                 
int instructionRegister;      

void Bienvenida(void);
int  cargarPrograma(void);
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
    int direccion = 0;
    int valor;

    printf("%02d ? ", direccion);

    while (scanf("%d", &valor) == 1) {

        if (valor == 9999) {
            break;
        }

        if (valor < -9999 || valor > 9998) {
            printf("*** Numero fuera de rango. Debe estar entre -9999 y +9998 ***\n");
            printf("%02d ? ", direccion);
            continue;
        }

        if (direccion >= MEMORIA) {
            printf("*** Se ha llenado la memoria de la Simpletron ***\n");
            return 1;
        }

        memory[direccion] = valor;
        direccion++;
        printf("%02d ? ", direccion);
    }

    return 1;
}


void ejecutarPrograma(void)
{
    int entrada;

    while (instructionCounter >= 0 && instructionCounter < MEMORIA) {

        instructionRegister = memory[instructionCounter];

        operationCode = instructionRegister / 100;
        operand = instructionRegister % 100;

        switch (operationCode) {

            case READ:
                printf("Ingrese un numero entero: ");
                scanf("%d", &entrada);

                if (entrada < -9999 || entrada > 9999) {
                    printf("\n*** Error: el valor leido esta fuera de rango ***\n");
                    printf("*** La ejecucion de Simpletron termino anormalmente ***\n\n");
                    vaciarMemoria();
                    return;
                }
                memory[operand] = entrada;
                instructionCounter++;
                break;

            case WRITE:
                printf("%+05d\n", memory[operand]);
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
                if (accumulator > 9999 || accumulator < -9999) {
                    printf("\n*** Desbordamiento del acumulador (overflow) ***\n");
                    printf("*** La ejecucion de Simpletron termino anormalmente ***\n\n");
                    vaciarMemoria();
                    return;
                }
                instructionCounter++;
                break;

            case SUBSTRACT:
                accumulator -= memory[operand];
                if (accumulator > 9999 || accumulator < -9999) {
                    printf("\n*** Desbordamiento del acumulador (overflow) ***\n");
                    printf("*** La ejecucion de Simpletron termino anormalmente ***\n\n");
                    vaciarMemoria();
                    return;
                }
                instructionCounter++;
                break;

            case MULTIPLY:
                accumulator *= memory[operand];
                if (accumulator > 9999 || accumulator < -9999) {
                    printf("\n*** Desbordamiento del acumulador (overflow) ***\n");
                    printf("*** La ejecucion de Simpletron termino anormalmente ***\n\n");
                    vaciarMemoria();
                    return;
                }
                instructionCounter++;
                break;

            case DIVIDE:
                if (memory[operand] == 0) {
                    printf("\n*** Intento de dividir entre cero ***\n");
                    printf("*** La ejecucion de Simpletron termino anormalmente ***\n\n");
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
                printf("\n*** Termino la ejecucion de Simpletron ***\n\n");
                vaciarMemoria();
                return;

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
    printf("acumulador:           %+05d\n", accumulator);
    printf("instructionCounter:        %02d\n", instructionCounter);
    printf("instructionRegister:  %+05d\n", instructionRegister);
    printf("operationcode:              %02d\n", operationCode);
    printf("operand:                    %02d\n\n", operand);

    printf("MEMORIA\n");
    printf("     ");
    for (i = 0; i < 10; i++) {
        printf("%5d  ", i);
    }
    printf("\n");

    for (i = 0; i < MEMORIA; i++) {
        if (i % 10 == 0) {
            printf("%2d   ", i / 10);
        }
        printf("%+05d  ", memory[i]);
        if (i % 10 == 9) {
            printf("\n");
        }
    }
}
