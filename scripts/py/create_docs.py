import random

s = ""

for _ in range(10000000):
  s += str(random.randint(0,9))

with(open('doc.txt', 'w')) as rfile:
  rfile.write(s)
