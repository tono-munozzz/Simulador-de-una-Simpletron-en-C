//Evaluacion completa de expresiones
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

union Dato {
    char c;
    float f;
};

struct Pila {
    union Dato datos[100];
    int tope;
};

void inicializar(struct Pila *p) {
    p->tope = -1;
}

void push(struct Pila *p, union Dato valor) {
    if (p->tope < 99) {
        p->tope++;
        p->datos[p->tope] = valor;
    }
}

union Dato pop(struct Pila *p) {
    union Dato valor;
    if (p->tope >= 0) {
        valor = p->datos[p->tope];
        p->tope--;
    } else {
        valor.c = '\0';
    }
    return valor;
}

int isEmpty(struct Pila *p) {
    return (p->tope == -1);
}

char stacktop(struct Pila *p) {
    if (p->tope >= 0) {
        return p->datos[p->tope].c;
    }
    return '\0';
}

int profundidad(char expresion[]) {
    struct Pila p;
    inicializar(&p);
    union Dato dato, temp;
    int i = 0;
    char symb;

    while (expresion[i] != '\0') {
        symb = expresion[i];

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
        i++;
    }

    if (!isEmpty(&p)) return 0;
    return 1;
}

int prec(char op) {
    if (op == '^') return 3;
    if (op == '*' || op == '/' || op == '%') return 2;
    if (op == '+' || op == '-') return 1;
    return 0;
}

int esOperador(char c) {
    return (c == '+' || c == '-' || c == '*' || c == '/' || c == '^' || c == '%');
}

void postfijo(char expresion[], char resultado[]) {
    struct Pila p;
    inicializar(&p);
    union Dato dato, temp;
    int i = 0, j = 0;
    char symb;

    while (expresion[i] != '\0') {
        symb = expresion[i];

        if (isalnum(symb)) {
            resultado[j++] = symb;
        }
        else if (symb == '(' || symb == '{' || symb == '[') {
            dato.c = symb;
            push(&p, dato);
        }
        else if (symb == ')' || symb == '}' || symb == ']') {
            while (!isEmpty(&p)) {
                char top = stacktop(&p);
                if ((symb == ')' && top == '(') ||
                    (symb == '}' && top == '{') ||
                    (symb == ']' && top == '[')) {
                    pop(&p);
                    break;
                }
                temp = pop(&p);
                resultado[j++] = temp.c;
            }
        }
        else if (esOperador(symb)) {
            while (!isEmpty(&p) && prec(stacktop(&p)) >= prec(symb)) {
                char top = stacktop(&p);
                if (top == '(' || top == '{' || top == '[') {
                    break;
                }
                temp = pop(&p);
                resultado[j++] = temp.c;
            }
            dato.c = symb;
            push(&p, dato);
        }
        i++;
    }

    while (!isEmpty(&p)) {
        temp = pop(&p);
        resultado[j++] = temp.c;
    }
    resultado[j] = '\0';
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
    struct Pila p;
    inicializar(&p);
    union Dato dato, temp;
    char symb;
    int i = 0;
    float opnd1, opnd2, value;

    while (expresion[i] != '\0') {
        symb = expresion[i];

        if (symb >= '0' && symb <= '9') {
            dato.f = convierte(symb);
            push(&p, dato);
        } else if (symb == '+' || symb == '-' || symb == '*' || symb == '/' || symb == '^') {
            temp = pop(&p);
            opnd2 = temp.f;
            temp = pop(&p);
            opnd1 = temp.f;
            value = evalua(opnd1, symb, opnd2);
            dato.f = value;
            push(&p, dato);
        }

        i++;
    }

    temp = pop(&p);
    return temp.f;
}

int main() {
    char expresion[100];
    char resultado[100];

    printf("Ingresa la expresion: ");
    scanf("%s", expresion);

    if (!profundidad(expresion)) {
        printf("\nERROR La expresion esta desbalanceada.\n");
        return 0;
    }

    printf("La expresion esta balanceada\n");
    postfijo(expresion, resultado);
    printf("Expresion postfija: %s\n", resultado);
    float resultadoFinal = evaluar(resultado);
    printf("El resultado es: %.2f\n", resultadoFinal);

    return 0;
}