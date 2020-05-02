# Deploys the experiment
import matplotlib.pyplot as plt
import numpy as np
import subprocess
from math import ceil
from pathlib import Path
from random import choice
from string import ascii_lowercase
from time import sleep

HOME_DIR = "/home/nicovsj/repos/research/SpannersConst"

SYSTEMS = ["rematch", "re2"]
EXP_CLASSES = ["1var"]

COMMANDS = {
    "rematch" :
        "{0}/bin/rematch -o nmappings -d {0}/exp/syn/{1}/{2}/doc.txt -r {0}/exp/syn/{1}/{2}/rematch.rgx",
    "re2" :
        "{0}/bin/re2test {0}/exp/syn/{1}/{2}/doc.txt {0}/exp/syn/{1}/{2}/re2.rgx"
}

MARKERS = {
    "rematch" : "o",
    "re2"     : "^"
}

DOC_REPS = (10, 25, 50, 100, 200, 400)
N_RANGE = (1, 1000)
EXPS_TO_RUN = (8,)
N_EXP = 60

def randstr(chars, n):
    ret = ""
    for _ in range(n):
        ret += choice(chars)
    return ret

FILES = {
    '1var' : {
        'exp1' : {
            'rematch' : lambda n: ".*c!x{{{0}}}c.*".format('a'*n) ,
            're2' : lambda n: "c({0})c".format('a'*n),
            'doc' : lambda n, r : "c{}c".format('a'*n) * r,
            'regexstr' : r"$\Sigma^*c!x\{a^n\}c\Sigma^*$",
            'docstr' : r"$(ca^nc)^r$"
        },
        'exp2' : {
            'rematch' : lambda n: ".*c!x{a+}c.*",
            're2' : lambda n: "c((?:a+)+)c",
            'doc' : lambda n, r : "c{}c".format('a'*n) * r,
            'regexstr' : r"$\Sigma^*c!x\{a^+\}c\Sigma^*$",
            'docstr' : r"$(ca^nc)^r$"
        },
        'exp3' : {
            'rematch' : lambda n: ".*c!x{{{0}{1}}}c.*".format('a'*n, "a?"*n) ,
            're2' : lambda n: "c({0}{1})c".format('a'*n, "a?"*n),
            'doc' : lambda n, r : "c{}c".format('a'*n) * r,
            'regexstr' : r"$\Sigma^*c!x\{a^n(a?)^n\}c\Sigma^*$",
            'docstr' : r"$(ca^nc)^r$"
        },
        'exp4' : {
            'rematch' : lambda n: ".*c!x{{{1}{0}}}c.*".format('a'*n, "a?"*n) ,
            're2' : lambda n: "c({1}{0})c".format('a'*n, "a?"*n),
            'doc' : lambda n, r : "c{}c".format('a'*n) * r,
            'regexstr' : r"$\Sigma^*c!x\{(a?)^na^n\}c\Sigma^*$",
            'docstr' : r"$(ca^nc)^r$"
        },
        'exp5' : {
            'rematch' : lambda n: ".*c!x{{(a|b)*b{0}}}c.*".format('a'*n) ,
            're2' : lambda n: "c((?:a|b)*b{0})c".format('a'*n),
            'doc' : lambda n, r : "c{}".format("{0}b{1}cc".format(randstr(['a', 'b'], 10*n), 'a'*n)*r),
            'regexstr' : r"$\Sigma^*c!x\{(a+b)^*ba^n\}c\Sigma^*$",
            'docstr' : r"$c((a+b)^{10n}ba^ncc)^r$"
        },
        'exp6' : {
            'rematch' : lambda n: ".*c!x{{{0}b(a|b)*}}c.*".format('a'*n) ,
            're2' : lambda n: "c({0}b(?:a|b)*)c".format('a'*n),
            'doc' : lambda n, r : "c{}".format("{1}b{0}cc".format(randstr(['a', 'b'], 10*n), 'a'*n)*r),
            'regexstr' : r"$\Sigma^*c!x\{a^nb(a+b)^*\}c\Sigma^*$",
            'docstr' : r"$c(a^nb(a+b)^{10n}cc)^r$"
        },
        'exp7' : {
            'rematch' : lambda n: ".*c!x{{{0}}}c(a|c)*b.*".format('a'*n) ,
            're2' : lambda n: "c({0})c(?:a|c)*b".format('a'*n),
            'doc' : lambda n, r : "b{0}b".format("c{0}c".format('a'*n)*n)*r,
            'regexstr' : r"$\Sigma^*bc!x\{a^n\}c(a+c)^*b\Sigma^*$",
            'docstr' : r"$(b(c(a^n)c)^nb)^r$"
        },
        'exp8' : {
            'rematch' : lambda n: r".*c!x{[^c]+}c.*",
            're2' : lambda n: "c([^c]+)c",
            'doc' : lambda n, r : "c{}c".format('a'*n) * r,
            'regexstr' : r"$\Sigma^*c!x\{a^n\}c\Sigma^*$",
            'docstr' : r"$(ca^nc)^r$"
        }
    }
}

