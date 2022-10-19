import json
import os
import os.path as pth
from datetime import date
import signal

import pandas as pd
from collections import namedtuple

import subprocess

import re

AutomataStats = namedtuple("AutomataStats", "dsize esize msize nsegs ambig")
DocStats = namedtuple("DocStats", "filesize nchars nlines")

HERE = pth.dirname(pth.realpath(__file__))
CONFIG_FILE_PATH = pth.join(HERE, "config2.json")

with open(CONFIG_FILE_PATH) as jsonFile:
	data = json.load(jsonFile)

HOME_DIR = data['homedir']
EXP_SUBPATH = data['exp-subpath']
NEXP = data['nexp']
BINARIES = data['binaries']
TIMEOUT_S = data['timeout-s']

def format_memory(size_in_kb: int):
    units = ["K", "M", "G"]
    counter = 0

    while size_in_kb > 1024:
        size_in_kb /= 1024
        counter += 1
    return f"{round(size_in_kb, 2)}{units[counter]}"


def get_regex(rgx_path):
    with open(rgx_path, "r") as file_path:
        rgx = file_path.read()
    return rgx


def docstats(doc_path: str):
    command = f'wc -ml "$(readlink -f "{doc_path}")" | awk \'{{print $1, $2}}\''
    process = subprocess.run(
        command, shell=True, check=True, capture_output=True, universal_newlines=True
    )
    nlines, nchars = process.stdout.split()
    command = 'du -h "$(readlink -f "{0}")" | awk \'{{print $1}}\''.format(doc_path)
    process = subprocess.run(
        command, shell=True, check=True, capture_output=True, universal_newlines=True
    )
    filesize = process.stdout.replace("\n", "")

    return DocStats(filesize, int(nchars), int(nlines) + 1)


def automata_stats(doc_path: str, rgx_path: str):
	command = [f"{HOME_DIR}/build/Release/bin/rematch-4",
             "--searching",
            #  "--macrodfa",
             "--mode=benchmark",
             f"--document={doc_path}",
             f"--regex-file={rgx_path}"]
	try:
		process = subprocess.run(' '.join(command), shell=True, check=True, capture_output=True, universal_newlines=True)
	except:
		print(f"Error while processing command: {command}")
		return AutomataStats('err', 'err', 'err', 'err', 'err')

    dsize = int(re.search(r"DetSize\s+(\d+)", process.stdout).group(1))
    esize = int(re.search(r"eVASize\s+(\d+)", process.stdout).group(1))
    msize = int(re.search(r"MDFASize\s+(\d+)", process.stdout).group(1))
    nsegs = int(re.search(r"Number of segments\s+(\d+)", process.stdout).group(1))
    ambig = int(re.search(r"Ambiguous\?\s+(\d)", process.stdout).group(1))

    return AutomataStats(dsize, esize, msize, nsegs, ambig)


def run_outputs(binary, doc_path, rgx_path):
    data = BINARIES[binary]
    args = data["args"].format(doc_path, rgx_path)

    bin_path = data["command"].format(HOME_DIR)

    subcommand = "{0} {1}".format(bin_path, args)

    try:
        process = subprocess.Popen(
            subcommand.split(" "),
            text=True,
            shell=False,
            start_new_session=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
        )
        nout, output = process.communicate(timeout=TIMEOUT_S)
		nout = int(re.search(r'Number of mappings\s+(\d+)', nout).group(1))
    except subprocess.TimeoutExpired:
        print(f"Timeout with command: {subcommand}")
        os.killpg(os.getpgid(process.pid), signal.SIGTERM)
        return "timeout", "timeout", "timeout"
    except:
        print(f"Error while processing command: {subcommand}")
        return "err", "err", "err"


