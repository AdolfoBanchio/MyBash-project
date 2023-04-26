# LABORATORIO 1: MYBASH #

Desarrollado por:
### La Miñoneta ###
* Banchio, Adolfo <adolfo.banchio@mi.unc.edu.ar>
* Gimenez, Julieta <julieta.gimenez.888@mi.unc.edu.ar>
* Viglianco, Agustín <aviglianco@mi.unc.edu.ar>

---
## Introducción ##
En este primer laboratorio buscamos programar un **shell** al estilo bash, un programa que nos permite exponer las funcionalidades del sistema operativo para su utilización desde el lado del usuario. Mybash, el nombre que recibe nuestra shell, permite a los usuarios ejecutar comandos en modo *foreground* y *background*, utilizar redirectores para redirigir la entrada y salida de los procesos que ejecute, y realizar *pipes* entre comandos. 

A continuación, procederemos a desarrollar un breve marco teórico que resulta importante comprender para realizar una correcta lectura del presente informe. En primer lugar, queremos definir **proceso**, que refiere a la instancia de un programa que se encuentra en ejecución en un momento dado. Además, es importante notar que "dentro" de estos procesos, pueden generarse a su vez nuevos procesos, lo que da origen a una estructura filiatoria donde el proceso creador se considera el *padre* del proceso que crea (que identificamos como *hijo*). La creación de estos procesos nos da pie para presentar el concepto de **llamada a sistema** o **system call**, que es un mecanismo que permite que el usuario solicite un servicio al sistema operativo, de modo que pueda acceder a las funcionalidades específicas del kernel. Las llamadas a sistema más utilizadas a lo largo de este proyecto son: *fork*, que permite crear una copia de un proceso; *execvp*, que permite ejecutar un binario arbitrario; *wait*, que bloquea el proceso actual hasta que sus hijos terminen la ejecución; y *open*/*close*, que permiten abrir y cerrar archivos especificados.

Por otro lado, y alejándonos de los apartados técnicos, consideramos al siguiente proyecto como una gran oportunidad para afianzar nuestros conocimientos teóricos, descubrir y comprender el funcionamiento del sistema de control de versiones *GIT*, aplicar buenas prácticas de programación y desarrollar las habilidades necesarias para construir un entorno agradable para el trabajo en equipo.

#### Objetivos ####
1. Utilizar los mecanismos de concurrencia y comunicación de gruesa granularidad que brinda UNIX.
2. Comprender que un intérprete de línea de comandos refleja la arquitectura y estructura interna de las primitivas de comunicación y concurrencia.
3. Implementar de manera sencilla un intérprete de línea de comandos.
4. Utilizas buenas prácticas de programación.
5. Descubrir y comprender el funcionamieno del software de control de versiones.

---
## Uso ##
Se incluye un Makefile con todos los elementos necesarios para la compilación del programa. Para ejecutar el programa, se debe incluir la siguiente secuencia en cualquier terminal:

> $ make
> 
> $ ./mybash

De igual forma, se incluye una batería de tests para comprobar el funcionamiento tanto de los distintos módulos, como del programa en su totalidad. Para utilizarlos, se puede ejecutar en la terminal:

> $ make test (test general)
> 
> $ make test-command (test de los TADs)
> 
> $ make test-parsing (test del módulo de parseo)
> 
> $ make memtest (test de manejo de memoria dinámica)

---
## Modularización ##
Para el desarrollo de este proyecto, elegimos seguir con el esquema de módulos recomendados por los docentes de la cátedra. El esquema en cuestión consistía de dividir la implementación en 6 módulos, que serán detallados a continuación:

1. **mybash:** módulo principal donde se encuentra la función *main*.
2. **command:** módulo con las definiciones de los TADs necesarios para representar comandos.
3. **parsing:** módulo de procesamiento de entrada de usuario.
4. **parser:** módulo proporcionado por la cátedra, que implementa el TAD *parser*.
5. **execute:** módulo que administra las llamadas al sistema operativo y ejecuta los comandos.
6. **builtin:** módulo que implementa los comandos internos del intérprete de comandos.

Como añadido, presentamos los siguiente módulo adicionales:

+ **prompt:** módulo que imprime un *prompt* con información relevante para el usuario.
+ **strextra:** módulo que contiene funciones útiles para el manejo de strings.

---
## Desarrollo ##
A la hora de desarrollar el proyecto, comenzamos implementando de manera conjunta el módulo **command**, al ser este fundamental para el desarrollo de los restantes módulos. Una vez finalizada la implementación de los TADs (que, igualmente, continuamos perfeccionando a medida que avanzábamos), decidimos dividir los demás módulos para aliviar la carga de trabajo y avanzar con mayor rapidez. De todos modos, mantuvimos constante la comunicación entre los integrantes, de forma tal que todos estuviéramos informados sobre el trabajo de los demás, con el objetivo de que todos tengamos injerencia en la totalidad del proyecto.

En lo que respecta al trabajo con memoria dinámica, intentamos trabajar de la forma más prolija posible, liberando siempre toda la memoria solicitada utilizando *free()*, y estudiando con detalle los métodos de bibliotecas externas (detallados más adelante) para verificar que no se perdieran referencias a elementos que nos puedan generar *memory leaks*. Utilizamos Valgrind como herramienta para controlar y minimizar los errores relacionados con el manejos de memoria dinámica.

De igual manera, considerando que este es un programa que depende fuertemente de las entradas del usuario, pusimos mucha atención a los distintos factores que pueden producir errores. Intentamos, en todo momento, ser cuidadosos con el manejo de los comandos inválidos que pudieran ingresar, para evitar que esto cause errores inesperados o la terminación abrupta del programa. Para esto, incluimos diferentes chequeos para posibles entradas, e imprimimos mensajes de error tanto para los casos donde falle la entrada del usuario, como para los casos donde fallen las llamadas al sistema. También recurrimos al uso de la función *assert()* para chequear que se cumplan las pre y postcondiciones necesarias para el correcto funcionamiento de todas las funciones implementadas.

#### Bibliotecas utilizadas ####
+ **assert.h**: incluye la macro *assert()*, que permite diagnosticar problemas "internos" de nuestra implementación.
  La utilizamos a lo largo y ancho de toda la implementación, ya que permite chequear las precondiciones y poscondiciones de todas las funciones implementadas, chequeando así que no ocurriera ningún error causado por alguna desatención nuestra.
+ **fcntl.h**: define constantes que resultan útiles para indicar el modo de acceso y permisos de creación de los archivos a la hora de utilizar la llamada *open()*.
+ **glib**: biblioteca de funciones de propósito general. 
  Utilizamos el tipo abstracto *GQueue*, que escogimos para construir los TADs *scommand* y *pipeline*. Escogimos este tipo puesto que, al tratarse de una cola FIFO, facilitaba el uso de métodos como *push* y *pop* para insertar y extraer con facilidad los elementos en el orden en el que los leeríamos naturalmente.
+ **pwd.h**: define una estructura de tipo *passwd* que almacena información del usuario actual, con algunas funciones interesantes para recuperar esta información.
  La utilizamos en el módulo **prompt** para obtener el nombre del usuario para poder imprimirlo en el prompt, mediante el uso de la función *getpwuid()*.
+ No mencionaremos, por motivos evidentes, los demás módulos de la librería estándar de C, que nos facilitaron desde el trabajo con memoria dinámica, hasta la impresión de mensajes por los canales de salida estándar.

#### Decisiones de diseño ####
En primera instancia, mencionaremos ejemplos de comandos que consideramos inválidos a nivel sintáctico, por no tener sentido o presentarse incompletos:

+ *ls |*: está incompleto y se considera error de parseo.
+ *< builtin.c*: está incompleto, por no presentar el nombre del comando a ejecutar.
+ *xeyes & fdfkf*: es inválido, pues presenta caracteres "basura" al final.

Para todos estos casos, decidimos que el retorno de *parse_scommand* devuelva un pipeline nulo (result = NULL). La idea es que esto luego nos permite verificar este caso en el módulo **mybash**, para que ejecutemos en ningún momento la función *execute_pipeline* con un pipeline nulo como parámetro.

En adición, elegimos definir una nueva función en el archivo **strextra**, llamada *merge_with_free*, con el objetivo de realizar una concatenación entre dos comandos, pero liberando previamente la memoria solicitada por anteriores llamadas a la función *strmerge*. Esto nos permitió evitar el uso indiscriminado de llamadas a *free()*, y generar una implementación más limpia de algunos métodos de los TADs.

Por otra parte, dentro del módulo **builtin**, decidimos independizar el comando *exit* de los demás comandos internos. Tomamos esta decisión para garantizar el correcto funcionamiento de la salida de nuestro programa, permitiéndonos controlar con mayor comodidad la destrucción de las instancias de los TADs utilizados y liberar la memoria alocada sin causar *memory leaks*. Esta diferenciación la realizamos mediante la creación de una función llamada *builtin_is_exit*, que nos permite controlar en el módulo **mybash** si el usuario en algún momento quiere salir del programa.

También en el módulo **builtin**, decidimos definir como constantes los strings que corresponden a los nombres de los comandos. Esto permite que nuestro código se adapte con mayor facilidad en el caso de que debamos modificar el nombre de cualquiera de los comandos internos, evitando errores causados por olvidarnos de realizar algún reemplazo. Además, incluimos un "easter egg", que puede accederse ejecutando el comando *miñoneta* en mybash.

Finalmente, tomamos la decisión de modularizar algunas funciones en el módulo **execute**, que realicen los diferentes "pasos" necesarios para ejecutar correctamente un pipeline ingresado por el usuario. Esta división consiste de una función que guarde los argumentos del programa en un arreglo, otras que definan los nuevos canales de entrada y salida, y finalmente de una que ejecute un comando simple.

#### Debugging ####
Para debuggear nuestro código recurrimos en gran medida a la batería de tests brindada por los docentes de la cátedra. Para cada módulo, controlamos que se aprobaran todas las pruebas propuestas en esos tests.

Sin embargo, como los test están específicamente diseñados para probar situaciones complejas, procedimos también a realizar diferentes pruebas "manuales" en directorios independientes. Allí, construíamos ejemplos patológicos y los analizábamos exhaustivamente utilizando como herramientas *GDB* y *Valgrind*.

En este apartado, nos parece importante aclarar que el test de memoria, que utiliza la herramienta *Valgrind* para verificar que no se produzcan *memory leaks*, arroja que, tras la ejecución del test, algunos bloques de memoria no se liberan correctamente. Una breve investigación en internet nos dejó claro que esta posible "pérdida" de memoria se debe a la forma en la que se manejan las estructuras internas de GLib, por lo que no encontramos forma de solucionar dichos *leaks*. Ninguno es causa de problemas que dependan de nuestra implementación.

---
## Puntos estrella implementados ##
De entre los puntos estrella propuestos por la cátedra, conseguimos implementar dos. 

El primero de ellos consiste en imprimir un *prompt* que incluyera el nombre de usuario y el directorio en el que se encuentra trabajando actualmente. Adicionalmente, conseguimos también añadirle colores a los elementos ya mencionados, utilizando secuencias de escape ANSI. Esta implementación puede encontrarse en el módulo **promtp**.

El segundo de los puntos estrella que realizamos consiste en generalizar la implementación del pipeline para ejecutar pipelines de más de dos comandos. Para ello, decidimos considerar tres tipos diferentes de comandos de acuerdo a su ubicación en el pipeline: el 'primero' del pipeline debe leer desde la *stdin* y redirrecionar su salida; los 'intermedios' deben redirrecionar tanto su entrada (para leer desde el proceso anterior) como su salida (para enviarla al proceso siguiente); y finalmente el 'último' debe redirrecionar únicamente su entrada y conservar la *stdout* como salida. Esta solución permite mantener un registro ordenado de todos los comandos, y redirrecionar sus salidas sin cometer errores ni confusiones.

Queremos mencionar que, para el caso de los pipelines de más de dos comandos tienen un comportamiento idéntico al que tienen en bash tradicional, excepto para el caso de comandos como *ls -l | wc -l | wc -c*, que arrojan siempre el resultado 0. No conseguimos identificar (y por ende solucionar) el origen de esta falla.

---
## Conclusiones ##
A modo de conclusión del presente trabajo, consideramos que fue una excelente oportunidad para familiarizarnos con el funcionamiento de los procesos, las llamadas a sistema, los file-descriptors y las herramientas de testing y debugging utilizadas. Por otra parte, creemos también que fue muy útil para redescubrir el poder que tiene C como lenguaje de programación, utilizando las ya mencionadas llamadas al sistema. Nos resultó sumamente interesante la posibilidad de entender el funcionamiento interno de una terminal, y las diferentes funcionalidades que esta tiene implementadas.

Sentimos que el presente trabajo nos empujó hasta el límite de nuestras capacidades, teniendo que aplicar conocimientos adquiridos en materias anteriores, a la vez que los incorporados en el corriente semestre. En todo momento tuvimos que analizar los problemas que pudieran surgir, y considerar los diferentes y variados escenarios para que nuestro programa funcione de la forma más robusta y completa posible. Al mismo tiempo, y como ya fue mencionado anteriormente, resultó una oportunidad fundamental para afianzar algunos conocimientos teóricos vistos en la materia desde una perspectiva práctica y dinámica.

Por último, agradecemos la posibilidad de realizar el desarrollo en equipo, puesto que nos permitió desarrollar una variedad de habilidades blandas, y tener una experiencia similar a un equipo de trabajo en un entorno laboral. De igual modo, consideramos realmente valiosos los conocimientos adquiridos respecto al software de control de versiones, y esperamos poder seguir descubriendo sus ventajas a lo largo de los siguientes trabajos.