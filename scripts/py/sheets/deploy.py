# Deploys the experiment
import matplotlib.pyplot as plt
import numpy as np
import subprocess
from math import ceil, floor, sqrt
from pathlib import Path
from random import choice
from string import ascii_lowercase
from time import sleep
from statistics import median, mean
import itertools
import os

from scipy.stats import t

from pylatex import Document, Section, Figure, NoEscape

HOME_DIR = "/home/nicovsj/repos/research/SpannersConst"

SYSTEMS = ["rematch", "re2"]
EXP_CLASSES = ["1var", "2vars"]

COMMANDS = {
    "rematch" :
        "{0}/build/Release/bin/rematch -l -o nmappings -d {0}/datasets/syn/{1}/exp{2:02d}/r{3}/n{4}.txt -r {0}/exp/syn/{1}/exp{2:02d}/rematch.rgx",
    "re2" :
        "{0}/build/Release/bin/test-re2 {0}/datasets/syn/{1}/exp{2:02d}/r{3}/n{4}.txt {0}/exp/syn/{1}/exp{2:02d}/re2.rgx"
}

MARKERS = {
    "rematch" : "o",
    "re2"     : "^"
}

DOC_REPS = (10, 25, 50, 100, 200, 400)
# DOC_REPS = (10, 25, 50)
N_RANGE = (1, 200)
EXPS_TO_RUN = (1,)
EXPCLASSES_TO_RUN = ("1var",)
N_EXP = 60
SLEEP = 0

def randstr(chars, n):
    ret = ""
    for _ in range(n):
        ret += choice(chars)
    return ret

def z_factor(r, n):
  return t.ppf(1-r, n-1)

def conf_interval(r, n, sigma):
  return z_factor(r, n) * sigma / sqrt(n)


