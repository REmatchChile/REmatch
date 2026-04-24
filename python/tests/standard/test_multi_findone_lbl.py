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
        "multi findone lbl",
        r"!x{.}!x{.}",
        "0\n23\n56\n",
        MultiMatch({"x": [(2, 3), (3, 4)]}),
    ),
    MultiFindoneCase(
        "multi findone lbl anchors",
        r"^!x{.}+",
        "01\n34\n67\n",
        MultiMatch({"x": [(0, 1)]}),
    ),
    MultiFindoneCase(
        "multi findone lbl anchors",
        r"!x{.}+$",
        "01\n34\n67\n",
        MultiMatch({"x": [(1, 2)]}),
    ),
]


@pytest.mark.parametrize(
    "test_case",
    cases,
    ids=[c.name for c in cases],
)
def test_multi_findone(test_case: MultiFindoneCase):
    """Test multi findone lbl"""
    regex = re.multi_reql(test_case.pattern, re.Flags.LINE_BY_LINE)

    try:
        actual = regex.findone(test_case.document)
    except re.REmatchException:
        actual = None

    assert actual == test_case.expected
