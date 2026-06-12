"""Conftest file"""

import shutil
import pyrematch as re

def pytest_sessionstart():
    """Called before tests start."""
    print(shutil.get_terminal_size().columns*"=")
    print(f"PyREmatch version: {re.__version__}")