FILES = {
    '1var' : {
        1 : {
            'rematch' : lambda n: ".*c!x{{{0}}}c.*".format('a'*n) ,
            're2' : lambda n: "c({0})c".format('a'*n),
            'doc' : lambda n, r : "c{}c".format('a'*n) * r,
            'regexstr' : r"$\Sigma^*c!x\{a^n\}c\Sigma^*$",
            'docstr' : r"$(ca^nc)^r$"
        },
        2 : {
            'rematch' : lambda n: ".*c!x{a+}c.*",
            're2' : lambda n: "c(a+)c",
            'doc' : lambda n, r : "c{}c".format('a'*n) * r,
            'regexstr' : r"$\Sigma^*c!x\{a^+\}c\Sigma^*$",
            'docstr' : r"$(ca^nc)^r$"
        },
        7 : {
            'rematch' : lambda n: ".*c!x{(a|b)*a*ac}.*",
            're2' : lambda n: "c((?:a|b)*a*ac)",
            'doc' : lambda n, r : "c{0}b{0}c".format('a'*n ) * r,
            'regexstr' : r"$\Sigma^*c!x\{(a+b)^*a^*ac\}\Sigma^*$",
            'docstr' : r"$(ca^nba^nc)^r$",
            'n_range' : (2, 200)
        },
        3 : {
            'rematch' : lambda n: ".*c!x{{{0}{1}}}c.*".format('a'*n, "a?"*n) ,
            're2' : lambda n: "c({0}{1})c".format('a'*n, "a?"*n),
            'doc' : lambda n, r : "c{}c".format('a'*n) * r,
            'regexstr' : r"$\Sigma^*c!x\{a^n(a?)^n\}c\Sigma^*$",
            'docstr' : r"$(ca^nc)^r$"
        },
        4 : {
            'rematch' : lambda n: ".*c!x{{{1}{0}}}c.*".format('a'*n, "a?"*n) ,
            're2' : lambda n: "c({1}{0})c".format('a'*n, "a?"*n),
            'doc' : lambda n, r : "c{}c".format('a'*n) * r,
            'regexstr' : r"$\Sigma^*c!x\{(a?)^na^n\}c\Sigma^*$",
            'docstr' : r"$(ca^nc)^r$"
        },
        5 : {
            'rematch' : lambda n: ".*c!x{{(a|b)*b{0}}}c.*".format('a'*n) ,
            're2' : lambda n: "c((?:a|b)*b{0})c".format('a'*n),
            'doc' : lambda n, r : "c{}c".format("{0}b{1}".format(randstr(['a', 'b'], 10*n), 'a'*n))*r,
            'regexstr' : r"$\Sigma^*c!x\{(a+b)^*ba^n\}c\Sigma^*$",
            'docstr' : r"$(c(a+b)^{10n}ba^n)c)^r$",
            'doc_reps' : (10, 25, 50, 100, 200, 400)
        },
        6 : {
            'rematch' : lambda n: ".*c!x{{{0}b(a|b)*}}c.*".format('a'*n) ,
            're2' : lambda n: "c({0}b(?:a|b)*)c".format('a'*n),
            'doc' : lambda n, r : "c{}".format("{1}b{0}cc".format(randstr(['a', 'b'], 10*n), 'a'*n)*r),
            'regexstr' : r"$\Sigma^*c!x\{a^nb(a+b)^*\}c\Sigma^*$",
            'docstr' : r"$c(a^nb(a+b)^{10n}cc)^r$",
            'doc_reps' : (10, 25, 50, 100, 200, 400)
        },
        8 : {
            'rematch' : lambda n: r".*c!x{[^c]+}c.*",
            're2' : lambda n: "c([^c]+)c",
            'doc' : lambda n, r : "c{}c".format('a'*n) * r,
            'regexstr' : r"$\Sigma^*c!x\{[\wedge c]+\}c\Sigma^*$",
            'docstr' : r"$(ca^nc)^r$"
        },
        9 : {
            'rematch' : lambda n: r".*c!x{a*ba*}c.*",
            're2' : lambda n: "c(a*ba*)c",
            'doc' : lambda n, r : gen_uniform(r"ca*ba*c", n+2, r),
            'regexstr' : r"$\Sigma^*c!x\{a*ba*\}c\Sigma^*$",
            'docstr' : r"$g(c(a*ba*)^{|x|=n}c)^r$"
        },
        10 : {
            'rematch' : lambda n: r".*c!x{a*ba*ba*}c.*",
            're2' : lambda n: "c(a*ba*ba*)c",
            'doc' : lambda n, r : gen_uniform(r"ca*ba*ba*c", n+2, r),
            'regexstr' : r"$\Sigma^*c!x\{a*ba*ba*\}c\Sigma^*$",
            'docstr' : r"$g(c(a*ba*ba*)^{|x|=n}c)^r$"
        },
        11 : {
            'rematch' : lambda n: r".*c!x{a+}(ba+)+bbc.*",
            're2' : lambda n: "c(a+)(?:ba+)+bbc",
            'doc' : lambda n, r : gen_uniform(r"c(aaaaa)+(ba+)+bbc", n, r),
            'regexstr' : r"$\Sigma^*c!x\{a^+\}(ba^+)^+bbc\Sigma^*$",
            'docstr' : r"$g((c(a^5)^+(ba^+)^+bbc))^{|x|=n})^r$",
            'n_range' : (15, 200),
        },
        12 : {
            'rematch' : lambda n: r".*c!x{(a|b)+}ab+c.*",
            're2' : lambda n: "c((?:a|b)+)ab+c",
            'doc' : lambda n, r : gen_uniform(r"c(a|b)+ab+c", n, r),
            'regexstr' : r"$\Sigma^*c!x\{(a+b)^+\}ab^+c\Sigma^*$",
            'docstr' : r"$g((a+b)^+ab^+c)^{|x|=n})^r$",
            'n_range' : (10, 200),
        },
        13 : {
            'rematch' : lambda n: ".*c!x{{{0}{1}}}c.*".format('a'*n, "a?"*n) ,
            're2' : lambda n: "c({0}{1})c".format('a'*n, "a?"*n),
            'doc' : lambda n, r : "c{}c".format('a'*2*n) * r,
            'regexstr' : r"$\Sigma^*c!x\{a^n(a?)^n\}c\Sigma^*$",
            'docstr' : r"$(ca^{2n}c)^r$"
        },
        14 : {
            'rematch' : lambda n: ".*c!x{{(a|b)*b{0}}}c.*".format('a'*n) ,
            're2' : lambda n: "c((?:a|b)*b{0})c".format('a'*n),
            'doc' : lambda n, r : "c{}c".format(gen_all(n) + "b{}".format('a'*n)) * r,
            'regexstr' : r"$\Sigma^*c!x\{(a+b)^*ba^n\}c\Sigma^*$",
            'docstr' : "(c(Lazy De Bruijn)ba^nc)",
            'doc_reps' : (5, 10, 20),
            'n_range' : (1, 20),
            'n_exp' : 1
        },
        15 : {
            'rematch' : lambda n: ".*c!x{a+c}.*",
            're2' : lambda n: "c(a+c)",
            'doc' : lambda n, r : "c{}c".format('a'*n) * r,
            'regexstr' : r"$\Sigma^*c!x\{a^+c\}\Sigma^*$",
            'docstr' : r"$(ca^nc)^r$"
        },
        17 : {
            'rematch' : lambda n: ".*c!x{{{0}{1}}}c.*".format('a'*n, "a?"*n) ,
            're2' : lambda n: "c({0}{1})c".format('a'*n, "a?"*n),
            'doc' : lambda n, r : "c{}c".format('a'*floor(3/2*n)) * r,
            'regexstr' : r"$\Sigma^*c!x\{a^n(a?)^n\}c\Sigma^*$",
            'docstr' : r"$(ca^{3/2n}c)^r$"
        },
        16 : {
            'rematch' : lambda n: ".*c!x{(a|b)*b(a|b)*}c.*" ,
            're2' : lambda n: "c((?:a|b)*b(?:a|b)*)c",
            'doc' : lambda n, r : "c{}c".format(gen_all(n)+'b'+gen_all(n)) * r,
            'regexstr' : r"$\Sigma^*c!x\{(a+b)^*b(a+b)^*\}c\Sigma^*$",
            'docstr' : "(c(Lazy De Bruijn)b(Lazy De Bruijn)c)",
            'doc_reps' : (5, 10, 20),
            'n_range' : (1, 20),
            'n_exp' : 1
        },
        18 : {
            'rematch' : lambda n: "!x{a+}",
            're2' : lambda n: "(a+)",
            'doc' : lambda n, r : "\n".join('a'*n for _ in range(r)),
            'regexstr' : r"$\Sigma^*c!x\{a^+\}c\Sigma^*$",
            'docstr' : r"$[a^n]^r$"
        },
    },
    '2vars' : {
      1 : {
            'rematch' : lambda n: ".*c!x{{{0}}}!y{{{1}}}c.*".format('a'*n, 'b'*n) ,
            're2' : lambda n: "c({0})({1})c".format('a'*n, 'b'*n),
            'doc' : lambda n, r : "c{}c".format('a'*n + 'b'*n) * r,
            'regexstr' : r"$\Sigma^*c!x\{a^n\}!y\{b^n\}c\Sigma^*$",
            'docstr' : r"$(ca^nb^nc)^r$",

      },
      2 : {
            'rematch' : lambda n: ".*c!x{a+}!y{b+}c.*" ,
            're2' : lambda n: "c(a+)(b+)c",
            'doc' : lambda n, r : gen_uniform("ca+b+c", n, r),
            'regexstr' : r"$\Sigma^*c!x\{a^+\}!y\{b^+\}c\Sigma^*$",
            'docstr' : r"$((ca^+b^+c)^{|x|=n})^r$",
            'n_range' : (5, 200),
      },
      3 : {
            'rematch' : lambda n: ".*c!x{(a|b)+}a!y{b+}c.*" ,
            're2' : lambda n: "c((?:a|b)+)a(b+)c",
            'doc' : lambda n, r : gen_uniform("c(a|b)+ab+c", n, r),
            'regexstr' : r"$\Sigma^*c!x\{(a+b)^+\}a!y\{b^+\}c\Sigma^*$",
            'docstr' : r"$((c(a+b)^+ab^+c)^{|x|=n})^r$",
            'n_range' : (6, 200),
      },
      4 : {
            'rematch' : lambda n: ".*c(a|b)+a!x{b+}a!y{b+}c.*" ,
            're2' : lambda n: "c(?:a|b)+a(b+)a(b+)c",
            'doc' : lambda n, r : gen_uniform("c(a|b)+ab+ab+c", n, r),
            'regexstr' : r"$\Sigma^*c(a+b)^+a!x\{b^+\}a!y\{b^+\}c\Sigma^*$",
            'docstr' : r"$((c(a+b)^+ab^+ab^+c)^{|x|=n})^r$",
            'n_range' : (8, 200),
      },

    },
    '3vars' : {
      1 : {
            'rematch' : lambda n: ".*c!x{{{0}}}!y{{{1}}}!z{{{0}}}c.*".format('a'*n, 'b'*n) ,
            're2' : lambda n: "c({0})({1})({0})c".format('a'*n, 'b'*n),
            'doc' : lambda n, r : "c{}c".format('a'*n + 'b'*n + 'a'*n) * r,
            'regexstr' : r"$\Sigma^*c!x\{a^n\}!y\{b^n\}!z\{a^n\}c\Sigma^*$",
            'docstr' : r"$(ca^nb^na^nc)^r$",

      },
      2 : {
            'rematch' : lambda n: ".*c!x{a+}!y{b+}!z{a+}c.*" ,
            're2' : lambda n: "c(a+)(b+)(a+)c",
            'doc' : lambda n, r : gen_uniform("ca+b+a+c", n, r),
            'regexstr' : r"$\Sigma^*c!x\{a^+\}!y\{b^+\}!z\{a^+\}c\Sigma^*$",
            'docstr' : r"$((ca^+b^+a^+c)^{|x|=n})^r$",
            'n_range' : (6, 200),
      },
      3 : {
            'rematch' : lambda n: ".*c!x{(a|b)+}a!y{b+}a!z{b+}c.*" ,
            're2' : lambda n: "c((?:a|b)+)a(b+)a(b+)c",
            'doc' : lambda n, r : gen_uniform("c(a|b)+ab+ab+c", n, r),
            'regexstr' : r"$\Sigma^*c!x\{(a+b)^+\}a!y\{b^+\}a!z\{b^+\}c\Sigma^*$",
            'docstr' : r"$((c(a+b)^+ab^+ab^+c)^{|x|=n})^r$",
            'n_range' : (7, 200),
      },
      4 : {
            'rematch' : lambda n: ".*c(a|b)+a!x{b+}a!y{b+}a!z{b+}c.*" ,
            're2' : lambda n: "c(?:a|b)+a(b+)a(b+)a(b+)c",
            'doc' : lambda n, r : gen_uniform("c(a|b)+ab+ab+ab+c", n, r),
            'regexstr' : r"$\Sigma^*c(a+b)^+a!x\{b^+\}a!y\{b^+\}a!z\{b^+\}c\Sigma^*$",
            'docstr' : r"$((c(a+b)^+ab^+ab^+ab^+c)^{|x|=n})^r$",
            'n_range' : (9, 200),
      },

    },
    '4vars' : {
      1 : {
            'rematch' : lambda n: ".*c!x{{{0}}}!y{{{1}}}!z{{{0}}}!w{{{1}}}c.*".format('a'*n, 'b'*n) ,
            're2' : lambda n: "c({0})({1})({0})({1})c".format('a'*n, 'b'*n),
            'doc' : lambda n, r : "c{}c".format('a'*n + 'b'*n + 'a'*n + 'b'*n) * r,
            'regexstr' : r"$\Sigma^*c!x\{a^n\}!y\{b^n\}!z\{a^n\}!w\{b^n\}c\Sigma^*$",
            'docstr' : r"$(ca^nb^na^nb^nc)^r$",

      },
      2 : {
            'rematch' : lambda n: ".*c!x{a+}!y{b+}!z{a+}!w{b+}c.*" ,
            're2' : lambda n: "c(a+)(b+)(a+)(b+)c",
            'doc' : lambda n, r : gen_uniform("ca+b+a+b+c", n, r),
            'regexstr' : r"$\Sigma^*c!x\{a^+\}!y\{b^+\}!z\{a^+\}!w\{b^+\}c\Sigma^*$",
            'docstr' : r"$((ca^+b^+a^+b^+c)^{|x|=n})^r$",
            'n_range' : (7, 200),
      },
      3 : {
            'rematch' : lambda n: ".*c!x{(a|b)+}a!y{b+}a!z{b+}a!w{b+}c.*" ,
            're2' : lambda n: "c((?:a|b)+)a(b+)a(b+)a(b+)c",
            'doc' : lambda n, r : gen_uniform("c(a|b)+ab+ab+ab+c", n, r),
            'regexstr' : r"$\Sigma^*c!x\{(a+b)^+\}a!y\{b^+\}a!z\{b^+\}a!w\{b^+\}c\Sigma^*$",
            'docstr' : r"$((c(a+b)^+ab^+ab^+ab^+c)^{|x|=n})^r$",
            'n_range' : (9, 200),
      },
      4 : {
            'rematch' : lambda n: ".*c(a|b)+a!x{b+}a!y{b+}a!z{b+}a!w{b+}c.*" ,
            're2' : lambda n: "c(?:a|b)+a(b+)a(b+)a(b+)a(b+)c",
            'doc' : lambda n, r : gen_uniform("c(a|b)+ab+ab+ab+ab+c", n, r),
            'regexstr' : r"$\Sigma^*c(a+b)^+a!x\{b^+\}a!y\{b^+\}a!z\{b^+\}a!w\{b^+\}c\Sigma^*$",
            'docstr' : r"$((c(a+b)^+ab^+ab^+ab^+ab^+c)^{|x|=n})^r$",
            'n_range' : (11, 200),
      },
    },

    '5vars' : {
      1 : {
            'rematch' : lambda n: ".*c!x{{{0}}}!y{{{1}}}!z{{{0}}}!w{{{1}}}!v{{{0}}}c.*".format('a'*n, 'b'*n) ,
            're2' : lambda n: "c({0})({1})({0})({1})({0})c".format('a'*n, 'b'*n),
            'doc' : lambda n, r : "c{}c".format('a'*n + 'b'*n + 'a'*n + 'b'*n + 'a'*n) * r,
            'regexstr' : r"$\Sigma^*c!x\{a^n\}!y\{b^n\}!z\{a^n\}!w\{b^n\}!v\{a^n\}c\Sigma^*$",
            'docstr' : r"$(ca^nb^na^nb^na^nc)^r$",

      },
      2 : {
            'rematch' : lambda n: ".*c!x{a+}!y{b+}!z{a+}!w{b+}!v{a+}c.*" ,
            're2' : lambda n: "c(a+)(b+)(a+)(b+)(a+)c",
            'doc' : lambda n, r : gen_uniform("ca+b+a+b+a+c", n, r),
            'regexstr' : r"$\Sigma^*c!x\{a^+\}!y\{b^+\}!z\{a^+\}!w\{b^+\}!v\{a^+\}c\Sigma^*$",
            'docstr' : r"$((ca^+b^+a^+b^+a^+c)^{|x|=n})^r$",
            'n_range' : (8, 200),
      },
      3 : {
            'rematch' : lambda n: ".*c!x{(a|b)+}a!y{b+}a!z{b+}a!w{b+}c.*" ,
            're2' : lambda n: "c((?:a|b)+)a(b+)a(b+)a(b+)c",
            'doc' : lambda n, r : gen_uniform("c(a|b)+ab+ab+ab+c", n, r),
            'regexstr' : r"$\Sigma^*c!x\{(a+b)^+\}a!y\{b^+\}a!z\{b^+\}a!w\{b^+\}c\Sigma^*$",
            'docstr' : r"$((c(a+b)^+ab^+ab^+ab^+c)^{|x|=n})^r$",
            'n_range' : (9, 200),
      },
      4 : {
            'rematch' : lambda n: ".*c(a|b)+a!x{b+}a!y{b+}a!z{b+}a!w{b+}c.*" ,
            're2' : lambda n: "c(?:a|b)+a(b+)a(b+)a(b+)a(b+)c",
            'doc' : lambda n, r : gen_uniform("c(a|b)+ab+ab+ab+ab+c", n, r),
            'regexstr' : r"$\Sigma^*c(a+b)^+a!x\{b^+\}a!y\{b^+\}a!z\{b^+\}a!w\{b^+\}c\Sigma^*$",
            'docstr' : r"$((c(a+b)^+ab^+ab^+ab^+ab^+c)^{|x|=n})^r$",
            'n_range' : (11, 200),
      },

    },
}

