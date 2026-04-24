from dataclasses import dataclass
import pyrematch as re
import pytest
from tests import options


@dataclass
class CheckCase:
    """Test case."""

    name: str
    pattern: str
    document: str
    expected: bool


cases = [
    CheckCase(
        "multi check",
        r"!x{.{12}}",
        "multi_check_lbl_a",
        True,
    ),
    CheckCase(
        "multi check anchors",
        r"!x{.{12}}$",
        "multi_check_lbl_a",
        True,
    ),
    CheckCase(
        "multi check no output",
        r"!x{multi_reql}",
        "multi_check_lbl_a",
        False,
    ),
]


@pytest.mark.parametrize(
    "test_case",
    cases,
    ids=[c.name for c in cases],
)
def test_multi_check_lbl(test_case: CheckCase):
    """Test multi check lbl"""
    regex = re.multi_reql(test_case.pattern, re.Flags.LINE_BY_LINE)
    reader = re.Reader(f"{options.DOCUMENTS_PATH}/{test_case.document}.txt")
    actual = regex.check(reader)

    assert actual == test_case.expected
