import json
import os
import os.path
from datetime import datetime
import signal
import time
from typing import List
import matplotlib.pyplot as plt

import pandas as pd
from collections import namedtuple

import subprocess

import re

AutomataStats = namedtuple("AutomataStats", "extended_va_size extended_det_va_size search_nfa_size search_dfa_size nodes_allocated nodes_used nodes_reused")
DocStats = namedtuple("DocStats", "filesize nchars nlines")

HERE = os.path.dirname(os.path.realpath(__file__))

with open(os.path.join(HERE, "config/benchmark-config.json")) as jsonFile:
    BENCHMARKS = json.load(jsonFile)

with open(os.path.join(HERE, "config/exp-run.json")) as jsonFile:
    run_configuration = json.load(jsonFile)

PROJECT_DIR: str = run_configuration["projectDir"]
BINARIES_DIR: str = run_configuration["binariesDir"].format(projectDir=PROJECT_DIR)
NUMBER_OF_RUNS: int = run_configuration["numberOfRuns"]
TIMEOUT_S: int = run_configuration["timeoutInSeconds"]

USE_SMALL_SAMPLE: bool = run_configuration["useSmallSample"]

CHOSEN_BENCHMARK: str = run_configuration["chosenBenchmark"]
CHOSEN_EXPERIMENTS: List[str] = run_configuration["experimentsToRun"]
EXCLUDE_BINARIES: List[str] = run_configuration["excludeBinaries"]


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


def get_boxplots(
    dfs: List[pd.DataFrame],
    results_dir: str,
    chosen_binaries: List[str],
    benchmark: str,
    experiment: str,
):
    T, M, O, F, P = dfs

    measures = {
        "Time": {"data": T[[binary for binary in chosen_binaries]], "units": "s"},
        "Outputs": {"data": O[[binary for binary in chosen_binaries]], "units": ""},
        "Memory": {"data": M[[binary for binary in chosen_binaries]], "units": "B"},
        "FOT": {"data": F[[binary for binary in chosen_binaries]], "units": "ms"},
        "Throughput": {
            "data": P[[binary for binary in chosen_binaries]],
            "units": "ms",
        },
    }
    for (k, v) in measures.items():
        fig_mem = plt.figure()
        v["data"].boxplot()
        plt.title(f"{benchmark}: {experiment} sample: {k}")
        plt.ylabel(v["units"])
        filename = os.path.join(results_dir, f"{k.lower()}.png")
        print(f"Saving figure: {filename}")
        fig_mem.savefig(filename)
        plt.close(fig_mem)


def gen_csvs(dfs: List[pd.DataFrame], results_dir: str):
    T, M, O, F, P = dfs

    filenames_map = {"time": T, "memory": M, "outputs": O, "fot": F, "throughput": P}

    for k, v in filenames_map.items():
        filename = os.path.join(results_dir, f"{k}.csv")
        print(f"Saving csv: {filename}")
        v.to_csv(filename, index=False)


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
    command = [
        os.path.join(BINARIES_DIR, "rematch_stats"),
        doc_path,
        rgx_path
    ]
    try:
        process = subprocess.run(
            " ".join(command),
            shell=True,
            check=True,
            capture_output=True,
            universal_newlines=True,
        )
    except Exception as err:
        print(f"Error while processing command: {command}")
        print(err)
        return AutomataStats("err", "err", "err", "err", "err", "err", "err")

    patterns = [
        r'ExtendedVA: (\d+)',
        r'ExtendedDetVA: (\d+)',
        r'SearchNFA: (\d+)',
        r'SearchDFA: (\d+)',
        r'Nodes allocated: (\d+)',
        r'Nodes used: (\d+)',
        r'Nodes reused: (\d+)',
    ]

    stats = [int(re.search(pattern, process.stdout).group(1)) for pattern in patterns]

    return AutomataStats(*stats)


def run_bench(binary_data, document_path, regex_path, n_runs, versions_binary=False):
    mem_command = "/usr/bin/time -v {0}"
    time_command = "{0}"

    args = binary_data["args"].format(document=document_path, regex=regex_path)

    bin_path = os.path.join(BINARIES_DIR, binary_data["command"])

    subcommand = "{binary_path} {args}".format(binary_path=bin_path, args=args)

    command_mem = mem_command.format(subcommand)
    command_time = time_command.format(subcommand)

    tot_time = 0

    try:
        elapsed_time = time.perf_counter()
        process = subprocess.Popen(
            command_mem.split(" "),
            text=True,
            shell=False,
            start_new_session=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
        )

        stdout, stderr = process.communicate(timeout=TIMEOUT_S)
        elapsed_time = time.perf_counter() - elapsed_time

    except subprocess.TimeoutExpired:
        print(f"Timeout with command: {command_mem}")
        os.killpg(os.getpgid(process.pid), signal.SIGTERM)
        return "timeout", "timeout", "timeout", "timeout"

    if not stdout:
        print("no output")
        raise RuntimeError(f"No output from command: {command_mem}")

    pattern = r"Maximum resident set size \(kbytes\): (\d+)"

    # Use re.search to find the match
    match = re.search(pattern, stderr)

    # Extract the maximum resident set size value
    memory = int(match.group(1))

    n_outputs = int(stdout)
    fot = 0.0

    tot_time += elapsed_time

    # mem = format_memory(mem)
    print(f"\t\t\t\tNumber of outputs: {n_outputs}")
    print(f"\t\t\t\tMemory used: {memory}")
    print(f"\t\t\t\tFirst output time: {fot}ms")

    for _ in range(n_runs - 1):
        process = subprocess.run(
            command_time,
            shell=True,
            check=True,
            capture_output=True,
            universal_newlines=True,
        )
        stdout = process.stdout
        elapsed_time = float(
            re.search(r"Total time\s+((?:[0-9,]*[.])?[0-9,]+)", stdout)
            .group(1)
            .replace(",", "")
        )
        # print("\tIteration {0} -> t={1:.2f}ms".format(i+2, float(t)))
        tot_time += float(elapsed_time)

    elapsed_time = tot_time / n_runs
    elapsed_time = round(elapsed_time, 2)
    print(f"\t\t\t\tAvg time: {elapsed_time}ms")

    return elapsed_time, memory, n_outputs, fot


