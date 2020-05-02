from __future__ import print_function
import pickle
import json
import os.path
from googleapiclient.discovery import build
from google_auth_oauthlib.flow import InstalledAppFlow
from google.auth.transport.requests import Request

import subprocess

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


def formatMem(sizeInKb):
	units = ['K', 'M', 'G']
	counter = 0

	while(sizeInKb > 1024):
		sizeInKb /= 1024
		counter += 1
	return '{0}{1}'.format(round(sizeInKb, 2), units[counter])

def writeInCell(service, cellRange, value):
	values = [[value]]
	body = {'values' : values}
	sheet = service.spreadsheets()
	result = sheet.values().update(spreadsheetId=SHEETS[DATASET],
		range=cellRange, valueInputOption='USER_ENTERED', body=body).execute()
	if result.get('updatedCells') == 1:
		print('[cell {} updated]'.format(cellRange))
	else:
		print('[{} cells updated]'.format(result.get('updatedCells')))


def main():
	"""Shows basic usage of the Sheets API.
	Prints values from a sample spreadsheet.
	"""
	creds = None
	# The file token.pickle stores the user's access and refresh tokens, and is
	# created automatically when the authorization flow completes for the first
	# time.
	if os.path.exists('token.pickle'):
		with open('token.pickle', 'rb') as token:
			creds = pickle.load(token)
	# If there are no (valid) credentials available, let the user log in.
	if not creds or not creds.valid:
		if creds and creds.expired and creds.refresh_token:
			creds.refresh(Request())
		else:
			flow = InstalledAppFlow.from_client_secrets_file(
				'credentials.json', SCOPES)
			creds = flow.run_local_server(port=0)
		# Save the credentials for the next run
		with open('token.pickle', 'wb') as token:
			pickle.dump(creds, token)


	COMMAND = "/usr/bin/time -f \"%e\\n%M\" {0}"

	service = build('sheets', 'v4', credentials=creds)
	for query in TOT_QUERIES:
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

				writeInCell(service, "Query {0}!{1}{2}".format(query, BINARIES[col]['output_col'], row+2), nout)
				writeInCell(service, "Query {0}!{1}{2}".format(query, BINARIES[col]['memory_col'], row+2), mem)

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
				writeInCell(service, current_range, t)

if __name__ == '__main__':
	main()


