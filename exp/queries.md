// Consultas con 1 OPTIONAL
.*sparql!x{[^\n]*OPTIONAL[^\n]*\n}.*

// Consultas con 2 OPTIONAL
.*sparql!x{[^\n]*OPTIONAL[^\n]*OPTIONAL[^\n]*\n}.*

// Consulta a las 12hrs con un FILTER
.*\n.................12[^\n]*sparql!x{[^\n]*FILTER[^\n]*\n}.*

// Consulta con 2 OPTIONAL y el tiempo de la ejecución
.*\n.................!y{\d\d:\d\d:\d\d}[^\n]*sparql!x{[^\n]*OPTIONAL[^\n]*OPTIONAL[^\n]*\n}.*

// Consulta con 2 FILTER y el endpoint
.*\n[^\n]*+0000 !y{[^ \n]*}[^\n]*sparql!x{[^\n]*FILTER[^\n]*FILTER[^\n]*\n}.*


Otros exp:
	1. Todas las consultas que usan prefijo en particular
	2. Todas consultas que usan SELECT DISICT
	3. Todas las consultas que usan toda la variable (SELECT *) 
	4. Todas las consultas que usan count (count despues de SELECT y antes de WHERE)
	5. Buscar un order by
	6. Todas las consultas que definen un grafo (GRAPH)
	7. Todas las consultas que tienen LIMIT de menos de 4 digitos
	8. Todas las consultas que tienen mas de 3 PREFIX capturando entre el PREFIX y SELECT
	9. Consultas que que usan tanto OPTIONAL como FILTER (orden, usar |) 
	10. COnsultas que usan mas de 5 triple patterns despues del WHERE (puntos que esten entre 2 espacios
	