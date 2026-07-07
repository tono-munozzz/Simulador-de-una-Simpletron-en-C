//Evaluacion de expresiones postfijas
#include <stdio.h>
#include <math.h>

typedef struct {
    float datos[100];
    int tope;
} Pila;

void inicializar(Pila *p) {
    p->tope = -1;
}

void push(Pila *p, float valor) {
    p->tope++;
    p->datos[p->tope] = valor;
}

float pop(Pila *p) {
    float valor = p->datos[p->tope];
    p->tope--;
    return valor;
}

float convierte(char numero) {
    float valor;
    valor = (float)(numero - '0');
    return valor;
}

float evalua(float opnd1, char sign, float opnd2) {
    float value = 0;

    if (sign == '+') {
        value = opnd1 + opnd2;
    } else if (sign == '-') {
        value = opnd1 - opnd2;
    } else if (sign == '*') {
        value = opnd1 * opnd2;
    } else if (sign == '/') {
        value = opnd1 / opnd2;
    } else if (sign == '^') {
        value = pow(opnd1, opnd2);
    }

    return value;
}

float evaluar(char expresion[]) {
    Pila p;
    inicializar(&p);

    char symb;
    int i = 0;
    float opnd1, opnd2, value;

    while (expresion[i] != '\0') {
        symb = expresion[i];

        if (symb >= '0' && symb <= '9') {
            push(&p, convierte(symb));
        } else if (symb == '+' || symb == '-' || symb == '*' || symb == '/' || symb == '^') {
            opnd2 = pop(&p);
            opnd1 = pop(&p);
            value = evalua(opnd1, symb, opnd2);
            push(&p, value);
        }

        i++;
    }

    return pop(&p);
}

int main() {
    char expresion[100];

    printf("Ingresa la expresion: ");
    scanf("%s", expresion);

    float resultado = evaluar(expresion);

    printf("El resultado es: %.2f\n", resultado);

    return 0;
}