//Compilador SML
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

enum { MEMORIA = 100 }; 

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

enum { VALOR_MINIMO = -9999, VALOR_MAXIMO = 9999 }; 
enum { MAX_LINEA = 256, MAX_TOKENS = 32 };

const char *ARCHIVO_FUENTE_SIMPLE = "programa.simple";
const char *ARCHIVO_SALIDA_SML    = "programa.sml";   

struct tableEntry {
    int  symbol;        
    char name[32];      
    char type;          
    int  location;      
    struct tableEntry *next;
};

struct tableEntry *symbolTable = NULL;

union Dato {
    char c;
    int  ubicacion;
};

struct Pila {
    union Dato datos[MAX_TOKENS];
    int tope;
};

int sml[MEMORIA];              
int flags[MEMORIA];            
int contadorInstrucciones = 0; 
int punteroDatos = MEMORIA - 1;
int contadorTemporales = -1;   
int erroresDetectados = 0;
int lineaFuenteActual = 0;     

/* Declaraciones (prototipos) para evitar declaraciones implícitas */
void BienvenidaCompilador(void);
void iniciarRegistrosCompilador(void);
struct tableEntry *buscarSimbolo(int symbol, const char *name, char type);
struct tableEntry *insertarSimbolo(int symbol, const char *name, char type, int location);
void liberarTablaSimbolos(void);
void errorCompilacion(const char *mensaje);
void verificarMemoriaDisponible(void);
int obtenerUbicacion(const char *token);
int nuevaTemporal(void);
void emitir(int codigoOperacion, int operando);
void emitirSalto(int codigoOperacion, int numeroLineaDestino);
void inicializarPila(struct Pila *p);
void push(struct Pila *p, union Dato valor);
union Dato pop(struct Pila *p);
int isEmpty(struct Pila *p);
char stacktop(struct Pila *p);
int esOperador(char c);
int prec(char op);
int expresionBalanceada(char tokensInfijos[][16], int nTokens);
int infijaAPosfija(char tokensInfijos[][16], int nTokens, char tokensPosfijos[][16]);
int evaluarPosfija(char tokensPosfijos[][16], int nTokens);
void compilarLet(void);
void compilarIf(void);
void compilarLinea(char *linea);
int compilarPrograma(void);
int compilarDesdeArchivo(FILE *archivo);
int compilarDesdeTeclado(void);
void segundaPasada(void);
int exportarArchivoSML(void);
void imprimirTablaSimbolos(void);
void imprimirProgramaSML(void);

int main(void)
{
    BienvenidaCompilador();
    iniciarRegistrosCompilador();

    if (compilarPrograma()) {
        printf("\n*** Primera Pasada ***\n");
        printf("*** Segunda Pasada ***\n");
        segundaPasada();

        if (erroresDetectados == 0) {
            if (exportarArchivoSML()) {
                imprimirTablaSimbolos();
                imprimirProgramaSML();
                printf("\n*** Programa SML guardado en '%s' ***\n",
                       ARCHIVO_SALIDA_SML);
            }
        } else {
            printf("\n%d errores detectados ***\n",
                   erroresDetectados);
        }
    }

    liberarTablaSimbolos();
    return 0;
}

void BienvenidaCompilador(void)
{
    printf("*** Bienvenido al Compilador ***\n");
    printf("*** Cada enunciado debe iniciar con un numero de linea. ***\n");
}

void iniciarRegistrosCompilador(void)
{
    int i;

    for (i = 0; i < MEMORIA; i++) {
        sml[i]   = 0;
        flags[i] = -1;
    }
}

struct tableEntry *buscarSimbolo(int symbol, const char *name, char type)
{
    struct tableEntry *p = symbolTable;

    while (p != NULL) {
        if (p->type == type) {
            if (type == 'V') {
                if (name != NULL && strcmp(p->name, name) == 0) return p;
            } else {
                if (p->symbol == symbol) return p;
            }
        }
        p = p->next;
    }
    return NULL;
}

struct tableEntry *insertarSimbolo(int symbol, const char *name, char type, int location)
{
    struct tableEntry *nuevo = (struct tableEntry *) malloc(sizeof(struct tableEntry));

