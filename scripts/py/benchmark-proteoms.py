import json
import os
import os.path as pth
from datetime import date
import signal

import googleutils

import subprocess

import re

here = pth.dirname(pth.realpath(__file__))

with open(pth.join(here, "benchmark-proteoms.json")) as jsonFile:
		data = json.load(jsonFile)


START_COL = "A"
START_ROW = 4

PARENT_FOLDER = "13CDshEJ3lXQIYmumBpmDN1_iZnGsnTjU"
# TEMPLATE_FILE = "1uFpKQU2xc5tMAf0hIQi672jVwi2fp7SMtQTFyPHUPr0"
# TEMPLATE_FILE = "1H2fIh7aOgs-2P9Vx5YYgfHifuAdd5dseNhEkkhUziFk"

# TEMPLATE_FILE = "1QMcMNan9QlRQ-vYCEW2vMy0bUyyweXKI8fREasefb7c"

TEMPLATE_FILE = data['template']
SCOPES = data['scopes']
SHEETS = data['sheets']
DATASET = data['dataset']
HOME_DIR = data['homedir']
EXP_SUBPATH = data['exp-subpath']
NEXP = data['nexp']
TOT_COLS = data['cols']
BINARIES = data['binaries']
DESCRIPTION = data['description']
BLACKLIST = ["boost-forced",
						 "PCRE-forced",
						 "re2-forced",
						 "onig-forced",
						 "tre-forced"]

def formatMem(sizeInKb):
	units = ['K', 'M', 'G']
	counter = 0

	while(sizeInKb > 1024):
		sizeInKb /= 1024
		counter += 1
	return '{0}{1}'.format(round(sizeInKb, 2), units[counter])


def writeInCell(service, ss_id, cellRange, values):
	body = {'values' : values}
	sheet = service.spreadsheets()
	result = sheet.values().update(spreadsheetId=ss_id,
		range=cellRange, valueInputOption='USER_ENTERED', body=body).execute()
	if result.get('updatedCells') == 1:
		print('[cell {} updated]'.format(cellRange))
	else:
		print('[{} cells updated at {}]'.format(result.get('updatedCells'), cellRange))

def get_rgx(rgx_path):
	with open(rgx_path, 'r') as file_path:
		rgx =  file_path.read()
	return rgx

def docstats(doc_path):
	command = f"wc -ml \"$(readlink -f \"{doc_path}\")\" | awk '{{print $1, $2}}'"
	process = subprocess.run(command, shell=True, check=True,
	                         capture_output=True, universal_newlines=True)
	nlines, nchars = process.stdout.split()
	command = "du -h \"$(readlink -f \"{0}\")\" | awk '{{print $1}}'".format(doc_path)
	process = subprocess.run(command, shell=True, check=True,
	                         capture_output=True, universal_newlines=True)
	filesize = process.stdout.replace('\n', '')

	return filesize, int(nchars), int(nlines)+1

def automata_stats(doc_path, rgx_path):
	command = "{0}/build/Release/bin/rematch --searching --macrodfa --mode=benchmark -d {1} -r {2}".format(HOME_DIR, doc_path, rgx_path)
	try:
		process = subprocess.run(command, shell=True, check=True,
														capture_output=True, universal_newlines=True)
	except:
		print(f"Error while processing command: {command}")
		return 'err', 'err', 'err', 'err'

	# print(process.stdout)
	dsize = int(re.search(r'DetSize\s+(\d+)', process.stdout).group(1))
	esize = int(re.search(r'eVASize\s+(\d+)', process.stdout).group(1))
	msize = int(re.search(r'MDFASize\s+(\d+)', process.stdout).group(1))
	nsegs = int(re.search(r'Number of segments\s+(\d+)', process.stdout).group(1))

	return dsize, esize, msize, nsegs

def re2_algo(doc_path, rgx_path):
	command = "{0}/build/Release/bin/re2-algo {1} {2}".format(HOME_DIR, doc_path, rgx_path)
	process = subprocess.run(command, shell=True, check=True,
	                         capture_output=True, universal_newlines=True)
	# print(process.stdout)

	return process.stdout



