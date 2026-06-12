from dataclasses import dataclass
from os import path
import pyrematch as re
import pytest
from tests.match_mock import Match
from tests import options


@dataclass
class FinditerCase:
    """Test case."""

    name: str
    pattern: str
    document: str
    expected: list[Match]


test_regex_finditer_cases = [
    FinditerCase(
        "simple",
        r"(public|private)\s+\w+\s+!fun{\w+}\(([^,)]+,){2,}[^,)]+\)",
        "finditer_a",
        [
            Match({"fun": (247, 252)}),
            Match({"fun": (387, 392)}),
        ],
    ),
]


@pytest.mark.parametrize(
    "test_case",
    test_regex_finditer_cases,
    ids=[c.name for c in test_regex_finditer_cases],
)
def test_finditer(test_case: FinditerCase):
    """Test finditer"""
    regex = re.reql(test_case.pattern)
    reader = re.Reader(path.join(options.DOCUMENTS_PATH, f"{test_case.document}.txt"))
    iterator = regex.finditer(reader)

    for match in iterator:
        assert match in test_case.expected, f"Match {match} was not expected"
        test_case.expected.remove(match)

    assert (
        len(test_case.expected) == 0
    ), f"Expected matches were not found in the document: {test_case.expected}"