def run_experiments(benchmark: str):
    # libraries-lookahead en el json
    benchmark_data = BENCHMARKS["benchmarks"][benchmark]

    print(f"Running experiments for benchmark: {benchmark}\n")

    # binarySets: regex-libraries en json
    binaries = BENCHMARKS["binarySets"][benchmark_data["binarySet"]]
    chosen_binaries = {k: v for k, v in binaries.items() if k not in EXCLUDE_BINARIES}

    print(f"Using binaries: {list(chosen_binaries.keys())}\n")

    for suite in CHOSEN_EXPERIMENTS:
        print(f"\tOn dataset: {suite}\n")

        experiment_paths = benchmark_data["experiments"][suite]

        sample_key = "small-sample" if USE_SMALL_SAMPLE else "large-sample"
        sample_dir = os.path.join(PROJECT_DIR, experiment_paths[sample_key])

        now = datetime.now()
        timestamp = now.strftime("%Y-%m-%d_%H:%M:%S")
        results_dir = os.path.join(
            PROJECT_DIR, experiment_paths["results"], benchmark, timestamp
        )
        os.makedirs(results_dir, exist_ok=True)
        print(f"\tStoring at: {results_dir}\n")

        document_path = os.path.join(PROJECT_DIR, experiment_paths["document"])

        time_df = pd.DataFrame()
        fot_df = pd.DataFrame()
        mem_df = pd.DataFrame()
        out_df = pd.DataFrame()
        tpo_df = pd.DataFrame()
        row_count = 0

        for root, dirs, files in os.walk(sample_dir, followlinks=True):
          for dataset in sorted(dirs):

            for experiment in sorted(
                map(
                    lambda x: x[0],
                    filter(
                        lambda x: bool(x[2]), os.walk(os.path.join(sample_dir, dataset))
                    ),
                )
            ):
                row = dict()
                row_t = dict()
                row_m = dict()
                row_o = dict()
                row_fot = dict()
                row_tpo = dict()

                print("\t\tOn experiment:", experiment)

                rem_rgx_path = os.path.join(experiment, "rematch.rgx")

                row["query"] = get_regex(rem_rgx_path)

                print(f"\n\t\tStarting query: {row['query']:50.50}...")

                print("\t\tCalculating docstats...")

                dstats = docstats(document_path)

                row["filesize"] = dstats.filesize
                row["nchars"] = dstats.nchars
                row["nlines"] = dstats.nlines

                print("\t\tDocstats calculated")

                astats = automata_stats(document_path, rem_rgx_path)

                row["size(eVA)"] = astats.extended_va_size
                row["size(d-eVA)"] = astats.extended_det_va_size
                row["size(SearchDFA)"] = astats.search_dfa_size
                row["size(SearchNFA)"] = astats.search_nfa_size
                row["nodes_allocated"] = astats.nodes_allocated
                row["nodes_used"] = astats.nodes_used
                row["nodes_reused"] = astats.nodes_reused

                for bin, bin_data in chosen_binaries.items():
                    print("\n\t\t\tTesting lib:", bin)

                    regex_path = os.path.join(experiment, f"{bin_data['rgx_type']}.rgx")

                    try:
                        t, memory, noutputs, foutt = run_bench(
                            bin_data,
                            document_path,
                            regex_path,
                            NUMBER_OF_RUNS,
                            benchmark_data["binarySet"] == "rematch-versions",
                        )
                    except RuntimeError:
                        print("runtime error")
                        t, memory, noutputs, foutt = 0, 0, 0, 0

                    row_t[f"{bin}"] = t
                    row_m[f"{bin}"] = memory
                    row_o[f"{bin}"] = noutputs
                    row_fot[f"{bin}"] = foutt
                    try:
                        row_tpo[f"{bin}"] = (noutputs + dstats.nchars) / t if t != 0 else 0
                    except:
                        row_tpo[f"{bin}"] = t

                row_t = row | row_t
                row_m = row | row_m
                row_o = row | row_o
                row_fot = row | row_fot
                row_tpo = row | row_tpo

                time_df = pd.concat([time_df, pd.DataFrame(row_t, index=[row_count])])
                mem_df = pd.concat([mem_df, pd.DataFrame(row_m, index=[row_count])])
                out_df = pd.concat([out_df, pd.DataFrame(row_o, index=[row_count])])
                fot_df = pd.concat([fot_df, pd.DataFrame(row_fot, index=[row_count])])
                tpo_df = pd.concat([tpo_df, pd.DataFrame(row_tpo, index=[row_count])])

                try:
                    get_boxplots(
                        dfs=[time_df, mem_df, out_df, fot_df, tpo_df],
                        results_dir=results_dir,
                        chosen_binaries=list(chosen_binaries.keys()),
                        benchmark=benchmark,
                        experiment=suite,
                    )
                except:
                    pass

                gen_csvs(
                    dfs=[time_df, mem_df, out_df, fot_df, tpo_df],
                    results_dir=results_dir,
                )

                row_count += 1


if __name__ == "__main__":
    run_experiments(CHOSEN_BENCHMARK)