    if (nuevo == NULL) {
        printf("*** Error fatal: memoria insuficiente para la tabla de simbolos ***\n");
        exit(EXIT_FAILURE);
    }

    nuevo->symbol = symbol;
    if (name != NULL) {
        strncpy(nuevo->name, name, 31);
        nuevo->name[31] = '\0';
    } else {
        nuevo->name[0] = '\0';
    }
    
    nuevo->type     = type;
    nuevo->location = location;
    nuevo->next     = symbolTable;
    symbolTable = nuevo;

    return nuevo;
}

void liberarTablaSimbolos(void)
{
    struct tableEntry *p = symbolTable;
    struct tableEntry *siguiente;

    while (p != NULL) {
        siguiente = p->next;
        free(p);
        p = siguiente;
    }
    symbolTable = NULL;
}

void errorCompilacion(const char *mensaje)
{
    printf("*** Error en linea %d: %s ***\n", lineaFuenteActual, mensaje);
    erroresDetectados++;
}

void verificarMemoriaDisponible(void)
{
    if (contadorInstrucciones > punteroDatos) {
        printf("*** Error fatal: la memoria de la Simpletron se ha llenado ***\n");
        printf("*** (el segmento de instrucciones choco con el de datos) ***\n");
        exit(EXIT_FAILURE);
    }
}

int obtenerUbicacion(const char *token)
{
    int  esConstante;
    char tipo;
    int  simbolo = 0;
    struct tableEntry *entrada; 
    
    int  location;

    esConstante = isdigit((unsigned char) token[0]) ||
                  (token[0] == '-' && isdigit((unsigned char) token[1]));
    tipo = esConstante ? 'C' : 'V';

    if (esConstante) {
        simbolo = atoi(token);
        entrada = buscarSimbolo(simbolo, NULL, tipo);
    } else {
        entrada = buscarSimbolo(0, token, tipo); 
    }
    if (entrada != NULL) {
        return entrada->location;
    }

    if (esConstante && (simbolo < VALOR_MINIMO || simbolo > VALOR_MAXIMO)) {
        errorCompilacion("constante fuera del rango permitido por la Simpletron");
    }
    if (esConstante && simbolo == 9999) {
        errorCompilacion("la constante 9999 no se puede usar: el simulador la interpreta como marca de fin");
    }

    location = punteroDatos--;
    verificarMemoriaDisponible();

    if (esConstante) {
        insertarSimbolo(simbolo, NULL, tipo, location);
        sml[location] = simbolo;
    } else {
        insertarSimbolo(0, token, tipo, location);
    }

    return location;
}

int nuevaTemporal(void)
{
    int location = punteroDatos--;

    verificarMemoriaDisponible();
    insertarSimbolo(contadorTemporales--, NULL, 'T', location);

    return location;
}

void emitir(int codigoOperacion, int operando)
{
    // Operación clásica de 4 dígitos (* 100)
    sml[contadorInstrucciones] = codigoOperacion * 100 + operando; 
    contadorInstrucciones++;
    verificarMemoriaDisponible();
}

void emitirSalto(int codigoOperacion, int numeroLineaDestino)
{
    int loc = contadorInstrucciones;
    struct tableEntry *destino = buscarSimbolo(numeroLineaDestino, NULL, 'L');

    if (destino != NULL) {
        emitir(codigoOperacion, destino->location);
    } else {
        emitir(codigoOperacion, 0);
        flags[loc] = numeroLineaDestino;
    }
}

void inicializarPila(struct Pila *p)
{
    p->tope = -1;
}

void push(struct Pila *p, union Dato valor)
{
    if (p->tope < MAX_TOKENS - 1) {
        p->tope++;
        p->datos[p->tope] = valor;
    }
}

union Dato pop(struct Pila *p)
{
    union Dato valor;

    if (p->tope >= 0) {
        valor = p->datos[p->tope];
        p->tope--;
    } else {
        valor.c = '\0';
    }
    return valor;
}

int isEmpty(struct Pila *p)
{
    return (p->tope == -1);
}

char stacktop(struct Pila *p)
{
    if (p->tope >= 0) {
        return p->datos[p->tope].c;
    }
    return '\0';
}

