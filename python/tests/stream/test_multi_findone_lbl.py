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
        "multi findone lbl",
        r"!x{.}!x{.}",
        "multi_findone_lbl_a",
        MultiMatch({"x": [(2, 3), (3, 4)]}),
    ),
    MultiFindoneCase(
        "multi findone lbl anchors",
        r"^!x{.{2}}",
        "multi_findone_lbl_a",
        MultiMatch({"x": [(2, 4)]}),
    ),
    MultiFindoneCase(
        "multi findone lbl anchors",
        r"!x{.}!x{.}$",
        "multi_findone_lbl_a",
        MultiMatch({"x": [(3, 4), (4, 5)]}),
    ),
]


@pytest.mark.parametrize(
    "test_case",
    cases,
    ids=[c.name for c in cases],
)
def test_multi_findone_lbl(test_case: MultiFindoneCase):
    """Test multi findone lbl"""
    regex = re.multi_reql(test_case.pattern, re.Flags.LINE_BY_LINE)
    reader = re.Reader(f"{options.DOCUMENTS_PATH}/{test_case.document}.txt")

    actual = regex.findone(reader)
    assert actual == test_case.expected
