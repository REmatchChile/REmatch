import subprocess
from typing import List
import os
import json
from datetime import datetime

HERE = os.path.dirname(os.path.realpath(__file__))

with open(os.path.join(HERE, "config/compare_outputs-config.json")) as jsonFile:
    BENCHMARKS = json.load(jsonFile)

with open(os.path.join(HERE, "config/exp-diff.json")) as jsonFile:
    run_configuration = json.load(jsonFile)

CHOSEN_EXPERIMENTS: List[str] = run_configuration["experimentsToRun"]

CPP_PROJECT_DIR: str = run_configuration["REmatch-cpp"]["projectDir"]
PAPER_PROJECT_DIR: str = run_configuration["REmatch-paper"]["projectDir"]

CPP_BINARIES_DIR = run_configuration["REmatch-cpp"]["binariesDir"]\
    .format(projectDir=CPP_PROJECT_DIR)
PAPER_BINARIES_DIR = run_configuration["REmatch-paper"]["binariesDir"]\
    .format(projectDir=PAPER_PROJECT_DIR)

CPP_BIN_DATA = BENCHMARKS["binarySets"]["regex-libraries"]["REmatch-cpp"]
PAPER_BIN_DATA = BENCHMARKS["binarySets"]["regex-libraries"]["REmatch-paper"]

CHOSEN_BENCHMARK: str = run_configuration["chosenBenchmark"]


def get_regex(rgx_path):
    with open(rgx_path, "r") as file_path:
        rgx = file_path.read()
    return rgx


def print_output_to_file(command, filename):
    with open(filename, "w") as file:
        try:
            process = subprocess.run(
                command.split(" "),
                text=True,
                shell=False,
                start_new_session=True,
                stdout=subprocess.PIPE,
                timeout=run_configuration["timeoutInSeconds"],
            )
            outputs = sorted(process.stdout.splitlines(True))
            file.writelines(outputs)

        except subprocess.TimeoutExpired:
            file.write("timeout\n")
            raise TimeoutError(command)


def find_missing_outputs(file1, file2):
    with open(file1, 'r') as f1, open(file2, 'r') as f2:
        outputs1 = f1.read().splitlines()
        outputs2 = f2.read().splitlines()

    idx1 = 0
    idx2 = 0

    missing_in_file1 = []
    missing_in_file2 = []

    while idx1 < len(outputs1) and idx2 < len(outputs2):

        output_file1 = outputs1[idx1]
        output_file2 = outputs2[idx2]

        if output_file1 > output_file2:
            missing_in_file1.append(output_file2)
            idx2 += 1
        elif output_file2 > output_file1:
            missing_in_file2.append(output_file1)
            idx1 += 1
        else:
            idx1 += 1
            idx2 += 1

    missing_in_file1.extend(outputs2[idx2:])
    missing_in_file2.extend(outputs1[idx1:])

    return missing_in_file1, missing_in_file2


def print_diff_between_files(file1, file2, missing_outputs_file1, missing_outputs_file2):
    if (missing_outputs_file1 or missing_outputs_file2):
        print(f"Outputs are different")
    else:
        print(f"Outputs are identical")

    if missing_outputs_file1:
        print(f"Outputs missing in {file1}:")
        for output in missing_outputs_file1:
            print(f"\t{output}")

    if missing_outputs_file2:
        print(f"Outputs missing in {file2}:")
        for output in missing_outputs_file2:
            print(f"\t{output}")


def check_if_outputs_are_equal(document_path, regex_path, results_dir, exp_id):
    cpp_binary = os.path.join(CPP_BINARIES_DIR, CPP_BIN_DATA["command"])
    paper_binary = os.path.join(PAPER_BINARIES_DIR, PAPER_BIN_DATA["command"])

    cpp_command_template =  f'{cpp_binary} {CPP_BIN_DATA["args"]}'
    paper_command_template = f'{paper_binary} {PAPER_BIN_DATA["args"]}'

    cpp_command = cpp_command_template.format(document=document_path, regex=regex_path)
    paper_command = paper_command_template.format(document=document_path, regex=regex_path)

    cpp_output_file = os.path.join(results_dir, f"{exp_id}-REmatch-cpp.txt")
    paper_output_file = os.path.join(results_dir, f"{exp_id}-REmatch-paper.txt")

    print_output_to_file(cpp_command, cpp_output_file)
    print_output_to_file(paper_command, paper_output_file)
    missing_outputs_cpp, missing_outputs_paper = find_missing_outputs(cpp_output_file, paper_output_file)

    print_diff_between_files(
        cpp_output_file, paper_output_file, missing_outputs_cpp, missing_outputs_paper
    )

    outputs_are_equal = not (missing_outputs_cpp or missing_outputs_paper)

    return outputs_are_equal


def run_experiments(benchmark: str):
    benchmark_data = BENCHMARKS["benchmarks"][benchmark]

    for suite in CHOSEN_EXPERIMENTS:
        print(f"\tOn dataset: {suite}\n")

        experiment_paths = benchmark_data["experiments"][suite]
        sample_dir = os.path.join(PAPER_PROJECT_DIR, experiment_paths["small-sample"])

        timestamp = datetime.now().strftime("%Y-%m-%d_%H:%M:%S")

        results_dir = os.path.join(
            CPP_PROJECT_DIR, experiment_paths["results"], benchmark + "-outputs", timestamp
        )
        os.makedirs(results_dir, exist_ok=True)
        print(f"\tStoring at: {results_dir}\n")

        document_path = os.path.join(PAPER_PROJECT_DIR, experiment_paths["document"])

        output_file = open(os.path.join(results_dir, "summary.csv"), "w")
        output_file.write("id,Regex,outputs_are_equal\n")

        for dataset in sorted(os.listdir(sample_dir)):
            print()
            dataset_results_dir = os.path.join(results_dir, dataset)
            os.makedirs(dataset_results_dir)

            for experiment in sorted(os.listdir(os.path.join(sample_dir, dataset))):
                regex_path = os.path.join(sample_dir, dataset, experiment, "rematch.rgx")
                print(f"Regex: {dataset}/{experiment}")
                try:
                    equal = check_if_outputs_are_equal(document_path, regex_path, dataset_results_dir, f"{dataset}-{experiment}")
                    output_file.write(f"{dataset}-{experiment},\"{get_regex(regex_path)}\",{equal}\n")
                except TimeoutError:
                    print("Timeout")
                    output_file.write(f"{dataset}-{experiment},\"{get_regex(regex_path)}\",timeout\n")

        output_file.close()


if __name__ == "__main__":
    run_experiments(CHOSEN_BENCHMARK)