int esOperador(char c)
{
    return (c == '+' || c == '-' || c == '*' || c == '/' || c == '%' || c == '^');
}

int prec(char op)
{
    if (op == '^') return 3;
    if (op == '*' || op == '/' || op == '%') return 2;
    if (op == '+' || op == '-') return 1;
    return 0;
}

int expresionBalanceada(char tokensInfijos[][16], int nTokens)
{
    struct Pila p;
    union Dato dato, temp;
    int i;
    char symb;

    inicializarPila(&p);

    for (i = 0; i < nTokens; i++) {
        symb = tokensInfijos[i][0];

        if (symb == '(' || symb == '{' || symb == '[') {
            dato.c = symb;
            push(&p, dato);
        }
        else if (symb == ')' || symb == '}' || symb == ']') {
            if (isEmpty(&p)) return 0;

            temp = pop(&p);
            if ((symb == ')' && temp.c != '(') ||
                (symb == '}' && temp.c != '{') ||
                (symb == ']' && temp.c != '[')) {
                return 0;
            }
        }
    }

    return isEmpty(&p);
}

int infijaAPosfija(char tokensInfijos[][16], int nTokens, char tokensPosfijos[][16])
{
    struct Pila p;
    union Dato dato, temp;
    int  nPosfijos = 0;
    int  i;
    char *token;

    inicializarPila(&p);

    for (i = 0; i < nTokens; i++) {
        token = tokensInfijos[i];

        if (strlen(token) == 1 && esOperador(token[0])) {
            while (!isEmpty(&p) && stacktop(&p) != '(' &&
                   prec(stacktop(&p)) >= prec(token[0])) {
                temp = pop(&p);
                tokensPosfijos[nPosfijos][0] = temp.c;
                tokensPosfijos[nPosfijos][1] = '\0';
                nPosfijos++;
            }
            dato.c = token[0];
            push(&p, dato);
        }
        else if (strcmp(token, "(") == 0) {
            dato.c = '(';
            push(&p, dato);
        }
        else if (strcmp(token, ")") == 0) {
            while (!isEmpty(&p) && stacktop(&p) != '(') {
                temp = pop(&p);
                tokensPosfijos[nPosfijos][0] = temp.c;
                tokensPosfijos[nPosfijos][1] = '\0';
                nPosfijos++;
            }
            if (!isEmpty(&p)) pop(&p);   
        }
        else {
            strcpy(tokensPosfijos[nPosfijos], token);
            nPosfijos++;
        }
    }
    while (!isEmpty(&p)) {
        temp = pop(&p);
        tokensPosfijos[nPosfijos][0] = temp.c;
        tokensPosfijos[nPosfijos][1] = '\0';
        nPosfijos++;
    }

    return nPosfijos;
}

int evaluarPosfija(char tokensPosfijos[][16], int nTokens)
{
    struct Pila p;
    union Dato dato, temp;
    int  i, ubA, ubB, tempLoc;
    char *token;

    inicializarPila(&p);

    for (i = 0; i < nTokens; i++) {
        token = tokensPosfijos[i];

        if (strlen(token) == 1 && esOperador(token[0])) {
            temp = pop(&p);
            ubB = temp.ubicacion;      
            temp = pop(&p);
            ubA = temp.ubicacion;      

            emitir(LOAD, ubA);
            switch (token[0]) {
                case '+': emitir(ADD,       ubB); break;
                case '-': emitir(SUBSTRACT, ubB); break;
                case '*': emitir(MULTIPLY,  ubB); break;
                case '/': emitir(DIVIDE,    ubB); break;
                case '%': emitir(MODULO,    ubB); break;
                case '^': emitir(POWER,     ubB); break;
            }
            tempLoc = nuevaTemporal();
            emitir(STORE, tempLoc);

            dato.ubicacion = tempLoc;
            push(&p, dato);
        } else {
            dato.ubicacion = obtenerUbicacion(token);
            push(&p, dato);
        }
    }

    temp = pop(&p);
    return temp.ubicacion;
}

