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
        "check lbl is true",
        r"!x{.+}",
        "check_lbl_a",
        True,
    ),
    CheckCase(
        "check lbl is false",
        r"!x{.{3}}",
        "check_lbl_a",
        False,
    ),
]


@pytest.mark.parametrize(
    "test_case",
    cases,
    ids=[c.name for c in cases],
)
def test_check_lbl(test_case: CheckCase):
    """Test check lbl"""
    regex = re.reql(test_case.pattern, re.Flags.LINE_BY_LINE)
    reader = re.Reader(path.join(options.DOCUMENTS_PATH, f"{test_case.document}.txt"))
    actual = regex.check(reader)

    assert actual == test_case.expected
