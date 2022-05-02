import googleutils
import pandas as pd
import os
import os.path as pth
import re
import numpy as np

HERE = pth.dirname(pth.realpath(__file__)) # Current directory
PROJECT_DIR = '/home/nicovsj/repos/research/REmatch-org/REmatch'
EXPS_DIRECTORY = pth.join(PROJECT_DIR, 'exp/official')

VALID_DIR_RGX = "exp\d+"

def get_rgx(rgx_path):
	with open(rgx_path, 'r') as file_path:
		rgx =  file_path.read()
	return rgx


def get_df_from_expdir(datasetdir: str):
  iterate = os.walk(datasetdir) # Walk recursively
  iterate = filter(lambda x: bool(x[2]), iterate) # Only directories
  iterate = map(lambda x: x[0], iterate) # Get path
  iterate = filter(lambda x: bool(re.fullmatch(VALID_DIR_RGX, pth.basename(x))), iterate)

  d = {'n': [], 'allmatch': [], 'perl': []}

  for i, exp in enumerate(sorted(iterate)):
    allm = get_rgx(pth.join(exp, 'rematch.rgx'))
    perl = get_rgx(pth.join(exp, 'perl.rgx'))

    d['n'].append(i)
    d['allmatch'].append(f"'{allm}")
    d['perl'].append(f"'{perl}")

  return pd.DataFrame(d, index=None)


def col2n(sscol):
  n = 0
  for c in sscol:
      n = n * 26 + 1 + ord(c) - ord('A')
  return n

def n2col(n):
  name = ''
  while n > 0:
      n, r = divmod (n - 1, 26)
      name = chr(r + ord('A')) + name
  return name

def try_duplicate_sheet(service, ss_id, from_name, new_name):
  sheet_md = service.spreadsheets().get(spreadsheetId=ss_id).execute()
  properties = sheet_md.get('sheets')
  for item in properties:
    if item.get("properties").get('title') == from_name:
       sheet_id = (item.get("properties").get('sheetId'))
       break

  body = {
    "requests":[ {
        "duplicateSheet": {
          "sourceSheetId": sheet_id,
          "newSheetName": new_name
        }
      }
    ]
  }

  request = SERVICE.spreadsheets().batchUpdate(spreadsheetId=SS_ID, body=body)
  response = request.execute()
  print(response)

def df_to_spreadsheet(service, ss_id, start_cell, df):
  values = df.values.tolist()
  cell_format = re.compile('([A-Za-z0-9\-]+)!([A-Z]+)(\d+)')
  m = cell_format.fullmatch(start_cell)
  if not m:
    raise ValueError
  sheet = m.group(1)
  col = m.group(2)
  row = m.group(3)

  end_row = int(row) + df.shape[0]
  end_col = n2col(col2n(col) + df.shape[1])


  cell_range = f'{start_cell}:{end_col}{end_row}'
  body = {'values': values}
  sheet = service.spreadsheets()
  result = sheet.values().update(spreadsheetId=ss_id,
		range=cell_range, valueInputOption='USER_ENTERED', body=body).execute()
  if result.get('updatedCells') == 1:
    print('[cell {} updated]'.format(cell_range))
  else:
    print('[{} cells updated at {}]'.format(result.get('updatedCells'), cell_range))



if __name__ == "__main__":
  SERVICE = googleutils.get_sheets_service()
  SS_ID = "1UvmvDVWpT6Ur98UK4vEWDdvvIzwYXiszbIV4EN1MLvE"

  for dataset in sorted(next(os.walk(EXPS_DIRECTORY))[1]):
    try_duplicate_sheet(SERVICE, SS_ID, 'Template', dataset)
    df = get_df_from_expdir(pth.join(EXPS_DIRECTORY, dataset))

    print(df)

    df_to_spreadsheet(SERVICE, SS_ID, f'{dataset}!A3', df)