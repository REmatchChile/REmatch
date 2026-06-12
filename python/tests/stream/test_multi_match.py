from os import path
import pyrematch as re
import pytest
from tests import options


@pytest.fixture(name="match")
def fixture_match():
    """fixture_match"""
    reader = re.Reader(path.join(options.DOCUMENTS_PATH, "multi_match_a.txt"))
    query = re.multi_reql(r"!x{\w{3,}}!x{.}")
    return query.findone(reader)


def test_match_groups(match):
    """test_match_groups"""
    expected = ["bb_", "_"]
    assert match.groups("x") == expected


def test_match_copy(match):
    """test_match_copy"""
    match2 = match
    expected = ["bb_", "_"]
    assert match.groups("x") == expected
    assert match2.groups("x") == expected


def test_match_span_functions(match):
    """test_match_span_functions"""
    assert match.empty() is False
    assert match.variables() == ["x"]
    assert match.spans("x") == [(4, 7), (7, 8)]
    assert match.groupdict() == {"x": ["bb_", "_"]}


def test_match_var_not_found(match):
    """test_match_var_not_found"""
    pytest.raises(re.REmatchException, match.spans, "a")
    pytest.raises(re.REmatchException, match.spans, 1)
    pytest.raises(re.REmatchException, match.groups, 1)
    pytest.raises(re.REmatchException, match.groups, 1)


@pytest.fixture(name="multi_match")
def fixture_multi_match():
    """fixture_multi_match"""
    pattern = r"(^|(\. ))!x{!y{\w+}([^\w.]+!y{\w+})*}\."
    reader = re.Reader(path.join(options.DOCUMENTS_PATH, "multi_match_b.txt"))
    query = re.multi_reql(pattern)
    return query.findone(reader)


def test_submatch_spans(multi_match):
    """test_submatch_spans"""
    submatch = multi_match.submatch((3, 10))
    assert submatch.spans("x") == []
    assert submatch.spans("y") == [(5, 9)]


def test_submatch_groups(multi_match):
    """test_submatch_groups"""
    submatch = multi_match.submatch((3, 10))
    assert submatch.groups("x") == []
    assert submatch.groups("y") == ["calm"]
