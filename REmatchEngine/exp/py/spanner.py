import re
import sys

with open(sys.argv[1]) as rgx:
	regex = rgx.read()

with open(sys.argv[2]) as docFile:
	doc = docFile.read()

print('Regex:', regex)

prog = re.compile(regex, re.DOTALL)

matches = prog.findall(doc)
print(matches)