void compilarLet(void)
{
    char *varDestino;
    char *igual;
    char tokensInfijos[MAX_TOKENS][16];
    char tokensPosfijos[MAX_TOKENS][16];
    int  n = 0;
    int  nPos;
    int  ubicacionResultado;
    int  ubicacionDestino;
    char *token;

    varDestino = strtok(NULL, " \t\r\n");
    igual      = strtok(NULL, " \t\r\n");

    if (varDestino == NULL || igual == NULL || strcmp(igual, "=") != 0) {
        errorCompilacion("Invalida en 'let' ");
        return;
    }

    while ((token = strtok(NULL, " \t\r\n")) != NULL) {
        strncpy(tokensInfijos[n], token, 15);
        tokensInfijos[n][15] = '\0';
        n++;
    }
    if (n == 0) {
        errorCompilacion("expresion vacia en 'let'");
        return;
    }

    if (!expresionBalanceada(tokensInfijos, n)) {
        errorCompilacion("la expresion está desbalanceados");
        return;
    }

    nPos = infijaAPosfija(tokensInfijos, n, tokensPosfijos);
    ubicacionResultado = evaluarPosfija(tokensPosfijos, nPos);
    ubicacionDestino = obtenerUbicacion(varDestino);

    emitir(LOAD,  ubicacionResultado);
    emitir(STORE, ubicacionDestino);
}

void compilarIf(void)
{
    char *op1, *relop, *op2, *gotoKw, *lineaStr;
    int destino, ubA, ubB;

    op1      = strtok(NULL, " \t\r\n");
    relop    = strtok(NULL, " \t\r\n");
    op2      = strtok(NULL, " \t\r\n");
    gotoKw   = strtok(NULL, " \t\r\n");
    lineaStr = strtok(NULL, " \t\r\n");

    if (!op1 || !relop || !op2 || !gotoKw || !lineaStr || strcmp(gotoKw, "goto") != 0) {
        errorCompilacion("Invalida en 'if'");
        return;
    }

    destino = atoi(lineaStr);
    ubA = obtenerUbicacion(op1);
    ubB = obtenerUbicacion(op2);

    emitir(LOAD, ubA);
    emitir(SUBSTRACT, ubB);      

    if (strcmp(relop, "==") == 0) {
        emitirSalto(BRANCHZERO, destino);
    }
    else if (strcmp(relop, "!=") == 0) {
        emitir(BRANCHZERO, contadorInstrucciones + 2);   
        emitirSalto(BRANCH, destino);
    }
    else if (strcmp(relop, ">") == 0) {
        emitir(BRANCHNEG,  contadorInstrucciones + 3);   
        emitir(BRANCHZERO, contadorInstrucciones + 2);   
        emitirSalto(BRANCH, destino);
    }
    else if (strcmp(relop, "<") == 0) {
        emitirSalto(BRANCHNEG, destino);
    }
    else if (strcmp(relop, ">=") == 0) {
        emitir(BRANCHNEG, contadorInstrucciones + 2);    
        emitirSalto(BRANCH, destino);
    }
    else if (strcmp(relop, "<=") == 0) {
        emitirSalto(BRANCHNEG,  destino);                
        emitirSalto(BRANCHZERO, destino);                
    }
    else {
        errorCompilacion("operador relacional no encontrado");
    }
}

void compilarLinea(char *linea)
{
    char *token;
    int numeroLinea;
    char *comando;
    char *var;
    char *destStr;

    token = strtok(linea, " \t\r\n");
    if (token == NULL || token[0] == '\0') {
        return;   
    }

    lineaFuenteActual = atoi(token);
    numeroLinea = lineaFuenteActual;

    if (buscarSimbolo(numeroLinea, NULL, 'L') != NULL) {
        errorCompilacion("numero de linea duplicado");
        return;
    }
    insertarSimbolo(numeroLinea, NULL, 'L', contadorInstrucciones);

    comando = strtok(NULL, " \t\r\n");
    if (comando == NULL) {
        errorCompilacion("falta la instruccion despues del numero de linea");
        return;
    }

    if (strcmp(comando, "rem") == 0) {
    }
    else if (strcmp(comando, "input") == 0) {
        var = strtok(NULL, " \t\r\n");
        if (!var) { errorCompilacion("'input' requiere una variable"); return; }
        emitir(READ, obtenerUbicacion(var));
    }
    else if (strcmp(comando, "print") == 0) {
        var = strtok(NULL, " \t\r\n");
        if (!var) { errorCompilacion("'print' requiere una variable"); return; }
        emitir(WRITE, obtenerUbicacion(var));
    }
    else if (strcmp(comando, "newline") == 0) {
        emitir(NEWLINE, 0);
    }
    else if (strcmp(comando, "goto") == 0) {
        destStr = strtok(NULL, " \t\r\n");
        if (!destStr) { errorCompilacion("'goto' requiere numero de linea"); return; }
        emitirSalto(BRANCH, atoi(destStr));
    }
    else if (strcmp(comando, "let") == 0) {
        compilarLet();
    }
    else if (strcmp(comando, "if") == 0) {
        compilarIf();
    }
    else if (strcmp(comando, "end") == 0) {
        emitir(HALT, 0);
    }
    else {
        errorCompilacion("instruccion desconocida");
    }
}

