from dataclasses import dataclass
import pyrematch as re
import pytest
from tests.match_mock import Match


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
        "0\n2345\n",
        Match({"x": (2, 4)}),
    ),
    FindoneCase(
        "findone lbl no match found",
        r"!x{.{4,}}",
        "01\n345\n",
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

    try:
        actual = regex.findone(test_case.document)
    except re.REmatchException:
        actual = None

    assert actual == test_case.expected
