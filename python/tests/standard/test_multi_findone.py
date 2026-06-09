from dataclasses import dataclass
import pyrematch as re
import pytest
from tests.match_mock import MultiMatch


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
        "one two three",
        MultiMatch({"x": [(0, 3), (4, 7)]}),
    ),
    MultiFindoneCase(
        "multi findone no output",
        r"!x{b}!x{b}",
        "aabaab",
        None,
    ),
    MultiFindoneCase(
        "multi findone empty match",
        r"!x{b}|a",
        "a",
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

    actual = regex.findone(test_case.document)
    assert actual == test_case.expected
