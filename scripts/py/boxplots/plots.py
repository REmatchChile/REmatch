import pandas as pd

from benchmark import run_experiments
import os
import os.path

here = os.path.dirname(os.path.dirname(__file__))
PROJECT_DIR = os.path.abspath(os.path.join(here, "../../.."))

T = pd.read_csv(os.path.join(PROJECT_DIR, "exp/crossings/morphemes/2grams/results/rm-versions/time.csv"))
M = pd.read_csv(os.path.join(PROJECT_DIR, "exp/crossings/morphemes/2grams/results/rm-versions/mem.csv"))
O = pd.read_csv(os.path.join(PROJECT_DIR, "exp/crossings/morphemes/2grams/results/rm-versions/outputs.csv"))

bp = T.boxplot(column=[f"REm-{i}" for i in range(6)])

bp