import pyrematch as re
from tests.match_mock import Match
from tests import options
import pytest
from os import path

def test_match_split_in_buffer():
    """test_match_split_in_buffer"""
    pattern = r"!x{hijklm}"
    query = re.reql(pattern, stream_buffer_size=30)
    reader = re.Reader(path.join(options.DOCUMENTS_PATH, "buffer_b.txt"))

    expected = [
        Match({"x": (7, 13)}),
        Match({"x": (34, 40)}),
        Match({"x": (61, 67)}),
        Match({"x": (88, 94)}),
    ]
    expected_group = "hijklm"

    for i, m in enumerate(query.finditer(reader)):
        assert m == expected[i]
        assert m.group("x") == expected_group


def test_match_split_in_buffer_lbl():
    """test_match_split_in_buffer_lbl"""
    pattern = r"!x{a\w{9}}"
    query = re.reql(pattern, flags=re.Flags.LINE_BY_LINE, stream_buffer_size=30)
    reader = re.Reader(path.join(options.DOCUMENTS_PATH, "buffer_b.txt"))

    expected = [
        Match({"x": (0, 10)}),
        Match({"x": (27, 37)}),
        Match({"x": (54, 64)}),
        Match({"x": (81, 91)}),
    ]
    expected_group = "abcdefghij"

    for i, m in enumerate(query.finditer(reader)):
        assert m == expected[i]
        assert m.group("x") == expected_group


def test_line_and_buffer_same_size():
    """test_line_and_buffer_same_size"""
    pattern = r"!x{456}"
    query = re.reql(pattern, flags=re.Flags.LINE_BY_LINE, stream_buffer_size=10)
    reader = re.Reader(path.join(options.DOCUMENTS_PATH, "buffer_a.txt"))

    expected = [
        Match({"x": (4, 7)}),
        Match({"x": (15, 18)}),
        Match({"x": (26, 29)}),
        Match({"x": (37, 40)}),
    ]
    expected_group = "456"

    for i, m in enumerate(query.finditer(reader)):
        assert m == expected[i]
        assert m.group("x") == expected_group


def test_match_end_of_line():
    """test_match_end_of_line"""
    pattern = r"!x{789}"
    query = re.reql(pattern, flags=re.Flags.LINE_BY_LINE, stream_buffer_size=10)
    reader = re.Reader(path.join(options.DOCUMENTS_PATH, "buffer_a.txt"))

    expected = [
        Match({"x": (7, 10)}),
        Match({"x": (18, 21)}),
        Match({"x": (29, 32)}),
        Match({"x": (41, 44)}),
    ]
    expected_group = "789"

    for i, m in enumerate(query.finditer(reader)):
        assert m == expected[i]
        assert m.group("x") == expected_group


def test_line_does_not_fit_in_buffer():
    """test_line_does_not_fit_in_buffer"""
    pattern = r"!x{\w{3}}"
    query = re.reql(pattern, flags=re.Flags.LINE_BY_LINE, stream_buffer_size=9)
    reader = re.Reader(path.join(options.DOCUMENTS_PATH, "buffer_a.txt"))

    pytest.raises(re.REmatchException, query.findall, reader)
