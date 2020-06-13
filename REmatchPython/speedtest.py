import sys
sys.path.insert(0, "../REmatchEngine/bin/SWIG")
import rematch
import re 
from time import time
import os
import csv

def rematch_experiment(doc, query):
    n_outputs = 0
    interface = rematch.PyInterface(doc, query)
    t_i = time()
    while interface.hasNext():
        interface.next()
        n_outputs += 1
    t_f = time()
    total_time = t_f - t_i
    print(f"Total outputs: {n_outputs}")
    print(f"Total running time: {total_time}\n")
    return total_time, n_outputs

def re_python_experiment(doc, query, option):
    if option == 1:
        print("Usando findall")
        query = re.compile(query)
        t_i = time()
        exp = query.findall(doc)
        t_f = time()
        total_time = t_f - t_i
        print(f"Total outputs: {len(exp)}")
        print(f"Total running time findall: {total_time}\n")
        return total_time, len(exp)

    elif option == 2:
        print("Usando finditer")
        t_i = time()
        for m in query.finditer(doc):
            n_outputs += 1
        t_f = time()
        total_time = t_f - t_i
        print(f"Total outputs: {n_outputs}")
        print(f"Total running time finditer: {total_time}\n")
        return total_time, n_outputs

def test_a_file(combo, query, iterations):
    if query == "0":
        query = ".*sparql!x{[^\n]*OPTIONAL[^\n]*\n}.*" #DEFAULT QUERY
        #query = ".*sparql!x{[^\n]*OPTIONAL[^\n]*OPTIONAL[^\n]*\n}.*" #QUERY 2
        #query = ".*.................12[^\n]*sparql!x{[^\n]*FILTER[^\n]*\n}.*"
    file = "sparql.log.combo." + combo
    path = os.path.join('RKBExplorer_exps', 'Datasets', file)
    with open(path, encoding="ISO-8859-1") as file:
        doc = file.read()

    rows = []
    times = []
    print("="*12+" INICIANDO REMATCH "+"="*12)
    for i in range(iterations):
        exp = rematch_experiment(doc, query)
        times.append(exp[0]) #Agregamos cada tiempo a una lista
        rows.append([i+1, exp[0], exp[1]]) #Agregamos: iteracion, tiempo, outputs a fila csv
        out_rematch = exp[1]

    # La query del modulo re hay que cambiarla manualmente.
    times_re = []
    print("="*12+" INICIANDO RE PYTHON "+"="*12)
    for i in range(iterations):
        exp = re_python_experiment(doc, ".................12.*sparql(?P<x>.*FILTER.*)", 1)
        times_re.append(exp[0])
        rows[i].append(exp[0]) #Agregamos tiempo
        rows[i].append(exp[1]) #Agregamos output
        out_repython = exp[1]
    avg_rematch = sum(times)/len(times)
    avg_repython = sum(times_re)/len(times_re)
    print(f"AVERAGE ReMatch: {avg_rematch}")
    print(f"AVERAGE Competencia: {avg_repython}")
    return rows, avg_rematch, avg_repython, out_rematch, out_repython

def test_all(query, iterations):
    rows = []
    for i in range(11):
        combo = str(i + 1)
        test = test_a_file(combo, query, iterations)
        rows.append([f"sparql.log.combo." + combo, test[1], test[3], test[2], test[4]])
    with open(f"results/results_all_experiments.csv", mode="w") as csv_file:
        fieldnames = ['Archivo', 'Tiempo_ReMatch', 'Outputs_ReMatch', 'Tiempo_RePython', 'Outputs_RePython']
        writer = csv.writer(csv_file)
        writer.writerow(fieldnames)
        writer.writerows(rows)

def write_a_csv(combo, rows, avg_rematch, avg_repython):
    with open(f"results/result_combo{combo}.csv", mode="w") as csv_file:
        fieldnames = ['Iteracion', 'Tiempo_ReMatch', 'Outputs_ReMatch', 'Tiempo_RePython', 'Outputs_RePython']
        writer = csv.writer(csv_file)
        writer.writerow(fieldnames)
        writer.writerows(rows)
        writer.writerow(["Promedio", avg_rematch, '-', avg_repython, '-'])

if __name__ == "__main__":
    print("[0] Test all files\n[1] Test a file")
    choice = input("Option: ")
    if choice == '1':
        combo = str(input("Ingrese el numero del combo: "))
        query = input("Ingrese consulta ([0] para usar default): ")
        iterations = int(input("Iteraciones: "))
        test = test_a_file(combo, query, iterations)
        write_a_csv(combo, test[0], test[1], test[2])
    elif choice == '0':
        query = input("Ingrese consulta ([0] para usar default): ")
        iterations = int(input("Iteraciones: "))
        test_all(query, iterations)