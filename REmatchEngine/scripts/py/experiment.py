import json
import os.path
from datetime import date

import googleutils

import subprocess

os.chdir(os.path.dirname(os.path.realpath(__file__)))

with open("settings.json") as jsonFile:
		data = json.load(jsonFile)

SCOPES = data['scopes']
SHEETS = data['sheets']
DATASET = data['dataset']
HOME_DIR = data['homedir']
NEXP = data['nexp']
TOT_COLS = data['cols']
TOT_ROWS = range(data['rows'][0], data['rows'][1] + 1)
TOT_QUERIES = range(data['queries'][0], data['queries'][1] + 1)
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
	result = sheet.values().update(spreadsheetId=ss_id,
		range=cellRange, valueInputOption='USER_ENTERED', body=body).execute()
	if result.get('updatedCells') == 1:
		print('[cell {} updated]'.format(cellRange))
	else:
		print('[{} cells updated]'.format(result.get('updatedCells')))

def get_rgx(rgx_path):
	with open(rgx_path, 'r') as file_path:
		rgx =  file_path.read()
	return rgx

def main():
	"""Shows basic usage of the Sheets API.
	Prints values from a sample spreadsheet.
	"""

	COMMAND = "/usr/bin/time -f \"%e\\n%M\" {0}"

	SHEETS_SERVICE = googleutils.get_sheets_service()
	DRIVE_SERVICE = googleutils.get_drive_service()

	spreadsheet_name = '({}) {} - {}'.format(str(date.today()), DATASET, DESCRIPTION)

	spreadsheet_id = googleutils.get_or_create_spreadsheet(DRIVE_SERVICE,
																												 spreadsheet_name)

	for query in TOT_QUERIES:
		writeInCell(SHEETS_SERVICE,
		            spreadsheet_id,
								"Query {}!D16".format(query),
								get_rgx("{0}/exp/{1}/exp{2}/regex.rgx".format(HOME_DIR, DATASET, query)))
		writeInCell(SHEETS_SERVICE,
		            spreadsheet_id,
								"Query {}!D17".format(query),
								get_rgx("{0}/exp/{1}/exp{2}/re2regex.rgx".format(HOME_DIR, DATASET, query)))
		# writeInCell(SHEETS_SERVICE,
		#             spreadsheet_id,
		# 						"Query {}!D18".format(query),
		# 						get_rgx("{0}/exp/{1}/exp{2}/script.awk".format(HOME_DIR, DATASET, query)))
		for row in TOT_ROWS:
			for col in TOT_COLS:
				current_range = 'Query {0}!{1}{2}'.format(query, col, row+2)

				print("\nAt cell", current_range)
				args = BINARIES[col]['args'].format(HOME_DIR, DATASET, row, query)
				callcommand = BINARIES[col]['command'].format(HOME_DIR)
				subcommand = "{0} {1}".format(callcommand, args)
				command = COMMAND.format(subcommand)

				totTime = 0

				process = subprocess.run(command, shell=True, check=True, capture_output=True, universal_newlines=True)
				output = process.stderr
				nout = process.stdout
				t, mem, _ = output.replace('"', '').split('\n')
				nout = int(nout)
				mem = formatMem(int(mem))
				print("Number of outputs:", nout)
				print("Memory used:", mem)

				writeInCell(SHEETS_SERVICE,
									  spreadsheet_id,
									  "Query {0}!{1}{2}".format(query, chr(ord(col)+2*len(BINARIES)), row+2),
										nout)

				writeInCell(SHEETS_SERVICE,
										spreadsheet_id,
										"Query {0}!{1}{2}".format(query, chr(ord(col)+len(BINARIES)), row+2),
										mem)

				print("\tIteration {0} -> t={1:.2f}s".format(1, float(t)))
				totTime += float(t)

				for i in range(NEXP-1):
					process = subprocess.run(command, shell=True, check=True, capture_output=True, universal_newlines=True)
					output = process.stderr
					t, _, _ = output.replace('"', '').split('\n')
					print("\tIteration {0} -> t={1:.2f}s".format(i+2, float(t)))
					totTime += float(t)

				t = totTime/NEXP
				print("Avg time: {0:.2f}s".format(t))
				writeInCell(SHEETS_SERVICE, spreadsheet_id, current_range, t)

if __name__ == '__main__':
	main()