def run_bench(binary, doc_path, rgx_path, nexp):
	if binary in BLACKLIST:
		return "blisted", "blisted", "blisted"
	mem_command = "/usr/bin/time -f \"%M\" {0}"
	time_command = "millisecond-time {0}"

	data = BINARIES[binary]
	args = data['args'].format(doc_path, rgx_path)

	bin_path = data['command'].format(HOME_DIR)

	subcommand = "{0} {1}".format(bin_path, args)

	command_mem = mem_command.format(subcommand)
	command_time = time_command.format(subcommand)

	tot_time = 0

	try:
		process = subprocess.run(command_mem.split(' '),
				shell=False, check=True, capture_output=True, universal_newlines=True,
				timeout=30)
	except subprocess.TimeoutExpired:
		print(f"Timeout with command: {command_mem}")
		return 'timeout', 'timeout', 'timeout'
	except:
		print(f"Error while processing command: {command_mem}")
		return 'err', 'err', 'err'
	output = process.stderr
	nout = process.stdout
	mem = output.replace('"', '').replace('\n','')
	nout = int(nout)
	mem = formatMem(int(mem))
	print("    Number of outputs:", nout)
	print("    Memory used:", mem)

	for _ in range(nexp):
		process = subprocess.run(command_time, shell=True, check=True, capture_output=True, universal_newlines=True)
		output = process.stderr
		t = output.replace('"', '').replace('\n','')
		# print("\tIteration {0} -> t={1:.2f}ms".format(i+2, float(t)))
		tot_time += float(t)

	t = tot_time/nexp
	print("    Avg time: {0}ms".format(t))

	return t, mem, nout



def main():
	"""Shows basic usage of the Sheets API.
	Prints values from a sample spreadsheet.
	"""

	SHEETS_SERVICE = googleutils.get_sheets_service()
	DRIVE_SERVICE = googleutils.get_drive_service()

	spreadsheet_name = '({}) {} - {}'.format(str(date.today()), DATASET, DESCRIPTION)

	spreadsheet_id = googleutils.get_or_create_spreadsheet(DRIVE_SERVICE,
																												 PARENT_FOLDER,
																												 spreadsheet_name,
																												 TEMPLATE_FILE)
	num_bins = len(BINARIES)

	current_range = 'Data!{0}{1}:{2}{3}'
	single_range = 'Data!{0}{1}'

	print("HOME_DIR:",HOME_DIR)

	exps_path = pth.join(HOME_DIR, EXP_SUBPATH)

	print(exps_path)

	ndocstats = 3
	nautomatastats = 4
	ntotstats = ndocstats + nautomatastats

	row_counter = START_ROW

	row_length = ntotstats + num_bins * 3 + 1

	for dataset in sorted(next(os.walk(exps_path))[1]):

		writeInCell(SHEETS_SERVICE, spreadsheet_id, single_range.format(START_COL, row_counter), [["!!" + dataset]])
		row_counter += 1

		for experiment in sorted(map(lambda x: x[0], filter(lambda x: bool(x[2]), os.walk(os.path.join(exps_path, dataset))))):

			print("On experiment:", experiment)

			rgx_col = chr(ord(START_COL))

			rem_rgx_path = os.path.join(experiment,"rematch.rgx")

			writeInCell(SHEETS_SERVICE, spreadsheet_id, single_range.format(rgx_col, row_counter), [[get_rgx(rem_rgx_path)]])

			print("\nStarting query:", get_rgx(rem_rgx_path))

			row_values = [[0 for _ in range(row_length)]]

			leftmost_col = chr(ord(START_COL) + 1)

			rmost_ord = ord(leftmost_col) + row_length

			if rmost_ord > 90 :
				rightmost_col = f"A{chr(rmost_ord - 26)}"
			else:
				rightmost_col = chr(rmost_ord)

			doc_path = os.path.join(experiment,"doc.txt")

			print("Calculating docstats...")

			filesize, nchars, nlines = docstats(doc_path)

			row_values[0][0] = filesize
			row_values[0][1] = nchars
			row_values[0][2] = nlines

			print("Docstats calculated")

			dsize, esize, msize, nsegs = automata_stats(doc_path, os.path.join(experiment,"rematch.rgx"))

			# row_values[0][-1] = evaltime

			row_values[0][ndocstats] = esize
			row_values[0][ndocstats+1] = dsize
			row_values[0][ndocstats+2] = msize
			row_values[0][ndocstats+3] = nsegs


			# row_values[0][-1] = re2_algo(doc_path, os.path.join(experiment,"perl.rgx"))

			for j, binary in enumerate(BINARIES):

				print("\n  Testing lib:", binary)

				rgx_path = os.path.join(experiment,"{0}.rgx".format(BINARIES[binary]["rgx_type"]))

				time, memory, noutputs = run_bench(binary, doc_path, rgx_path, NEXP)

				# Write data to cells
				row_values[0][ntotstats + num_bins*0 + j] = time
				row_values[0][ntotstats + num_bins*1 + j] = memory
				row_values[0][ntotstats + num_bins*2 + j] = noutputs

			writeInCell(SHEETS_SERVICE, spreadsheet_id,
								  current_range.format(leftmost_col, row_counter, rightmost_col, row_counter),
								  row_values)
			row_counter += 1

if __name__ == '__main__':
	main()


