# REmatch - Python version

Interfaz desarrollada para hacer uso en python de la librería de expresiones regulares `REmatch` creada en c++. 

Esta interfaz está adaptada para tener una sintaxis similar a la librería `Re` encontrada por defecto en python, sin embargo, algunas funciones no tiene el mismo comportamiento por lo que se recomienda leer la documentación de forma detallada.

Es importante mencionar que las expresiones regulares deben estar entre `.*` para ser compiladas, por ejemplo:
    
    .*correo@!dominio{gmail}.cl.*

## __Instalación (En un futuro)__

Puedes instalar la libreria desde [PyPI](http://link.com):

    pip install REmatch

## __Historial de versiones__

0.1

    Implementacion de las funciones:
    - find
    - findall
    - finditer
    - search
    - match
    - fullmatch

## __Contenido del modulo__

### REmatch. __compile(pattern, flags)__

Compila una expresion regular en un ``Regex object``, el cual puede ser usado para hacer match con los metodos que se describirán a continuacion.

## Regular Expression Object

### Regex. __find(string)__

Escanea el string de izquierda a derecha hasta encontrar la primera posición donde la expresion regular produzca match, retornando el correspondiente `Match object`. Retorna `None` si no se logra hacer match con el string.

    >> pattern = rematch.compile('.*d.*')
    >> pattern.find('dog') # Match at index 0
    <REmatch.Match object at 0x7f374c2e2bd0>

### Regex. __search(string)__

Mismo comportamiento de `find`. Creado para mantener sintaxis con libreria `Re`.

### Regex. __match(string)__ *

Si cero o mas caracteres desde la primera posicion del string hacen match con la expresion regular, retorna el correspondiente `match object`. Retorna `None` si no se hace match con el inicio del string.

    >> pattern = rematch.compile('.*a...s.*')
    >> pattern.match('abyssal') # Match at index 0
    
### Regex. __fullmatch(string)__ *

Si la expresion regular es capaz de hacer match con todo el string, retorna el correspondiente `match object`. Retorna `None` en caso contrario.

    >> pattern = rematch.compile('.*a...s.*')
    >> pattern.fullmatch('abyssal')
    None
    >> pattern.fullmatch('abyss)

### Regex. __findall(string)__

Escanea el string de iquierda a derecha encontrando todos los substring que produzcan match con la expresion regular. Retorna una lista de `match object` en el orden en que fueron encontrados. En caso de no producir ningun match retorna una lista vacia.

    >> pattern = rematch.compile('.*!x{teen}.*')
    >> matches = pattern.findall('fifteen, sixteen, seventeen,...')
    [<REmatch.Match object at 0x7f163ba14b10>, <REmatch.Match object at 0x7f163ba1e150>, <REmatch.Match object at 0x7f163ba2abd0>]
    >>
    >> for match in matches:
    >>     print(match.span('x'), match.group('x'))
    (3, 7) teen
    (12, 16) teen
    (23, 27) teen

### Regex. __finditer(string)__

Mismo comportamiento de `findall`. Retorna un `iterator` de `match objects` en el orden que fueron encontrados. En caso de no producir ningun match retorna un iterador vacio.
 
    >> pattern = rematch.compile('.*!x{teen}.*')
    >> matches = pattern.finditer('fifteen, sixteen, seventeen,...')
    <generator object Regex.finditer at 0x7f08c46d3850>
    >>
    >> for match in matches:
    >>     print(match.span('x'), match.group('x'))
    (3, 7) teen
    (12, 16) teen
    (23, 27) teen

## Match Objects

Notar que para todas las funcionalidades de los `match objects` es necesario hacer uso de los `capture`.

El `capture` (especificado previamente en la compilación de la expresion regular) puede ser el nombre en formato de string o el indice (partiendo en 1) en formato de integer. La sintaxis para un capture es `!capture_name{regular_expression}`.

### Match. __start(capture) / end(capture)__
Retorna el indice del inicio/termino del substring que ha hecho match especificamente con el ``capture`` indicado en la expresion regular.

    >> pattern = rematch.compile('.*!var{stick}.*')
    >> match = pattern.find("fantastick")
    >> match.start('var')
    5
    >> match.end('var')
    10

### Match. __span(capture)__

Para un match `m` retorna la tupla ``(m.start(capture), m.end(capture))``.

    >> pattern = rematch.compile('.*!var{stick}.*')
    >> match = pattern.find("fantastick")
    >> match.span('var')
    (5, 10)

### Match. __group(capture)__ *

Retorna el substring asociado al `capture`. Notar que si la flag `save_anchors` está activada, estará disponible `match.group(0)` que retornará el string completo con el cual se ha hecho match.

    >> pattern = rematch.compile('.*!var1{fan}..!var2{stick}.*')
    >> match = pattern.find("fantastick")
    >> match.group('var1')
    fan
    >> match.group(1)
    fan
    >> match.group('var2')
    stick
    >> match.group(2)
    stick


### Match. __groups(default=None)__ *

Retorna una tupla de strings que contiene todos los grupos del match.

    >> pattern = rematch.compile('.*!var1{fan}..!var2{stick}.*')
    >> match = pattern.find("fantastick")
    >> match.groups()

### Match. __groupdict(default=None)__ *

Retorna un diccionario de todos los grupos en el match. Cada elemento tiene como key el nombre del grupo y value el string correspondiente al grupo.

    >> pattern = rematch.compile('.*!name{\w+}, !city{\w+}.*')
    >> match = pattern.find("Erick, Santiago")
    >> match.groupdict()

## Ejemplos de uso

Ejemplos mas complejos una vez que esté lista la libreria...

    Codigo
    Codigo
    Codigo
    Codigo
