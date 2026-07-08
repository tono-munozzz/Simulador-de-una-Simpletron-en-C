//Funcion postfija Fase II
#include <stdio.h>
#include <string.h>
#include <ctype.h>

char pila[100];
int tope = -1;


void push(char symb) {
    if (tope < 99) {
        tope++;
        pila[tope] = symb;
    }
}

char pop() {
    if (tope >= 0) {
        char temp = pila[tope];
        tope--;
        return temp;
    }
    return '\0';
}

int isEmpty() {
    return (tope == -1);
}

char stacktop() {
    if (tope >= 0) {
        return pila[tope];
    }
    return '\0';
}

int profundidad(char expresion[]) {
    int i = 0;
    char symb, temp;
    tope = -1;

    while (expresion[i] != '\0') {
        symb = expresion[i];

        if (symb == '(' || symb == '{' || symb == '[') {
            push(symb);
        }
        else if (symb == ')' || symb == '}' || symb == ']') {
            if (isEmpty()) return 0; 

            temp = pop();
            if ((symb == ')' && temp != '(') ||
                (symb == '}' && temp != '{') ||
                (symb == ']' && temp != '[')) {
                return 0; 
            }
        }
        i++;
    }

    if (!isEmpty()) return 0; 
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
    int i = 0, j = 0;
    char symb;
    tope = -1; 

    while (expresion[i] != '\0') {
        symb = expresion[i];

        if (isalnum(symb)) {
            resultado[j++] = symb;
        }
        else if (symb == '(' || symb == '{' || symb == '[') {
            push(symb);
        }
        else if (symb == ')' || symb == '}' || symb == ']') {
            while (!isEmpty()) {
                char top = stacktop();
                if ((symb == ')' && top == '(') ||
                    (symb == '}' && top == '{') ||
                    (symb == ']' && top == '[')) {
                    pop(); 
                    break;
                }
                resultado[j++] = pop();
            }
        }
        else if (esOperador(symb)) {
            while (!isEmpty() && prec(stacktop()) >= prec(symb)) {
                char top = stacktop();
                if (top == '(' || top == '{' || top == '[') {
                    break;
                }
                resultado[j++] = pop();
            }
            push(symb);
        }
        i++;
    }

    while (!isEmpty()) {
        resultado[j++] = pop();
    }
    resultado[j] = '\0';
}

int main() {
    char expresion[100];
    char resultado[100];

    printf("Ingresa la expresion: ");
    scanf("%s", expresion);

    if (profundidad(expresion)) {
        postfijo(expresion, resultado);
        printf("Expresion postfija: %s\n", resultado);
    } else {
        printf("\nERROR La expresion esta desbalanceada.\n");
        printf("Revisa las llaves.\n");
    }

    return 0;
}