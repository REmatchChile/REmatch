# https://docs.python.org/3/library/re.html

import enum
from typing import Tuple

from ._pyrematch import PyFlags, PyMatch, PyMatchIterator, PyRegex


class Flags(enum.IntFlag):
    def __init__(self):
        # TODO:
        pass

    def _get_pyflags(self) -> "PyFlags":
        # TODO:
        return None


class Match:
    def __init__(self, pymatch: "PyMatch"):
        self._pymatch = pymatch

    def start(self, variable: str | int) -> int:
        return self._pymatch.start(variable)

    def end(self, variable: str | int) -> int:
        return self._pymatch.end(variable)

    def span(self, variable: str | int) -> Tuple[int, int]:
        return self._pymatch.span(variable)


class MatchIterator:
    def __init__(self, document: str, pyregex: "PyRegex"):
        self._match_iterator: "PyMatchIterator" = pyregex.finditer(document)

    def __iter__(self) -> "MatchIterator":
        return self

    def __next__(self) -> "Match":
        pymatch = self._match_iterator.next()
        if pymatch:
            return Match(pymatch)
        raise StopIteration


class Regex:
    def __init__(self, pattern: str, flags: Flags = None):
        if flags is None:
            pyflags = PyFlags()
        else:
            pyflags = flags._get_pyflags()
        self._pyregex: "PyRegex" = PyRegex(pattern, pyflags)

    def finditer(self, document: str) -> "MatchIterator":
        return MatchIterator(document, self._pyregex)
