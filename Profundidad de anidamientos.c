//Profundiad de anidamineto
#include <stdio.h>
#include <string.h>

char pila[100];
int tope = -1;

void push(char symb) {
    tope++;
    pila[tope] = symb;
}

char pop() {
    char temp = pila[tope];
    tope--;
    return temp;
}

int isEmpty() {
    return (tope == -1);
}

int profundidad(char expresion[]) {
    char symb, temp;
    int i = 0;
    int valido = 1;

    while (expresion[i] != '\0') {
        symb = expresion[i];

        if (symb == '(' || symb == '{' || symb == '[') {
            push(symb);
        }
        else if (symb == ')' || symb == '}' || symb == ']') {
            if (isEmpty()) {
                return 0;
            }
            else {
                temp = pop();
                if (symb == ')') {
                    if (temp != '(') {
                        return 0;
                    }
                }
                else if (symb == '}') {
                    if (temp != '{') {
                        return 0;
                    }
                }
                else if (symb == ']') {
                    if (temp != '[') {
                        return 0;
                    }
                }
            }
        }
        i++;
    }

    if (!isEmpty()) {
        return 0;
    }

    return 1;
}

int main() {
    char expresion[100];

    printf("Ingrese la expresion: ");
    scanf("%s", expresion);

    tope = -1; 
    if (profundidad(expresion)) {
        printf("La expresion esta balanceada\n");
    } else {
        printf("La expresion esta desbalanceada\n");
    }

    return 0;
}
