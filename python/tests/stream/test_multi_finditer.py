from dataclasses import dataclass
import pyrematch as re
import pytest
from tests.match_mock import MultiMatch
from tests import options
from os import path


@dataclass
class MultiFinditerCase:
    """Test case."""

    name: str
    pattern: str
    document: str
    expected: list[MultiMatch]


cases = [
    MultiFinditerCase(
        "multi finditer",
        r"((^|[^\w])!x{\w+} !x{\w+}($|[^\w]))",
        "multi_finditer_a",
        [
            MultiMatch({"x": [(0, 3), (4, 7)]}),
            MultiMatch({"x": [(4, 7), (8, 13)]}),
        ],
    ),
    MultiFinditerCase(
        "multi finditer no output",
        r"!x{b}!x{b}",
        "multi_finditer_a",
        [],
    ),
    MultiFinditerCase(
        "multi finditer empty match",
        r"!x{b}|two",
        "multi_finditer_a",
        [MultiMatch()],
    ),
]


@pytest.mark.parametrize(
    "test_case",
    cases,
    ids=[c.name for c in cases],
)
def test_multi_finditer(test_case: MultiFinditerCase):
    """Test multi finditer"""
    regex = re.multi_reql(test_case.pattern)
    reader = re.Reader(path.join(options.DOCUMENTS_PATH, f"{test_case.document}.txt"))
    iterator = regex.finditer(reader)

    for match in iterator:
        assert match in test_case.expected, f"Match {match} was not expected"
        test_case.expected.remove(match)

    assert (
        len(test_case.expected) == 0
    ), f"Expected matches were not found in the document: {test_case.expected}"
