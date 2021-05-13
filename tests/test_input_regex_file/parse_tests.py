import os
import os.path as pth
import re
from functools import cmp_to_key
from collections import defaultdict

here = pth.dirname(pth.realpath(__file__))

class Test:

  def __init__(self, doc, pattern, mappings):
    self.doc = doc
    self.pattern = pattern
    self.vars = mappings.vars
    self.mappings = mappings.mappings

  def to_cpp_style(self, max_width=80):
    v = str(self.vars); m = str(self.mappings)
    v = v.replace("[", "").replace("]", "").replace("'", "\"")
    m = m.replace("[", "{").replace("]", "}").replace("(", "{").replace(")","}")
    v = ''.join(v.split())
    m = ''.join(m.split())
    v = v if self.vars else " "
    s = f"  {{\"{self.doc}\",\"{self.pattern}\",{{{v}}},"

    s += "{"

    max_allowed = 80

    tab_idx = 4

    end_idx = len(s)

    curr_idx = end_idx

    v = ""

    dlines = False
    for mapping in self.mappings:
      sp = ",".join(f"{{{span[0]},{span[1]}}}" for span in mapping)
      sp = f"{{{sp}}},"
      if(curr_idx + len(sp) <= max_allowed):
        v += sp
        curr_idx += len(sp)
      elif not dlines:
        dlines = True
        v += sp
        curr_idx += -end_idx + tab_idx + len(sp)
      else:
        v += '\n' + (" " * tab_idx) + sp
        curr_idx = tab_idx + len(sp)

    if dlines:
      s += "\n    " + v
    else:
      s += " " + v

    s = s[:-1]
    s += " }}"

    return s

class Mappings:

  def __init__(self, vars, mapping_list):
    self.vars = vars
    self.mappings = mapping_list



def parse_spanners(s):
  mappings = list()
  for line in s.split('\n'):
    line = ''.join(line.split())
    if not line:
      break
    mapping = dict()
    while(line):
      i = line.find("=|"); j = line.find(">");
      var = line[:i]; lo, hi = line[i+2:j].split(',')
      mapping[var] = (int(lo), int(hi))
      line = line[j+1:]
    mappings.append(mapping)

  return mappings


def parse_directory(path):
  with open(pth.join(path, 'document.txt')) as f:
    doc = f.read()
  with open(pth.join(path, 'regex.txt')) as f:
    rgx = f.read()
  with open(pth.join(path, 'spanners.txt')) as f:
    spanners = f.read()
    mappings = parse_spanners(spanners)

  return Test(doc, rgx, adapt_mappings(mappings))

def adapt_mappings(mappings):
  vars = list()
  nvars = 0
  new_mappings = list()
  for mapping in mappings:
    if nvars == 0:
      nvars = len(mapping)
    else:
      assert nvars == len(mapping)
    new_mapping = list()
    for i, (key, value) in enumerate(mapping.items()):
      if len(vars) < nvars:
        vars.append(key)
      else:
        assert vars[i] == key
      new_mapping.append(value)
    new_mappings.append(new_mapping)

  assert len(vars) == nvars

  def maporder(m1, m2):
    assert len(m1) == len(m2)
    for i in range(len(m1)):
      for j in range(2):
        if m1[i][j] < m2[i][j]:
          return -1
        elif m1[i][j] > m2[i][j]:
          return 1
    return 0

  maporder_key = cmp_to_key(maporder)

  new_mappings.sort(key=maporder_key)

  return Mappings(vars, new_mappings)





if __name__ == "__main__":
  classifier = defaultdict(list)
  for root, subdirectories, files in os.walk(here):
    subdirectories.sort()
    for subdirectory in subdirectories:
      classifier[subdirectory[:-1]].append(subdirectory)

  tot = ""

  for key, value in classifier.items():
    s = "std::vector<RegexTest> " + key + " = {\n"
    v = ",\n".join(parse_directory(pth.join(here, directory)).to_cpp_style() for directory in value)
    s += v + '\n};\n\n'
    tot += s

  with open("regex_tests.cpp", "w") as f:
    f.write(tot)
