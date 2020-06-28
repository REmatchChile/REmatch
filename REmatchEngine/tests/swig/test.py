import sys
import os.path

dir_path = os.path.dirname(os.path.realpath(__file__))
sys.path.append(os.path.abspath(dir_path + '/' + '../../bin/SWIG'))

import rematch as re

# Instantiate RegExOptions object
rgx_opts = re.RegExOptions()

# Set early output flag to True
rgx_opts.set_early_output(True)

# Instantiate RegEx object with the options
rgx = re.RegEx(".*!x{a}.*", rgx_opts)

# Search every output
while True:
  match = rgx.findIter("bbbaa")
  if (not match): # If the match is not found
    break
  else:
    for var in match.variables():
      print(match.span(var))
