import json
import os
import os.path as pth
from datetime import date
import signal
import csv

import googleutils

import subprocess

import re

import pandas as pd

here = pth.dirname(pth.realpath(__file__))

with open(pth.join(here, "benchmark-alt-forceall.json")) as jsonFile:
    data = json.load(jsonFile)


START_COL = "A"
START_ROW = 4

PARENT_FOLDER = "13CDshEJ3lXQIYmumBpmDN1_iZnGsnTjU"
# TEMPLATE_FILE = "1uFpKQU2xc5tMAf0hIQi672jVwi2fp7SMtQTFyPHUPr0"
# TEMPLATE_FILE = "1H2fIh7aOgs-2P9Vx5YYgfHifuAdd5dseNhEkkhUziFk"

# TEMPLATE_FILE = "1QMcMNan9QlRQ-vYCEW2vMy0bUyyweXKI8fREasefb7c"

TEMPLATE_FILE = data["template"]
SCOPES = data["scopes"]
SHEETS = data["sheets"]
DATASET = data["dataset"]
HOME_DIR = data["homedir"]
EXP_SUBPATH = data["exp-subpath"]
NEXP = data["nexp"]
TOT_COLS = data["cols"]
BINARIES = data["binaries"]
DESCRIPTION = data["description"]
BLACKLIST = [
    "onig-f"
    # ,"re2-f", 'PCRE-f', "boost-f"
    ,
    "MDFA",
    "REm"
    # ,"re2", "boost", "onig", "PCRE"
]

TIMEOUT = 100


def formatMem(sizeInKb):
    units = ["K", "M", "G"]
    counter = 0

    while sizeInKb > 1024:
        sizeInKb /= 1024
        counter += 1
    return "{0}{1}".format(round(sizeInKb, 2), units[counter])


def writeInCell(service, ss_id, cellRange, values):
    body = {"values": values}
    sheet = service.spreadsheets()
    result = (
        sheet.values()
        .update(
            spreadsheetId=ss_id,
            range=cellRange,
            valueInputOption="USER_ENTERED",
            body=body,
        )
        .execute()
    )
    if result.get("updatedCells") == 1:
        print("[cell {} updated]".format(cellRange))
    else:
        print("[{} cells updated at {}]".format(result.get("updatedCells"), cellRange))


def get_rgx(rgx_path):
    with open(rgx_path, "r") as file_path:
        rgx = file_path.read()
    return rgx


def docstats(doc_path):
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

    return filesize, int(nchars), int(nlines) + 1


def automata_stats(doc_path, rgx_path):
    command = "{0}/build/Release/bin/rematch --searching --mode=benchmark -d {1} -r {2}".format(
        HOME_DIR, doc_path, rgx_path
    )
    try:
        process = subprocess.run(
            command,
            shell=True,
            check=True,
            capture_output=True,
            universal_newlines=True,
        )
    except:
        print(f"Error while processing command: {command}")
        return "err", "err", "err", "err", "err"

    # print(process.stdout)
    dsize = int(re.search(r"DetSize\s+(\d+)", process.stdout).group(1))
    esize = int(re.search(r"eVASize\s+(\d+)", process.stdout).group(1))
    msize = int(re.search(r"MDFASize\s+(\d+)", process.stdout).group(1))
    nsegs = int(re.search(r"Number of segments\s+(\d+)", process.stdout).group(1))
    ambig = int(re.search(r"Ambiguous\?\s+(\d)", process.stdout).group(1))

    return dsize, esize, msize, nsegs, ambig


def ambiguous(doc_path, rgx_path):
    command = f"{HOME_DIR}/build/Release/bin/rematch --mode=ambiguous -d {doc_path} -r {rgx_path}"
    try:
        process = subprocess.run(
            command,
            shell=True,
            check=True,
            capture_output=True,
            universal_newlines=True,
        )
    except:
        print(f"Error while processing command: {command}")
        return "err"

    return int(process.stdout)


def re2_algo(doc_path, rgx_path):
    command = "{0}/build/Release/bin/re2-algo {1} {2}".format(
        HOME_DIR, doc_path, rgx_path
    )
    process = subprocess.run(
        command, shell=True, check=True, capture_output=True, universal_newlines=True
    )
    # print(process.stdout)

    return process.stdout


