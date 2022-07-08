import random

s = ""
choices = ["A", "C", "G", "T"]

for _ in range(100000):
  s += str(random.choice(choices))

with(open('doc.txt', 'w')) as rfile:
  rfile.write(s)
