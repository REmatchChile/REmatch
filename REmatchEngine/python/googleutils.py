from __future__ import print_function
import pickle
import os.path
from googleapiclient.discovery import build
from google_auth_oauthlib.flow import InstalledAppFlow
from google.auth.transport.requests import Request

# Defined IDs corresponding to current Drive location
PARENT_FOLDER = "1uv4AWVa7z1kM75l5K80cuY_97Xt9_O9z"
TEMPLATE_FILE = "1jakMn4ckrESICFFFuRhRcwNN-VeswGRgnv_LjeYVHew"

# If modifying these scopes, delete the file token.pickle.
DRIVE_SCOPES = ['https://www.googleapis.com/auth/drive']
SHEETS_SCOPES = ['https://www.googleapis.com/auth/spreadsheets']


def get_or_create_spreadsheet(drive_service, name):
    """Looks for a spreadsheet (not trashed) given a parentId and a name.
    If nothing is found then copies a spreadsheet from copyFromId and sets
    the parentId as a parent.

    Returns: Spreadsheet ID
    """

    query = ("mimeType='application/vnd.google-apps.spreadsheet' and "
             "name='{0}' and "
             "'{1}' in parents and "
             "trashed = false").format(name, PARENT_FOLDER)

    response = drive_service.files().list(q=query,
                                          spaces='drive',
                                          fields='nextPageToken, files(id, name)').execute()
    files = response.get('files', [])
    if len(files) > 1:
        raise ValueError("Found 2 or more spreadsheets with same name.")
    elif len(files) == 1:
        return files[0].get('id')
    else:
        data = {
            'mimeType' : 'application/vnd.google-apps.spreadsheet',
            'name' : name,
            'parents' : [PARENT_FOLDER]
        }

        sheet = drive_service.files().copy(fileId=TEMPLATE_FILE,
                                           body=data).execute()
        return sheet['id']

def get_sheets_service():
    creds = None
    # The file token.pickle stores the user's access and refresh tokens, and is
    # created automatically when the authorization flow completes for the first
    # time.
    if os.path.exists('sheets_token.pickle'):
        with open('sheets_token.pickle', 'rb') as token:
            creds = pickle.load(token)
    # If there are no (valid) credentials available, let the user log in.
    if not creds or not creds.valid:
        if creds and creds.expired and creds.refresh_token:
            creds.refresh(Request())
        else:
            flow = InstalledAppFlow.from_client_secrets_file(
                'sheets_credentials.json', SHEETS_SCOPES)
            creds = flow.run_local_server(port=0)
        # Save the credentials for the next run
        with open('sheets_token.pickle', 'wb') as token:
            pickle.dump(creds, token)

    return build('sheets', 'v4', credentials=creds)


def get_drive_service():
    creds = None
    # The file token.pickle stores the user's access and refresh tokens, and is
    # created automatically when the authorization flow completes for the first
    # time.
    if os.path.exists('drive_token.pickle'):
        with open('drive_token.pickle', 'rb') as token:
            creds = pickle.load(token)
    # If there are no (valid) credentials available, let the user log in.
    if not creds or not creds.valid:
        if creds and creds.expired and creds.refresh_token:
            creds.refresh(Request())
        else:
            flow = InstalledAppFlow.from_client_secrets_file(
                'drive_credentials.json', DRIVE_SCOPES)
            creds = flow.run_local_server(port=0)
        # Save the credentials for the next run
        with open('drive_token.pickle', 'wb') as token:
            pickle.dump(creds, token)

    return build('drive', 'v3', credentials=creds)

def main():
    """Shows basic usage of the Drive v3 API.
    Prints the names and ids of the first 10 files the user has access to.
    """

    DRIVE = get_drive_service()

    id_ = get_or_create_spreadsheet(DRIVE, "test")

    print(id_)


if __name__ == '__main__':
    main()