def col2n(sscol):
    n = 0
    for c in sscol:
        n = n * 26 + 1 + ord(c) - ord("A")
    return n


def n2col(n):
    name = ""
    while n > 0:
        n, r = divmod(n - 1, 26)
        name = chr(r + ord("A")) + name
    return name


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
        nout, output = process.communicate(timeout=TIMEOUT)
    except subprocess.TimeoutExpired:
        print(f"Timeout with command: {subcommand}")
        os.killpg(os.getpgid(process.pid), signal.SIGTERM)
        return "timeout", "timeout", "timeout"
    except:
        print(f"Error while processing command: {subcommand}")
        return "err", "err", "err"

    return int(nout.strip())


def run_bench(binary, doc_path, rgx_path, nexp):
    if binary in BLACKLIST:
        return "blisted", "blisted", "blisted"
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
        nout, output = process.communicate(timeout=TIMEOUT)
    except subprocess.TimeoutExpired:
        print(f"Timeout with command: {command_mem}")
        os.killpg(os.getpgid(process.pid), signal.SIGTERM)
        return "timeout", "timeout", "timeout"
    except:
        print(f"Error while processing command: {command_mem}")
        return "err", "err", "err"
    mem = output.replace('"', "").replace("\n", "")
    nout = int(nout)
    mem = formatMem(int(mem))
    print("    Number of outputs:", nout)
    print("    Memory used:", mem)

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
    print("    Avg time: {0}ms".format(t))

    return t, mem, nout


def output_main():
    SHEETS_SERVICE = googleutils.get_sheets_service()
    DRIVE_SERVICE = googleutils.get_drive_service()

    spreadsheet_name = "({}) {} - {}".format(str(date.today()), DATASET, DESCRIPTION)

    spreadsheet_id = googleutils.get_or_create_spreadsheet(
        DRIVE_SERVICE, PARENT_FOLDER, spreadsheet_name, TEMPLATE_FILE
    )

    bufsize = 50
    col_buffer = [[0] for _ in range(bufsize)]
    rgx_buffer = [[""] for _ in range(bufsize)]
    col_size = 0

    row_counter = 0

    exps_path = pth.join(HOME_DIR, EXP_SUBPATH)

    df = pd.DataFrame()

    for dataset in sorted(next(os.walk(exps_path, followlinks=True))[1]):
        for experiment in sorted(
            map(
                lambda x: x[0],
                filter(lambda x: bool(x[2]), os.walk(os.path.join(exps_path, dataset))),
            )
        ):
            print("On experiment:", experiment)

            row = dict()

            # if(int(experiment[-4:]) < 0):
            # 	continue

            # if(int(experiment[-4:]) <= 3275):
            # 	continue

            rem_rgx_path = os.path.join(experiment, "rematch.rgx")

            curr_rgx = get_rgx(rem_rgx_path)

            # rgx_buffer[col_size][0] = curr_rgx
            row["query"] = curr_rgx

            print(f"\nStarting query: {curr_rgx}")

            doc_path = os.path.join(experiment, "doc.txt")

            binary = "SDFA"

            print("\n  Testing lib:", binary)

            ambig = ambiguous(doc_path, rem_rgx_path)

            # noutputs = run_outputs(binary, doc_path, rem_rgx_path)

            # col_buffer[col_size][0] = noutputs
            # row['noutputs'] = noutputs
            row["ambiguous"] = ambig

            col_size += 1

            # if col_size == bufsize:
            # 	print('\nSending to GoogleSheets!\n')
            # 	writeInCell(SHEETS_SERVICE, spreadsheet_id,
            # 	f'Data!AP{row_counter-(col_size-1)}:AP{row_counter}',
            # 	col_buffer)

            # 	writeInCell(SHEETS_SERVICE, spreadsheet_id,
            # 	f'Data!A{row_counter-(col_size-1)}:A{row_counter}',
            # 	rgx_buffer)

            # 	col_size = 0

            row_counter += 1

            df = pd.concat([df, pd.DataFrame(data=row, index=[row_counter - 1])])
            df.to_csv(pth.join(here, "data.csv"), quoting=csv.QUOTE_NONNUMERIC)

        # if col_size > bufsize:
        # 	writeInCell(SHEETS_SERVICE, spreadsheet_id,
        # 	f'Data!AP{row_counter-(col_size-1)}:AP{row_counter}',
        # 	col_buffer)

        # 	writeInCell(SHEETS_SERVICE, spreadsheet_id,
        # 	f'Data!A{row_counter-(col_size-1)}:A{row_counter}',
        # 	rgx_buffer)

        # 	print('\nSending to GoogleSheets!\n')

        # 	col_size = 0


