import pytest
import pyrematch as re
from match_mock import MatchMock


def test_flag_line_by_line():
    regex = re.Regex(r"!name{([A-Z]\w+,\n)+}", re.Flags.LINE_BY_LINE)
    iterator = regex.finditer("\nName,\nJohn,\nAlice,\nBob,\nEmma,\n")
    expected = [
        MatchMock({"name": (1, 7)}),
        MatchMock({"name": (7, 13)}),
        MatchMock({"name": (13, 20)}),
        MatchMock({"name": (20, 25)}),
        MatchMock({"name": (25, 31)}),
    ]
    for match in iterator:
        assert match in expected, f"Match {match} was not expected"
        expected.remove(match)

    assert (
        len(expected) == 0
    ), f"Expected matches were not found in the document: {expected}"


def test_flag_max_deterministic_states():
    pattern = "!x{" + "a" * 100 + "}"
    document = "a" * 20000

    regex = re.Regex(pattern, max_deterministic_states=200)
    match_iterator = regex.finditer(document)

    with pytest.raises(re.ComplexQueryException):
        for _ in match_iterator:
            pass


def test_flag_max_mempool_duplications():
    pattern = "!x{.+}"
    document = "a" * 200

    regex = re.Regex(pattern, max_mempool_duplications=0)
    match_iterator = regex.finditer(document)

    with pytest.raises(re.MemoryLimitExceededException):
        for _ in match_iterator:
            pass
