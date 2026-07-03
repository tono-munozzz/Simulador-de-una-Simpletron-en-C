//Función Postfijo Fase I
#include <stdio.h>
#include <string.h>
#include <ctype.h>

char p[100];
int topeIdx = -1;             

int isEmpty() {
    return topeIdx == -1;
}

void push(char c) {
    p[++topeIdx] = c;
}

char pop() {
    return p[topeIdx--];
}

char stacktop() {
    return p[topeIdx];
}


int esOperando(char c) {
    return isalnum(c);          
}

int prec(char op1, char op2) {
    int jer1 = 0, jer2 = 0;

    if (op1 == '^')
        jer1 = 3;
    else if (op1 == '*' || op1 == '/')
        jer1 = 2;
    else if (op1 == '+' || op1 == '-')
        jer1 = 1;

    if (op2 == '^')
        jer2 = 3;
    else if (op2 == '*' || op2 == '/')
        jer2 = 2;
    else if (op2 == '+' || op2 == '-')
        jer2 = 1;

    if (jer1 >= jer2)
        return 1;   
    else
        return 0;   
}

void postfijo(char expresion[], char resultado[]) {
    char symb, topSymb;
    int i = 0, j = 0;

    topeIdx = -1;   

    while (expresion[i] != '\0') {
        symb = expresion[i];
        i++;

        if (esOperando(symb)) {
            resultado[j++] = symb;
        }
        else {
            while (!isEmpty() && prec(stacktop(), symb)) {
                topSymb = pop();
                resultado[j++] = topSymb;
            }
            push(symb);
        }
    }

    while (!isEmpty()) {
        topSymb = pop();
        resultado[j++] = topSymb;
    }

    resultado[j] = '\0';
}

int main() {
    char expresion[100];
    char resultado[100];

    printf("Ingresa la expresion: ");
    scanf("%s", expresion);

    postfijo(expresion, resultado);

    printf("Expresion postfija: %s\n", resultado);

    return 0;
}