def main():
    """Shows basic usage of the Sheets API.
    Prints values from a sample spreadsheet.
    """

    SHEETS_SERVICE = googleutils.get_sheets_service()
    DRIVE_SERVICE = googleutils.get_drive_service()

    spreadsheet_name = "({}) {} - {}".format(str(date.today()), DATASET, DESCRIPTION)

    spreadsheet_id = googleutils.get_or_create_spreadsheet(
        DRIVE_SERVICE, PARENT_FOLDER, spreadsheet_name, TEMPLATE_FILE
    )
    num_bins = len(BINARIES)

    current_range = "Data!{0}{1}:{2}{3}"
    single_range = "Data!{0}{1}"

    exps_path = pth.join(HOME_DIR, EXP_SUBPATH)

    ndocstats = 3
    nautomatastats = 4
    ntotstats = ndocstats + nautomatastats

    row_counter = START_ROW

    row_length = ntotstats + num_bins * 4 + 1

    for dataset in sorted(next(os.walk(exps_path, followlinks=True))[1]):

        writeInCell(
            SHEETS_SERVICE,
            spreadsheet_id,
            single_range.format(START_COL, row_counter),
            [["!!" + dataset]],
        )
        row_counter += 1

        for experiment in sorted(
            map(
                lambda x: x[0],
                filter(lambda x: bool(x[2]), os.walk(os.path.join(exps_path, dataset))),
            )
        ):

            print("On experiment:", experiment)

            rem_rgx_path = os.path.join(experiment, "rematch.rgx")

            writeInCell(
                SHEETS_SERVICE,
                spreadsheet_id,
                single_range.format(START_COL, row_counter),
                [[get_rgx(rem_rgx_path)]],
            )

            print("\nStarting query:", get_rgx(rem_rgx_path))

            row_values = [[0 for _ in range(row_length)]]

            leftmost_col = n2col(col2n(START_COL) + 1)

            rightmost_col = n2col(col2n(leftmost_col) + row_length)

            doc_path = os.path.join(experiment, "doc.txt")

            print("Calculating docstats...")

            filesize, nchars, nlines = docstats(doc_path)

            row_values[0][0] = filesize
            row_values[0][1] = nchars
            row_values[0][2] = nlines

            print("Docstats calculated")

            dsize, esize, msize, nsegs = automata_stats(
                doc_path, os.path.join(experiment, "rematch.rgx")
            )

            # row_values[0][-1] = evaltime

            row_values[0][ndocstats] = esize
            row_values[0][ndocstats + 1] = dsize
            row_values[0][ndocstats + 2] = msize
            row_values[0][ndocstats + 3] = nsegs

            # row_values[0][-1] = re2_algo(doc_path, os.path.join(experiment,"perl.rgx"))

            for j, binary in enumerate(BINARIES):

                print("\n  Testing lib:", binary)

                rgx_path = os.path.join(
                    experiment, "{0}.rgx".format(BINARIES[binary]["rgx_type"])
                )

                time, memory, noutputs = run_bench(binary, doc_path, rgx_path, NEXP)

                BINARIES[binary]["time"] = time
                BINARIES[binary]["memory"] = memory
                BINARIES[binary]["noutputs"] = noutputs

                # Write data to cells
                row_values[0][ntotstats + num_bins * 0 + j] = time
                row_values[0][ntotstats + num_bins * 2 + j] = memory
                row_values[0][ntotstats + num_bins * 3 + j] = noutputs

            def time_key(x):
                if type(BINARIES[x]["time"]) == str:
                    return float("inf")
                return BINARIES[x]["time"]

            ranked = list(sorted(BINARIES, key=time_key))
            for i, key in enumerate(ranked):
                row_values[0][ntotstats + num_bins * 1 + i] = key

            writeInCell(
                SHEETS_SERVICE,
                spreadsheet_id,
                current_range.format(
                    leftmost_col, row_counter, rightmost_col, row_counter
                ),
                row_values,
            )
            row_counter += 1


if __name__ == "__main__":
    # main()
    output_main()
