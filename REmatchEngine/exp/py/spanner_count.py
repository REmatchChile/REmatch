import re
import sys

with open(sys.argv[1]) as rgx:
	regex = rgx.read()

with open(sys.argv[2]) as docFile:
	doc = docFile.read()

matches = re.findall(regex, doc)
print(len(matches))
