from dataclasses import dataclass
import pyrematch as re
import pytest
from tests import options
from os import path


@dataclass
class CheckCase:
    """Test case."""

    name: str
    pattern: str
    document: str
    expected: bool


cases = [
    CheckCase(
        "simple",
        r"!x{a}",
        "check_a",
        True,
    ),
    CheckCase(
        "empty match",
        r"abba",
        "check_a",
        True,
    ),
    CheckCase(
        "no matches",
        r"!x{docx}",
        "check_b",
        False,
    ),
]


@pytest.mark.parametrize(
    "test_case",
    cases,
    ids=[c.name for c in cases],
)
def test_check(test_case: CheckCase):
    """Test check"""
    regex = re.reql(test_case.pattern)
    reader = re.Reader(path.join(options.DOCUMENTS_PATH, f"{test_case.document}.txt"))
    actual = regex.check(reader)

    assert actual == test_case.expected