def run_sim():
    for expclass, d1 in FILES.items():
        for nexp, d2 in d1.items():
            if nexp not in ['exp{}'.format(x) for x in EXPS_TO_RUN]:
                continue
            Path(HOME_DIR + "/exp/syn/{}/{}".format(expclass, nexp)).mkdir(parents=True, exist_ok=True)
            fig, axs = plt.subplots(ceil(len(DOC_REPS)/2), 2, figsize=(10, 5*ceil(len(DOC_REPS)/2)))
            data = {
                "rematch": dict(),
                "re2" : dict()
            }
            for i, reps in enumerate(DOC_REPS):
                data_n = np.unique(np.linspace(*N_RANGE, dtype=int, num = 20))
                dlen = len(data_n)
                for system in SYSTEMS:
                    data[system]['t'] = np.zeros(dlen)
                    data[system]['m'] = np.zeros(dlen)
                    data[system]['o'] = np.zeros(dlen)
                for j, n in enumerate(data_n):
                    gen_doc(expclass, nexp, n, reps)
                    gen_rgxs(expclass, nexp, n)
                    print("r = {}; n = {}".format(reps, n))
                    for system in SYSTEMS:
                          t, m, o = run_exp(expclass, nexp, system)
                          data[system]['t'][j] = t
                          data[system]['m'][j] = m
                          data[system]['o'][j] = o
                    assert data["rematch"]['o'][j] == data["re2"]['o'][j]
                    for _ in range(N_EXP-1):
                      for system in SYSTEMS:
                          t, m, o = run_exp(expclass, nexp, system)
                          data[system]['t'][j] += t
                    for system in SYSTEMS:
                      data[system]['t'][j] /= N_EXP


                caxs = axs[i//2, i%2]
                caxs.set_xlabel('n')
                caxs.set_ylabel('t')
                caxs.set_title('r={}'.format(reps))

                for system in SYSTEMS:
                    caxs.plot(data_n, data[system]['t'], label=system, marker=MARKERS[system])
                caxs.legend()

            regexstr = d2['regexstr']
            docstr = d2['docstr']
            fig.suptitle(r"$\bf{{regex}}$: {0}      $\bf{{document}}$: {1}".format(regexstr, docstr), fontsize=16)
            fig.savefig(HOME_DIR + "/exp/syn/{}/{}/plot.png".format(expclass, nexp))
            # plt.show()



def run_exp(expclass, nexp, system):
    '''Run experiments assuming the filenames doc.txt, rematch.rgx and re2.rgx'''
    # sleep(1)
    # time_command = "/usr/bin/time -f \"%e\\n%M\""
    time_command = "millisecond-time"

    comm = time_command + ' ' + COMMANDS[system]
    true_comm = comm.format(HOME_DIR, expclass, nexp)

    process = subprocess.run(true_comm, shell=True, check=True, capture_output=True, universal_newlines=True)
    output = process.stderr
    o = process.stdout
    # print(output)
    # t, m, _ = output.replace('"', '').split('\n')
    t, _ = output.replace('"', '').split('\n')
    o = int(o)
    # m = int(m)
    m = 0
    t = int(t)/1000


    # for _ in range(N_EXP-1):
    #     process = subprocess.run(true_comm, shell=True, check=True, capture_output=True, universal_newlines=True)
    #     output = process.stderr
    #     o = process.stdout
    #     # t, m, _ = output.replace('"', '').split('\n')
    #     strt, _ = output.replace('"', '').split('\n')
    #     o = int(o)
    #     # m = int(m)
    #     m = 0
    #     t += int(strt)/1000
    #     # sleep(0.1)
    # t /= N_EXP


    print("\tsystem: {}".format(system))
    print('\t\tt:', t, '\n\t\tm:', m, '\n\t\to:', o)
    print()
    # sleep(1)
    return  t, m, o



def gen_doc(expclass, nexp, n, r):
    filepath = HOME_DIR + "/exp/syn/{0}/{1}/doc.txt".format(expclass, nexp)
    with open(filepath, 'w') as fp:
        fp.write(FILES[expclass][nexp]['doc'](n, r))

def gen_rgxs(expclass, nexp, n):
    for system in SYSTEMS:
        filepath = HOME_DIR + "/exp/syn/{0}/{1}/{2}.rgx".format(expclass, nexp, system)
        with open(filepath, 'w') as fp:
            fp.write(FILES[expclass][nexp][system](n))

if __name__ == "__main__":
    run_sim()