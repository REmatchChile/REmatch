import sys
import os.path
from time import time_ns
import timeit
from math import floor
import re
import re2
import pandas as pd

dir_path = os.path.dirname(os.path.realpath(__file__))
sys.path.append(os.path.abspath(dir_path + '/' + '../../bin/SWIG'))

import rematch

NEXPERIMENTS = 10
QUERIES = (1,2,3)
COMBOS = (1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11)

def rematch_experiment(regex, doc):
  count = 0
  rgx = rematch.RegEx(regex)
  while True:
    match = rgx.findIter(doc)
    # if(match):
    count += 1
    # print(match.span('x'))
    if(count > 10):
      break
    # else:
    #   break
  return count

def relib_experiment(regex, doc):
  n_outputs = 0
  for _ in re.finditer(regex, doc):
    n_outputs += 1

  return n_outputs

def re2lib_experiment(regex, doc):
  n_outputs = 0
  for _ in re2.finditer(regex, doc):
    n_outputs += 1

  return n_outputs

def load_doc(docPath):
  with open(docPath, "r", encoding='latin1') as fp:
    ret = fp.read()
  return ret

def load_query(queryNum, is_rematch=False):
  rgx_filename = "regex" if is_rematch else "re2regex"
  path = os.path.join(dir_path, '../../', "exp/RKBExplorer/exp{}/{}.rgx".format(queryNum, rgx_filename))
  return load_doc(path)

def load_sparql_combo(comboNum):
  path = os.path.join(dir_path, '../../', "datasets/RKBExplorer/sparql.log.combo.{}".format(comboNum))
  return load_doc(path)

FUNCTIONS = {
  "rematch" : rematch_experiment,
  "re" : relib_experiment,
  "re2" : re2lib_experiment
}

def main_benchmark():
  ## Dummy code, to run benchmark in "hot"
  query, doc = load_query(QUERIES[0]), load_sparql_combo(COMBOS[0])
  for _ in range(NEXPERIMENTS):
    relib_experiment(query, doc)


  ## Actual bechmark
  for q in QUERIES:
    data = list()
    for _, c in enumerate(COMBOS):
      qdata = {"combo": c}
      logdoc = load_sparql_combo(c)
      print("\tLoaded combo.{}".format(c))
      # run once to get output numbers
      for func in FUNCTIONS:
        query = load_query(q, (func == "rematch"))
        print("Loaded query {}".format(q))
        num_outputs = FUNCTIONS[func](query, logdoc)
        print("\t\tOutputs: {}".format(num_outputs))

        tot_t = 0

        for _ in range(NEXPERIMENTS):
          start = time_ns()
          FUNCTIONS[func](query, logdoc)
          t = floor((time_ns()-start) // 10**6)
          print("\t\t\tTook: {} ms".format(t))
          tot_t += t

        tot_t /= NEXPERIMENTS
        print("\n\t\tTot time: {} ms".format(tot_t))
        qdata.update({func: tot_t})

      data.append(qdata)

    df = pd.DataFrame(data)
    df.to_csv("query{}.csv".format(q), index=False)

if __name__ == "__main__":
  main_benchmark()





