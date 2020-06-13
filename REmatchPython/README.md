##  ¿Como correr las pruebas?

- Primero, es necesario tener la carpeta RKBExplorer_exps de los datasets en este directorio.

Antes de correr los experimentos, habrá que modificar en el archivo `speedtest.py` la query a utilizar en la libreria RE de python (Line 68), ya que
si bien pedirá una query, esta solo corresponderá a la de REmatch. Ahora solo basta con correr `speedtest.py` y seguir las instrucciones 
en consola. Nota: Cuando pide la query hay que pasarla en el formato de REmatch (.* .*).

Al momento de correr irá dando detalles de las query en consola y al finalizar se creará un archivo `.csv` en la carpeta `results`. Si se
ejecutó solo un experimento, el archivo tendrá el detalle de tiempo de cada query comparado con RePython, y si se corrieron todos los experimentos,
se creará un archivo con el promedio de las iteraciones solicitadas.

Nota: Tambien se puede cambiar la query default (Opcion [0]) en la linea 47.
