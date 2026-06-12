import pyrematch as re
import pytest


@pytest.fixture(name="match")
def fixture_match():
    """fixture_match"""
    document = "aa--bb__cc"
    query = re.multi_reql(r"!x{\w{3,}}!x{.}")
    return query.findone(document)


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
    document = "Keep calm. Use MultiQuery."
    query = re.multi_reql(pattern)
    return query.findone(document)


def test_submatch_spans(multi_match):
    """test_submatch_spans"""
    submatch = multi_match.submatch((3, 10))
    assert submatch.spans("x") == []
    assert submatch.spans("y") == [(5, 9)]


def test_submatch_groups(multi_match):
    """test_submatch_spans"""
    submatch = multi_match.submatch((3, 10))
    assert submatch.groups("x") == []
    assert submatch.groups("y") == ["calm"]
