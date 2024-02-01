import subprocess
from typing import List
import os
import json
from datetime import datetime

HERE = os.path.dirname(os.path.realpath(__file__))

with open(os.path.join(HERE, "config/test_correctness-config.json")) as jsonFile:
    EXPERIMENT_CONFIG = json.load(jsonFile)

with open(os.path.join(HERE, "config/test_correctness-run.json")) as jsonFile:
    BENCHMARK_CONFIG = json.load(jsonFile)

CHOSEN_EXPERIMENTS: List[str] = BENCHMARK_CONFIG["experimentsToRun"]

PROJECT_DIR: str = BENCHMARK_CONFIG["projectDir"]

CPP_BINARIES_DIR = BENCHMARK_CONFIG["binariesDir"].format(projectDir=PROJECT_DIR)

COMMAND_DATA = EXPERIMENT_CONFIG["REmatchData"]

def get_regex(rgx_path):
    with open(rgx_path, "r") as file_path:
        rgx = file_path.read()
    return rgx


def get_sorted_outputs(command: str) -> List[str]:
    try:
        process = subprocess.run(
            command.split(" "),
            text=True,
            shell=False,
            start_new_session=True,
            stdout=subprocess.PIPE,
            timeout=BENCHMARK_CONFIG["timeoutInSeconds"],
        )
        return sorted(process.stdout.splitlines())

    except subprocess.TimeoutExpired:
        raise TimeoutError(command)


def find_differences(actual_outputs, expected_outputs):
    idx1 = 0
    idx2 = 0

    missing_outputs = []
    extra_outputs = []

    while idx1 < len(actual_outputs) and idx2 < len(expected_outputs):

        actual_output = actual_outputs[idx1]
        expected_output = expected_outputs[idx2]

        if actual_output > expected_output:
            missing_outputs.append(expected_output)
            idx2 += 1
        elif expected_output > actual_output:
            extra_outputs.append(actual_output)
            idx1 += 1
        else:
            idx1 += 1
            idx2 += 1

    missing_outputs.extend(expected_outputs[idx2:])
    extra_outputs.extend(actual_outputs[idx1:])

    return missing_outputs, extra_outputs


def print_differences(missing_outputs, extra_outputs):
    if (missing_outputs or extra_outputs):
        print(f"Outputs are different")
    else:
        print(f"Outputs are identical")

    if missing_outputs:
        print(f"Missing outputs:")
        for output in missing_outputs:
            print(f"\t{output}")

    if extra_outputs:
        print(f"Extra outputs in:")
        for output in extra_outputs:
            print(f"\t{output}")


def get_sorted_expected_outputs(expected_outputs_path):
    with open(expected_outputs_path, "r") as expected_outputs_file:
        expected_outputs = expected_outputs_file.readlines()

    expected_outputs = [output.strip() for output in expected_outputs]
    return sorted(expected_outputs)


def check_if_outputs_are_correct(document_path, regex_path, expected_outputs_path):
    binary = os.path.join(CPP_BINARIES_DIR, COMMAND_DATA["command"])
    command_template =  f'{binary} {COMMAND_DATA["args"]}'

    command = command_template.format(document=document_path, regex=regex_path)

    actual_outputs = get_sorted_outputs(command)
    expected_outputs = get_sorted_expected_outputs(expected_outputs_path)

    missing_outputs, extra_outputs = find_differences(actual_outputs, expected_outputs)

    print_differences(missing_outputs, extra_outputs)

    outputs_are_equal = not (missing_outputs or extra_outputs)

    return outputs_are_equal


def run_experiments():
    for suite in CHOSEN_EXPERIMENTS:
        print(f"\tOn dataset: {suite}\n")

        experiment_paths = EXPERIMENT_CONFIG["experiments"][suite]
        sample_dir = os.path.join(PROJECT_DIR, experiment_paths["queries"])

        timestamp = datetime.now().strftime("%Y-%m-%d_%H:%M:%S")

        results_dir = os.path.join(
            PROJECT_DIR, EXPERIMENT_CONFIG["summaryDirectory"], suite, timestamp
        )
        os.makedirs(results_dir, exist_ok=True)
        print(f"\tStoring at: {results_dir}\n")

        document_path = os.path.join(PROJECT_DIR, experiment_paths["document"])

        output_file = open(os.path.join(results_dir, "summary.csv"), "w")
        output_file.write("id,regex,is_correct\n")

        for dataset in sorted(os.listdir(sample_dir)):
            print()
            expected_outputs_path = os.path.join(PROJECT_DIR, experiment_paths["outputs"], dataset)

            for experiment in sorted(os.listdir(os.path.join(sample_dir, dataset))):
                regex_path = os.path.join(sample_dir, dataset, experiment, "rematch.rgx")
                expected_output_path = os.path.join(expected_outputs_path, experiment + ".txt")
                print(f"Regex: {dataset}/{experiment}")
                try:
                    equal = check_if_outputs_are_correct(document_path, regex_path, expected_output_path)
                    output_file.write(f"{dataset}-{experiment},\"{get_regex(regex_path)}\",{equal}\n")
                except TimeoutError:
                    print("Timeout")
                    output_file.write(f"{dataset}-{experiment},\"{get_regex(regex_path)}\",timeout\n")

        output_file.close()


if __name__ == "__main__":
    run_experiments()
