from dataclasses import dataclass
import pyrematch as re
import pytest
from tests.match_mock import Match
from tests import options
from os import path


@dataclass
class FinditerCase:
    """Test case."""

    name: str
    pattern: str
    document: str
    expected: list[Match]


test_regex_finditer_cases = [
    FinditerCase(
        "lbl simple",
        r"!x{.+}",
        "finditer_lbl_a",
        [
            Match({"x": (0, 1)}),
            Match({"x": (2, 3)}),
            Match({"x": (2, 4)}),
            Match({"x": (3, 4)}),
            Match({"x": (5, 6)}),
        ],
    ),
]


@pytest.mark.parametrize(
    "test_case",
    test_regex_finditer_cases,
    ids=[c.name for c in test_regex_finditer_cases],
)
def test_finditer_lbl(test_case: FinditerCase):
    """Test finditer lbl"""
    regex = re.reql(test_case.pattern, re.Flags.LINE_BY_LINE)
    reader = re.Reader(path.join(options.DOCUMENTS_PATH, f"{test_case.document}.txt"))
    iterator = regex.finditer(reader)

    for match in iterator:
        assert match in test_case.expected, f"Match {match} was not expected"
        test_case.expected.remove(match)

    assert (
        len(test_case.expected) == 0
    ), f"Expected matches were not found in the document: {test_case.expected}"
