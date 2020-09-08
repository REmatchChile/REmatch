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


def writeInCell(service, ss_id, cellRange, value):
	values = [[value]]
	body = {'values' : values}
	sheet = service.spreadsheets()
	sheet.values().update(spreadsheetId=ss_id,
		range=cellRange, valueInputOption='USER_ENTERED', body=body).execute()
	# if result.get('updatedCells') == 1:
	# 	print('[cell {} updated]'.format(cellRange))
	# else:
	# 	print('[{} cells updated]'.format(result.get('updatedCells')))

def get_rgx(rgx_path):
	with open(rgx_path, 'r') as file_path:
		rgx =  file_path.read()
	return rgx

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

	current_range = 'RKBExplorer!{0}{1}'

	doc_path = "{0}/datasets/RKBExplorer/sparql.head".format(HOME_DIR)

	exps_path = "{0}/exp/RKBExplorer".format(HOME_DIR)

	for query, experiment in enumerate(sorted(next(os.walk(exps_path))[1])):

			row = START_ROW + query
			rgx_col = chr(ord(START_COL))

			rem_rgx_path = "{0}/exp/RKBExplorer/{1}/rematch.rgx".format(HOME_DIR, experiment)

			writeInCell(SHEETS_SERVICE, spreadsheet_id, current_range.format(rgx_col, row), get_rgx(rem_rgx_path))

			print("\nStarting query:", get_rgx(rem_rgx_path))

			for j, binary in enumerate(BINARIES):

				print("\n  Testing lib:", binary)

				col_int = ord(START_COL) + j + 1

				time_col = chr(col_int + 0*num_bins)
				mem_col = chr(col_int + 1*num_bins)
				nout_col = chr(col_int + 2*num_bins)

				rgx_path = "{0}/exp/RKBExplorer/{1}/{2}.rgx".format(HOME_DIR, experiment, BINARIES[binary]["rgx_type"])

				time, memory, noutputs = run_bench(binary, doc_path, rgx_path, NEXP)

				# Write data to cells
				writeInCell(SHEETS_SERVICE, spreadsheet_id, current_range.format(time_col, row), time)
				writeInCell(SHEETS_SERVICE, spreadsheet_id, current_range.format(mem_col, row), memory)
				writeInCell(SHEETS_SERVICE, spreadsheet_id, current_range.format(nout_col, row), noutputs)

if __name__ == '__main__':
	main()


