import os.path
import sys

dir_path = os.path.dirname(os.path.realpath(__file__))
sys.path.append(os.path.abspath(dir_path + '/' + '../../build/Debug/bin/SWIG'))

import rematch as re

regex = ".*!x{a+}.*"
document = "bbbbaaaabbbbbaaa"

rgx = re.RegEx(regex)
match = rgx.findIter(document)

while match:
  print(match.span("x"))
  match = rgx.findIter(document)

