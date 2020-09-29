import json
import os
import os.path
from datetime import date

import googleutils

import subprocess

os.chdir(os.path.dirname(os.path.realpath(__file__)))

with open("benchmark.json") as jsonFile:
		data = json.load(jsonFile)

START_COL = "A"
START_ROW = 4

PARENT_FOLDER = "13CDshEJ3lXQIYmumBpmDN1_iZnGsnTjU"
TEMPLATE_FILE = "1uFpKQU2xc5tMAf0hIQi672jVwi2fp7SMtQTFyPHUPr0"

SCOPES = data['scopes']
SHEETS = data['sheets']
DATASET = data['dataset']
HOME_DIR = data['homedir']
NEXP = data['nexp']
TOT_COLS = data['cols']
BINARIES = data['binaries']
DESCRIPTION = data['description']

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
	command = "wc -ml {0} | awk '{{print $1, $2}}'".format(doc_path)
	process = subprocess.run(command, shell=True, check=True,
	                         capture_output=True, universal_newlines=True)
	nlines, nchars = process.stdout.split()
	command = "du -h {0} | awk '{{print $1}}'".format(doc_path)
	process = subprocess.run(command, shell=True, check=True,
	                         capture_output=True, universal_newlines=True)
	filesize = process.stdout.replace('\n', '')

	return filesize, int(nchars), int(nlines)


def run_bench(binary, doc_path, rgx_path, nexp):
	mem_command = "/usr/bin/time -f \"%M\" {0}"
	time_command = "millisecond-time {0}"

	data = BINARIES[binary]
	args = data['args'].format(doc_path, rgx_path)

	bin_path = data['command'].format(HOME_DIR)

	subcommand = "{0} {1}".format(bin_path, args)

	command_mem = mem_command.format(subcommand)
	command_time = time_command.format(subcommand)

	tot_time = 0

	process = subprocess.run(command_mem, shell=True, check=True, capture_output=True, universal_newlines=True)
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

	exps_path = "{0}/exp/benchmark".format(HOME_DIR)

	ndocstats = 3

	row_counter = START_ROW

	for dataset in next(os.walk(exps_path))[1]:

		writeInCell(SHEETS_SERVICE, spreadsheet_id, single_range.format(START_COL, row_counter), [["!!" + dataset]])
		row_counter += 1

		for experiment in sorted(map(lambda x: x[0], filter(lambda x: bool(x[2]), os.walk(os.path.join(exps_path, dataset))))):

			print("On experiment:", experiment)

			rgx_col = chr(ord(START_COL))

			rem_rgx_path = os.path.join(experiment,"rematch.rgx")

			writeInCell(SHEETS_SERVICE, spreadsheet_id, single_range.format(rgx_col, row_counter), [[get_rgx(rem_rgx_path)]])

			print("\nStarting query:", get_rgx(rem_rgx_path))

			row_values = [[0 for _ in range(3 + num_bins * 3 + 2)]]

			leftmost_col = chr(ord(START_COL) + 1)
			rightmost_col = chr(ord(leftmost_col) + 3 + num_bins*3 + 2)

			doc_path = os.path.join(experiment,"doc.txt")

			filesize, nchars, nlines = docstats(doc_path)

			row_values[0][0] = filesize
			row_values[0][1] = nchars
			row_values[0][2] = nlines

			for j, binary in enumerate(BINARIES):

				print("\n  Testing lib:", binary)

				rgx_path = os.path.join(experiment,"{0}.rgx".format(BINARIES[binary]["rgx_type"]))

				time, memory, noutputs = run_bench(binary, doc_path, rgx_path, NEXP)

				# Write data to cells
				row_values[0][ndocstats + num_bins*0 + j] = time
				row_values[0][ndocstats + num_bins*1 + j] = memory
				row_values[0][ndocstats + num_bins*2 + j] = noutputs

			writeInCell(SHEETS_SERVICE, spreadsheet_id,
								  current_range.format(leftmost_col, row_counter, rightmost_col, row_counter),
								  row_values)
			row_counter += 1

if __name__ == '__main__':
	main()