int compilarPrograma(void)
{
    FILE *archivo = fopen(ARCHIVO_FUENTE_SIMPLE, "r");
    int resultado;

    if (archivo != NULL) {
        printf("*** Se encontro el archivo '%s' ***\n", ARCHIVO_FUENTE_SIMPLE);
        resultado = compilarDesdeArchivo(archivo);
        fclose(archivo);
        return resultado;
    }

    printf("*** No se encontro '%s', se usara captura interactiva ***\n",
           ARCHIVO_FUENTE_SIMPLE);
    return compilarDesdeTeclado();
}

int compilarDesdeArchivo(FILE *archivo)
{
    char linea[MAX_LINEA];

    while (fgets(linea, sizeof(linea), archivo) != NULL) {
        compilarLinea(linea);
    }
    return 1;
}

int compilarDesdeTeclado(void)
{
    char linea[MAX_LINEA];

    printf("*** Introduzca su programa en Simple, una linea a la vez. ***\n");
    printf("*** Escriba 'fin' para terminar de introducir su programa. ***\n\n");

    printf("> ");
    while (fgets(linea, sizeof(linea), stdin) != NULL) {
        if (strncmp(linea, "fin", 3) == 0) {
            break;
        }
        compilarLinea(linea);
        printf("> ");
    }
    return 1;
}

void segundaPasada(void)
{
    int i;
    struct tableEntry *destino;

    for (i = 0; i < MEMORIA; i++) {
        if (flags[i] == -1) continue;

        destino = buscarSimbolo(flags[i], NULL, 'L');
        if (destino == NULL) {
            // Imprime índice de memoria a 2 dígitos
            printf("*** Error: numero de linea %d ***\n", flags[i], i); 
            erroresDetectados++;
            continue;
        }
        sml[i] += destino->location;   
    }
}

int exportarArchivoSML(void)
{
    FILE *f = fopen(ARCHIVO_SALIDA_SML, "w");
    int i;

    if (!f) {
        printf("*** No se pudo crear el archivo de salida '%s' ***\n", ARCHIVO_SALIDA_SML);
        return 0;
    }
    for (i = 0; i < MEMORIA; i++) {
        fprintf(f, "%d\n", sml[i]);
    }
    fclose(f);
    return 1;
}

void imprimirTablaSimbolos(void)
{
    struct tableEntry *p = symbolTable;

    printf("\nTabla de simbolos:\n");
    printf("%-15s %-6s %-10s\n", "simbolo/nombre", "tipo", "ubicacion");
    printf("-----------------------------------\n");

    while (p != NULL) {
        if (p->type == 'V') {
            printf("%-15s %-6c %-10d\n", p->name, p->type, p->location);
        } else {
            printf("%-15d %-6c %-10d\n", p->symbol, p->type, p->location);
        }
        p = p->next;
    }
}

void imprimirProgramaSML(void)
{
    int i;
    printf("\nPrograma SML generado (instrucciones 00-%02d):\n",
           contadorInstrucciones - 1);
    for (i = 0; i < contadorInstrucciones; i++) {
        printf("%02d  %+05d\n", i, sml[i]); 
    }

    printf("\nZona de datos (%02d-99):\n", punteroDatos + 1);
    for (i = punteroDatos + 1; i < MEMORIA; i++) {
        printf("%02d  %+05d\n", i, sml[i]);
    }
}