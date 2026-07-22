# **Proyecto final: Simpletron**

En este proyecto que se realizo durante el ciclo de VERANO 2026, se desarrollo una computadora virtual Simpletron en C

La cual incluye:
1. Lenguaje de máquina SML
2. Extensiones al simulador
3. Herramientas de evaluación de expresiones
4. Compilador que traduzca programas Simple a SML

El proyecto consto de 8 fases, en las que se fueron desarrollando las funciones de la computadora virtual, primero se realizo un simluador basico de simpletron con una memoria maxima de 100 elementos, está simpletron maneja un formato de palabras de 4 digitos con un limite de validación entre -9999 a +9998.
Después se le realizaron mejoras a la simpletron como lo son la carga de programas desde archivo "programa.simp", se le expandio la memoria a 1000 elementos, al igual se implemento el manejo de residuos y divisiones entre 0 como tambien el calculo de potencias (A^B), tambien en los caracteres se le agrego un salto de linea que la simpletron basica no maneja y por ultimo se agregaron las entreadas y salidas de cadenas (almacenamiento ASCII).

Al terminar con el desarrollo de la Simpletron se continuo con las funciones que serian utilizadas para el compilador que traduciera Simple a SML.
Primero se realizo la Funcion de Profundidad de anidamientos para verficar que todas las llaves tengan su apertura y su cierre, despues se realizo la fase I de funciones postfijas que es para la conversion de expresiones aritmeticas a postfijas, despues en la fase II se juntaron la profundidad de anidamientos y la fase I, continuamos con la funcion de evaluacion de expresiones postfijas y al final de todas estas se intregaron en un solo codigo para despues poder continuar con el desarrollo completo del compilador.

### **Instriccones de compilación**
El proyecto son archivos en C estándar, por lo que no requiere librerías externas. No es necesario enlazar librerías adicionales (-lm, -lpthread, etc.); solo se usa la biblioteca estándar de C (stdio.h, stdlib.h, string.h, ctype.h, etc.).

### **Instrucciones de ejecucióm**
El compilador funciona de dos formas, dependiendo de si existe el archivo de entrada programa.simple en el mismo directorio que el ejecutable, en el caso de que no se encuentre usted mismo tendra que poner el codigo linea por linea.

1. Modo Archivo
   1. Cargue el archivo `programa.simple` en el mismo directorio
   2. Ejecute el programa
   3. Si la compilación es exitosa en el mismo directorio le dara la traduccion a SML con el nombre de archivo `prgrama.sml`

2. Modo Manual
   1. Si no se encuentra el archivo `prgrama.simple` se ejecutara el programa.
   2. Escriba el codigo linea por linea.
   3. Finalice escribiendo 'fin' al final del codigo
   4. Se hara la compilación y se cargara en el mismo repositorio el `programa.sml`

### **Ejemplos de entrada y salida**

**Entrada** 
```
10 rem Programa media de 3
20 input a
21 input b
22 input c
30 let k = a + b + c
31 let r = k / 3
40 print r
50 end
```

**Salida**
```
*** Primera Pasada ***
*** Segunda Pasada ***

Tabla de simbolos:
simbolo/nombre  tipo   ubicacion 
-----------------------------------
7               L      17        
6               L      16        
r               V      91        
-3              T      92        
3               C      93        
5               L      11        
k               V      94        
-2              T      95        
-1              T      96        
4               L      3         
c               V      97        
3               L      2         
b               V      98        
2               L      1         
a               V      99        
1               L      0         
0               L      0         

Programa SML generado (instrucciones 00-17):
00  +1099
01  +1098
02  +1097
03  +2099
04  +3098
05  +2196
06  +2096
07  +3097
08  +2195
09  +2095
10  +2194
11  +2094
12  +3293
13  +2192
14  +2092
15  +2191
16  +1191
17  +4300

Zona de datos (91-99):
91  +0000
92  +0000
93  +0003
94  +0000
95  +0000
96  +0000
97  +0000
98  +0000
99  +0000
```

### **Salida general**
Se genera el archivo `programa.sml`. Las primeras líneas corresponden a las instrucciones y las últimas a la zona de datos:
```
1099
1098
1097
2099
3098
2196
2096
3097
2195
2095
2194
2094
3293
2192
2092
2191
1191
4300
0
0
0
0
...
```
### **Instrucciones soportadas**

|  Instrucción  |  Descripción  |
| ------------- |:-------------:|
|`input variable`| 	Lee un valor y lo guarda en `variable`     |
| `print variable` | Imprime el valor de `variable`    |
| `newline`  | 	Imprime un salto de línea    |
|`let var = expresion`| valúa una expresión aritmética (`+ - * / % ^` y paréntesis) y la asigna a `var`   |
| `if op1 relop op2 goto linea`   | Salto condicional (`==, !=, >, <, >=, <=`)     |
|`goto linea`    |	Salto incondicional  |
|`rem comentario`| Comentario, se ignora     |
| `end` | 	Finaliza el programa (HALT)    |

### **Archivos Principales**

| Archivo  | 	Descripción |
| ------------- | ------------- |
| `Compilador SML.c`      | Código fuente completo del compilador. Contiene todas las funciones del proceso de compilación.     |
| `SimpletronM7.c`    | Codigo fuente de la Simpletron con todas las mejoras mencionadas.     |
| `programa.simple`      | Archivo de entrada (creado por el usuario) con el código fuente en lenguaje Simple.    |
| `programa.sml`      | 	Archivo de salida generado por el compilador con el programa objeto para la Simpletron.    |

### Laboratorio de GitBranch
En el repositorio se encuentran dos carpetas `.zip` en las que estan los laboratorios de gitbranch realizados durante el curso
