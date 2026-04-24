from dataclasses import dataclass
import pyrematch as re
import pytest
from tests.match_mock import MultiMatch


@dataclass
class MultiFinditerCase:
    """Test case."""

    name: str
    pattern: str
    document: str
    expected: list[MultiMatch]


cases = [
    MultiFinditerCase(
        "multi finditer lbl",
        r"!x{.}!x{.}",
        "01\n34\n67\n",
        [
            MultiMatch({"x": [(0, 1), (1, 2)]}),
            MultiMatch({"x": [(3, 4), (4, 5)]}),
            MultiMatch({"x": [(6, 7), (7, 8)]}),
        ],
    ),
    MultiFinditerCase(
        "multi finditer lbl anchors",
        r"^!x{.}+",
        "01\n34\n67\n",
        [
            MultiMatch({"x": [(0, 1)]}),
            MultiMatch({"x": [(0, 1), (1, 2)]}),
            MultiMatch({"x": [(3, 4)]}),
            MultiMatch({"x": [(3, 4), (4, 5)]}),
            MultiMatch({"x": [(6, 7)]}),
            MultiMatch({"x": [(6, 7), (7, 8)]}),
        ],
    ),
    MultiFinditerCase(
        "multi finditer lbl anchors",
        r"!x{.}+$",
        "01\n34\n67\n",
        [
            MultiMatch({"x": [(0, 1), (1, 2)]}),
            MultiMatch({"x": [(1, 2)]}),
            MultiMatch({"x": [(3, 4), (4, 5)]}),
            MultiMatch({"x": [(4, 5)]}),
            MultiMatch({"x": [(6, 7), (7, 8)]}),
            MultiMatch({"x": [(7, 8)]}),
        ],
    ),
]


@pytest.mark.parametrize(
    "test_case",
    cases,
    ids=[c.name for c in cases],
)
def test_multi_finditer_lbl(test_case: MultiFinditerCase):
    """Test multi finditer lbl"""
    regex = re.multi_reql(test_case.pattern, re.Flags.LINE_BY_LINE)
    iterator = regex.finditer(test_case.document)

    for match in iterator:
        assert match in test_case.expected, f"Match {match} was not expected"
        test_case.expected.remove(match)

    assert (
        len(test_case.expected) == 0
    ), f"Expected matches were not found in the document: {test_case.expected}"