def run_bench(binary, doc_path, rgx_path, nexp):
    mem_command = '/usr/bin/time -f "%M" {0}'
    time_command = "millisecond-time {0}"

    data = BINARIES[binary]
    args = data["args"].format(doc_path, rgx_path)

    bin_path = data["command"].format(HOME_DIR)

    subcommand = "{0} {1}".format(bin_path, args)

    command_mem = mem_command.format(subcommand)
    command_time = time_command.format(subcommand)

    tot_time = 0

    try:
        process = subprocess.Popen(
            command_mem.split(" "),
            text=True,
            shell=False,
            start_new_session=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
        )

        out, output = process.communicate(timeout=TIMEOUT_S)

    except subprocess.TimeoutExpired:
        print(f"Timeout with command: {command_mem}")
        os.killpg(os.getpgid(process.pid), signal.SIGTERM)
        return "timeout", "timeout", "timeout"

    except:
        print(f"Error while processing command: {command_mem}")
        return "err", "err", "err"

    mem = int(output.replace('"', "").replace("\n", ""))
    nout = int(re.search(r'Number of mappings\s+(\d+)', out).group(1))
	foutt = float(re.search(r'First output time\s+((?:[0-9,]*[.])?[0-9,]+)', out).group(1).replace(',',''))
    # mem = format_memory(mem)
    print("    Number of outputs:", nout)
    print("    Memory used:", mem)
	print(f"\tFirst output time: {foutt}ms")

    for _ in range(nexp):
        process = subprocess.run(
            command_time,
            shell=True,
            check=True,
            capture_output=True,
            universal_newlines=True,
        )
        output = process.stderr
        t = output.replace('"', "").replace("\n", "")
        # print("\tIteration {0} -> t={1:.2f}ms".format(i+2, float(t)))
        tot_time += float(t)

    t = tot_time / nexp
    t = round(t, 2)
    print(f"    Avg time: {t}ms")

	return t, mem, nout, foutt


def run_experiments():
    exps_path = pth.join(HOME_DIR, EXP_SUBPATH)

	time_df = pd.DataFrame()
	fot_df = pd.DataFrame()
	mem_df = pd.DataFrame()
	out_df = pd.DataFrame()
	row_count = 0

	for dataset in sorted(next(os.walk(exps_path, followlinks=True))[1]):
		for experiment in sorted(map(lambda x: x[0], filter(lambda x: bool(x[2]), os.walk(os.path.join(exps_path, dataset))))):
			row = dict()
			row_t = dict()
			row_m = dict()
			row_o = dict()
			row_fot = dict()

            print("On experiment:", experiment)

            rem_rgx_path = os.path.join(experiment, "rematch.rgx")

            row["query"] = get_regex(rem_rgx_path)

            print("\nStarting query:", row["query"])

            doc_path = os.path.join(experiment, "doc.txt")

            print("Calculating docstats...")

            dstats = docstats(doc_path)

            row["filesize"] = dstats.filesize
            row["nchars"] = dstats.nchars
            row["nlines"] = dstats.nlines

            print("Docstats calculated")

            astats = automata_stats(doc_path, os.path.join(experiment, "rematch.rgx"))

            row["size(eVA)"] = astats.esize
            row["size(d-eVA)"] = astats.dsize
            row["size(md-eVA)"] = astats.msize
            row["size(segments)"] = astats.nsegs
            row["ambiguous"] = astats.ambig

            for bin, bin_data in BINARIES.items():

                print("\n  Testing lib:", bin)

                rgx_path = os.path.join(experiment, f"{bin_data['rgx_type']}.rgx")

				time, memory, noutputs, foutt = run_bench(bin, doc_path, rgx_path, NEXP)

				row_t[f"{bin}"] = time
				row_m[f"{bin}"] = memory
				row_o[f"{bin}"] = noutputs
				row_fot[f"{bin}"] = foutt

			row_t = row | row_t
			row_m = row | row_m
			row_o = row | row_o
			row_fot = row | row_fot

			time_df = pd.concat([time_df, pd.DataFrame(row_t, index=[row_count])])
			mem_df = pd.concat([mem_df, pd.DataFrame(row_m, index=[row_count])])
			out_df = pd.concat([out_df, pd.DataFrame(row_o, index=[row_count])])
			fot_df = pd.concat([fot_df, pd.DataFrame(row_fot, index=[row_count])])

			time_df.to_csv(pth.join(HERE, 'time.csv'), index=False)
			mem_df.to_csv(pth.join(HERE, 'mem.csv'), index=False)
			out_df.to_csv(pth.join(HERE, 'outputs.csv'), index=False)
			fot_df.to_csv(pth.join(HERE, 'foutt.csv'), index=False)

            row_count += 1

			

	return time_df, mem_df, out_df, fot_df

if __name__ == '__main__':
	T, M, O, F = run_experiments()