def run_sim():
    # Run dummy tests to measure while hot
    gen_doc("1var", 1, 400, 400)
    gen_rgxs("1var", 1, 400)
    for _ in range(100):
      run_exp("1var", 1, "rematch", 400, 400)

    for expclass, d1 in FILES.items():
        if expclass not in EXPCLASSES_TO_RUN:
            continue
        for nexp, d2 in d1.items():
            if nexp not in EXPS_TO_RUN:
                continue
            aux = FILES[expclass][nexp].get('n_range')
            n_range = aux if aux else N_RANGE

            aux = FILES[expclass][nexp].get('n_exp')
            n_exp = aux if aux else N_EXP

            aux = FILES[expclass][nexp].get('doc_reps')
            doc_reps = aux if aux else DOC_REPS

            Path(HOME_DIR + "/exp/syn/{0}/exp{1:02d}".format(expclass, nexp)).mkdir(parents=True, exist_ok=True)
            # fig, axs = plt.subplots(ceil(len(doc_reps)/2), 2, figsize=(10, 5*ceil(len(doc_reps)/2)))
            fig, axs = plt.subplots(3, 2, figsize=(10, 5*3))
            data = {
                "rematch": dict(),
                "re2" : dict()
            }
            for i, reps in enumerate(doc_reps):
                data_n = np.unique(np.linspace(*n_range, dtype=int, num = 10))
                dlen = len(data_n)
                for system in SYSTEMS:
                    data[system]['t'] = [[] for _ in range(dlen)]
                    data[system]['m'] = np.zeros(dlen)
                    data[system]['o'] = np.zeros(dlen)
                    data[system]['e'] = np.zeros(dlen)
                for j, n in enumerate(data_n):
                    gen_doc(expclass, nexp, n, reps)
                    gen_rgxs(expclass, nexp, n)
                    print("r = {}; n = {}".format(reps, n))
                    for system in SYSTEMS:
                          t, m, o = run_exp(expclass, nexp, system, reps, n)
                          data[system]['t'][j].append(t)
                          data[system]['m'][j] = m
                          data[system]['o'][j] = o
                          print("\t{} outputs = {}".format(system, o))
                    assert data["rematch"]['o'][j] == data["re2"]['o'][j]
                    for _ in range(n_exp-1):
                      for system in SYSTEMS:
                          t, m, o = run_exp(expclass, nexp, system, reps, n)
                          data[system]['t'][j].append(t)
                    for system in SYSTEMS:
                      # print(data[system]['t'][j])s
                      t = np.mean(data[system]['t'][j])
                      s = np.std(data[system]['t'][j], ddof=1 if n_exp > 1 else 0)
                      data[system]['e'][j] = conf_interval(0.95, n_exp, s)
                      data[system]['t'][j] = t
                      print("\n\t{} time = {}ms".format(system, t))
                      #- variabilidad: mediana, mediana + correr mismo sistema en seguido - experimento 8: optimizacion se re2 importante (estados especiales que hace loop sobre cualquier cosa en si mismo) - resumen de tres optimizaciones de re2: links en github - quizas todos nos ganan por el mismo tema de exp8, o pasar al NFA, y no determinizar

                if len(doc_reps) > 2:
                  caxs = axs[i//2, i%2]
                else:
                  caxs = axs[i]
                caxs.set_xlabel('n')
                caxs.set_ylabel('t(ms)')
                caxs.set_title('r={}'.format(reps))

                for system in SYSTEMS:
                    caxs.errorbar(data_n, data[system]['t'], yerr = data[system]['e'],label=system, marker=MARKERS[system])
                caxs.legend()
                caxs.set
            algo = get_algorithm_used(expclass, nexp, doc_reps[-1], n_range[-1])
            regexstr = d2['regexstr']
            docstr = d2['docstr']
            fig.suptitle(r"$\bf{{regex}}$: {0}".format(regexstr) + '\n'
                        + r"$\bf{{document}}$: {0}".format(docstr) + '\n' +
                        r"$\bf{{re2 algorithm}}$: {0}".format(algo), fontsize=16)
            fig.savefig(HOME_DIR + "/exp/syn/{0}/exp{1:02d}/plot.png".format(expclass, nexp))
            # plt.show()

def get_algorithm_used(expclass, nexp, reps, n):
    comm = "{0}/bin/re2-algo {0}/datasets/syn/{1}/exp{2:02d}/r{3}/n{4}.txt {0}/exp/syn/{1}/exp{2:02d}/re2.rgx"
    true_comm = comm.format(HOME_DIR, expclass, nexp, reps, n)
    print(true_comm)
    process = subprocess.run(true_comm, shell=True, check=True, capture_output=True, universal_newlines=True)
    return str(process.stdout)

def run_exp(expclass, nexp, system, reps, n):
    '''Run experiments assuming the filenames doc.txt, rematch.rgx and re2.rgx'''
    # sleep(SLEEP)

    # time_command = "/usr/bin/time -f \"%e\\n%M\""
    time_command = "millisecond-time"

    comm = time_command + ' ' + COMMANDS[system]
    true_comm = comm.format(HOME_DIR, expclass, nexp, reps, n)

    # print(true_comm)

    process = subprocess.run(true_comm, shell=True, check=True, capture_output=True, universal_newlines=True)
    output = process.stderr
    o = process.stdout

    # print(output)
    # t, m, _ = output.replace('"', '').split('\n')
    t, _ = output.replace('"', '').split('\n')
    o = int(o)
    # m = int(m)
    m = 0
    t = int(t)
    return  t, m, o



def gen_doc(expclass, nexp, n, r, rgx=''):
    pathdir = HOME_DIR + "/datasets/syn/{0}/exp{1:02d}/r{2}".format(expclass, nexp, r)
    pathobj = Path(pathdir)
    pathobj.mkdir(parents=True, exist_ok=True)
    filepath = pathdir + '/n{}.txt'.format(n)
    if Path(filepath).exists():
      return
    with open(filepath, 'w') as fp:
      data = FILES[expclass][nexp]
      fp.write(data['doc'](n, r))

def gen_rgxs(expclass, nexp, n):
    for system in SYSTEMS:
        filepath = HOME_DIR + "/exp/syn/{0}/exp{1:02d}/{2}.rgx".format(expclass, nexp, system)
        with open(filepath, 'w') as fp:
            fp.write(FILES[expclass][nexp][system](n))

def gen_uniform(regex, n, r):
    command = HOME_DIR + "/bin/resample -e \"{}\" -n {}".format(regex, n)
    ret = ''
    for _ in range(r):
      process = subprocess.run(command, shell=True, check=True, capture_output=True, universal_newlines=True)
      ret += process.stdout[:-1]
    return ret

def gen_all(n):
  return "".join("".join(item) for item in itertools.product(["a","b"], repeat=n))

def write_latex():
  doc = Document(default_filepath="REmatch_exps_summary", geometry_options="landscape, margin=2pt ")
  rootdir = os.path.join(HOME_DIR, "exp/syn")

  def iter_pics():
    for subdir in sorted(os.listdir(rootdir)):
      for nexp in sorted(os.listdir(os.path.join(rootdir, subdir))):
        yield os.path.join(rootdir, subdir, nexp, "plot.png")

  pics = iter_pics()
  pic_path = next(pics, None)
  while(pic_path):
    with doc.create(Figure(position='h!')) as graph_pic:
      graph_pic.add_image(pic_path, width=NoEscape(r'0.5\textwidth'))
      pic_path = next(pics, None)
      if pic_path:
        graph_pic.add_image(pic_path, width=NoEscape(r'0.5\textwidth'))
        pic_path = next(pics, None)

  doc.generate_pdf(filepath="/home/nicovsj/Drive/Personal/Universidad/Magíster/Tesis/Docs/REmatch-exps")



if __name__ == "__main__":
    run_sim()
    write_latex()
