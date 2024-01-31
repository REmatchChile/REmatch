from pyrematch import Regex

rgx = Regex("!x{.+}")
doc = "aa"
for match in rgx.finditer(doc):
    print(match.span("x"))
