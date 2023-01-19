import os
import os.path

HERE = os.path.dirname(os.path.dirname(__file__))
PROJECT_DIR = os.path.abspath(os.path.join(HERE, "../.."))


with open(os.path.join(PROJECT_DIR, 'docs/doc1.txt'), 'w') as fp:
  fp.write(f'c{"a" * 200}{"b" * 200}{"a" * 200}{"b" * 200}{"a" * 200}c')
