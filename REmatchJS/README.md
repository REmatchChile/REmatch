# Rematch en JavaScript

Esta librería provee una serie de métodos para realizar búsquedas de texto, a través de expresiones regulares.
REmatch está creada originalmente en C++, y aquí se pone a disposición una interfaz para JavaScript. En esta versión se encuentran implementados sólo algunos métodos y funcionalidades.

Los patrones de expresión regular que utiliza REmatch siguen la siguiente sintaxis:

    '.*abc!nombre_variable{defg}.*'

Donde la variable `nombre_variable` guarda información relativa al match de su patrón. Más adelante se explica con más detalle como se puede acceder a esta información a través del objeto `Match`. El comportamiento es similar a los `groups` normalmente utilizados en expresiones regulares compiladas en la librería `re` de Python.

## Futura instalación

Para instalar Rematch usando `npm` se debe correr lo siguiente:

    npm install rematch

Luego se podrá cargar en proyectos usando:

    const REmatch = require('rematch');

## Forma de uso

En primer lugar, se comienza con la compilación del patrón de la expresión regular en un objeto de expresión regular:

    let rgx = REmatch.compile(pattern, flags);

Las flags son opcionales. El objeto que retornará tiene los métodos que se explicarán en la sección Métodos, y estos serán los que que reciben el texto sobre el cual se requiere hacer la búsqueda.

También, se pueden utilizar los métodos directamente desde la librería:

    let match = REmatch.find(pattern, string, flags);

Sin embargo, se recomienda utilizar la primera forma dado que será mucho más eficiente si se quiere evaluar el mismo `pattern` sobre varios documentos distintos.

## Flags (No disponibles)

El comportamiento de la expresión regular puede ser modificado si se especifican algunas de las siguientes flags. Sin embargo, por ahora no causan la modificación que deberían dado que no están implementados. Aún así, por default cada una es false.

* `multi_line`: Se utiliza para que `^` y `$` hagan match con otras líneas al hacer `\n`.
* `dot_nl`: Se utiliza para que el símbolo `.` haga match con otras líneas al hacer `\n`.
* `line_by_line`: Se utiliza para que la evaluación se haga línea por línea.
* `early_output`: El algoritmo entregará outputs a medida que los encuentra, de otra forma el algoritmo escanea todo el documento y luego retorna los resultados.
* `save_anchors`: Por default la librería guarda las variables o grupos desde el índice 1, sin embargo, si se activa esta flag se tendrá acceso al grupo de índice 0, y este almacenará la captura de todo el patrón de la expresión regular. 

### Ejemplo de utilización

    let rgx = REmatch.compile('.*Lorem ipsum \n dolor !var_1{.*} tristique.*', dot_nl=true, save_anchors=true);

## Métodos

* `find(string)`: Realiza una búsqueda anchored del primer match. Si encuentra una coincidencia, retorna un objeto `Match`, sino retorna `null`.
* `findIter(string)`: Entrega un iterador de JavaScript de todos los matches encontrados en el texto.
* `findall(string)`: Realiza una búsqueda anchored de todos los match del string con el patrón de la expresión regular. Retorna una lista con los objetos `Match` correspondientes.
* `search(string)`: Realiza una búsqueda anchored del primer match. Si encuentra una coincidencia retorna un objeto `Match`, sino retorna `null`. Este método si bien tiene el mismo comportamiento que `find`, se incluye para tener mayor compatibilidad con los métodos típicos para búsqueda con expresiones regulares.
* `match(string)`: Si los caracteres al inicio del `string` hacen match con el patrón de la expresión regular, retorna el correspondiente objeto `Match`. Si no es así, se retorna `null`. (Para esta versión, falta la resolución de algunos errores, por ende no se encuentra disponible para su uso por ahora).
* `fullmatch(string)`: Si todo el string hace match con el patrón de la expresión regular, entonces retorna el respectivo objeto `Match`. Si no, retorna `null`. (Con este método ocurre lo mismo que con `search`, es decir no está disponible por ahora).

Se sugiere utilizar los métodos `find`,`findIter` y `findall`, ya que los demás sólo se incluyen para tener compatibilidad con los métodos típicos utilizados para expresiones regulares.

## Objeto Match

Este objeto contiene toda la información relativa a la coincidencia encontrada según el patrón de la expresión regular entregado. Como ya se ha explicado antes, con REmatch se pueden indicar variables o grupos que encapsulan cierta información, los que se guardan por defecto desde el índice 1. A continuación, se referirá a estos sólo como variables, se indicará `var_id` al índice, y `var_name` al nombre de esta. Sin embargo, por ahora sólo se puede utilizar el `var_name` para acceder a la información de la variable.

Los métodos que posee `Match` son los siguientes:

* `start(var_name)`: Retorna la posición en el string donde comienza el span de la variable.
* `end(var_name)`: Retorna la posición en el string donde termina el span de la variable.
* `span(var_name)`: Retorna un array con el par del inicio y término del span de la variable.
* `group(var_name)`: Entrega el string que hizo match con el patrón contenido en la variable.
* `groups()`: Retorna un array, indexado por los `var_id`, con los strings capturados por cada una de las variables. (Falta arreglar un error para terminar la implementación de este método, por ende no está disponible por ahora)
* `group_dict()`: Entrega un diccionario que tiene como `keys` los nombres de las variables, y los `values` son los strings capturados por las variables. (Con este método ocurre lo mismo que con `groups()`, por lo tanto no está disponible).

## Ejemplos

### find(string)

    >> const text = 'Lorem ipsum dolor sit amet consectetur adipiscing elit euismod condimentum, conubia a mollis pulvinar nascetur taciti elementum praesent'
    >> let rgx1 = REmatch.compile('.*!word{dolor}.*')
    >> const result = rgx1.find(text).span('word')
    >> console.log(result)
    [ 12, 17 ]

### findIter(string)

    >> let rgx2 = REmatch.compile('.*!x{ly}.*');
    >> const text = "He was carefully disguised but captured quickly by police.";
    >> console.log(rgx2.findIter(text).next());
    >> console.log(rgx2.findIter(text).next());
    >> console.log(rgx2.findIter(text).next());
    { value: Match { obj: Match {} }, done: false }
    { value: Match { obj: Match {} }, done: false }
    { value: undefined, done: true }

### findall(string)
Si se quieren encontrar todos los adverbios en medio de un texto, se puede hacer de la siguiente forma:

    >> const text = "He was carefully disguised but captured quickly by police.";
    >> let rgx3 = REmatch.compile('.*!adverb{ [a-zA-Z]+ly}.*')
    >> let list = rgx3.findall(text);
    >> for (let index = 0; index < list.length; index++) {
    >>   const element = list[index].group('adverb');
    >>   console.log(element)
    >> }
     carefully
     quickly
