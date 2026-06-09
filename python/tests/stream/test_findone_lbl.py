from dataclasses import dataclass
import pyrematch as re
import pytest
from tests.match_mock import Match
from tests import options


@dataclass
class FindoneCase:
    """Test case."""

    name: str
    pattern: str
    document: str
    expected: Match | None


cases = [
    FindoneCase(
        "findone lbl",
        r"!x{.{2}}",
        "findone_lbl_a",
        Match({"x": (2, 4)}),
    ),
    FindoneCase(
        "findone lbl no match found",
        r"!x{.{4,}}",
        "findone_lbl_a",
        None,
    ),
]


@pytest.mark.parametrize(
    "test_case",
    cases,
    ids=[c.name for c in cases],
)
def test_findone_lbl(test_case: FindoneCase):
    """Test findone lbl"""
    regex = re.reql(test_case.pattern, re.Flags.LINE_BY_LINE)
    reader = re.Reader(f"{options.DOCUMENTS_PATH}/{test_case.document}.txt")

    actual = regex.findone(reader)
    assert actual == test_case.expected
