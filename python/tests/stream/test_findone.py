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
        "simple",
        r"(public|private)\s+\w+\s+!fun{\w+}\(([^,)]+,){2,}[^,)]+\)",
        "findone_a",
        Match({"fun": (247, 252)}),
    ),
]


@pytest.mark.parametrize(
    "test_case",
    cases,
    ids=[c.name for c in cases],
)
def test_findone(test_case: FindoneCase):
    """Test findone"""
    regex = re.reql(test_case.pattern)
    reader = re.Reader(f"{options.DOCUMENTS_PATH}/{test_case.document}.txt")

    try:
        actual = regex.findone(reader)
    except re.REmatchException:
        actual = None

    assert actual == test_case.expected
