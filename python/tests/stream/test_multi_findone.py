from dataclasses import dataclass
import pyrematch as re
import pytest
from tests.match_mock import MultiMatch
from tests import options


@dataclass
class MultiFindoneCase:
    """Test case."""

    name: str
    pattern: str
    document: str
    expected: MultiMatch | None


cases = [
    MultiFindoneCase(
        "multi findone",
        r"((^|[^\w])!x{\w+} !x{\w+}($|[^\w]))",
        "multi_findone_a",
        MultiMatch({"x": [(0, 3), (4, 7)]}),
    ),
    MultiFindoneCase(
        "multi findone no output",
        r"((^|[^\w])!x{\w+} !x{\w+}a($|[^\w]))",
        "multi_findone_a",
        None,
    ),
    MultiFindoneCase(
        "multi findone empty match",
        r"!x{b}|one",
        "multi_findone_a",
        MultiMatch(),
    ),
]


@pytest.mark.parametrize(
    "test_case",
    cases,
    ids=[c.name for c in cases],
)
def test_multi_findone(test_case: MultiFindoneCase):
    regex = re.multi_reql(test_case.pattern)
    reader = re.Reader(f"{options.DOCUMENTS_PATH}/{test_case.document}.txt")

    try:
        actual = regex.findone(reader)
    except re.REmatchException:
        actual = None

    assert actual == test_case.expected
