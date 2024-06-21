import enum
from typing import List, Tuple, Union

from ._pyrematch import (
    PyFlags,
    PyMatch,
    PyMatchIterator,
    PyMultiQuery,
    PyMultiMatch,
    PyMultiMatchIterator,
    PyQuery,
)


class Flags(enum.Flag):
    NOFLAG = enum.auto()
    LINE_BY_LINE = enum.auto()
    MULTI_MATCH = enum.auto()

    def _get_pyflags(self) -> "PyFlags":
        pyflags = PyFlags()
        pyflags.line_by_line = Flags.LINE_BY_LINE in self

        return pyflags


def reql(
    pattern: str,
    flags: Flags = Flags.NOFLAG,
    max_mempool_duplications=8,
    max_deterministic_states=1000,
):
    if Flags.MULTI_MATCH in flags:
        return MultiQuery(
            pattern, flags, max_mempool_duplications, max_deterministic_states
        )
    return Query(pattern, flags, max_mempool_duplications, max_deterministic_states)


class Match:
    def __init__(self, pymatch: "PyMatch"):
        self._pymatch = pymatch

    def start(self, variable: Union[str, int]) -> int:
        return self._pymatch.start(variable)

    def end(self, variable: Union[str, int]) -> int:
        return self._pymatch.end(variable)

    def span(self, variable: Union[str, int]) -> Tuple[int, int]:
        return self._pymatch.span(variable)

    def group(self, variable: Union[str, int]) -> str:
        return self._pymatch.group(variable)

    def empty(self) -> bool:
        return self._pymatch.empty()

    def variables(self) -> List[str]:
        return self._pymatch.variables()

    def __repr__(self) -> str:
        variable_repr = [
            f"{variable} -> [{self.start(variable)}, {self.end(variable)}>"
            for variable in self.variables()
        ]

        if variable_repr:
            return "\n".join(variable_repr)

        return "Empty match"


class MatchIterator:
    def __init__(self, document: str, pyquery: "PyQuery"):
        self._match_iterator: "PyMatchIterator" = pyquery.finditer(document)

    def __iter__(self) -> "MatchIterator":
        return self

    def __next__(self) -> Match:
        pymatch = self._match_iterator.next()
        if pymatch:
            return Match(pymatch)
        raise StopIteration

    def variables(self) -> List[str]:
        return self._match_iterator.variables()


class Query:
    def __init__(
        self,
        pattern: str,
        flags: Flags = Flags.NOFLAG,
        max_mempool_duplications=8,
        max_deterministic_states=1000,
    ):
        pyflags = flags._get_pyflags()
        pyflags.max_mempool_duplications = max_mempool_duplications
        pyflags.max_deterministic_states = max_deterministic_states
        self._pyquery: "PyQuery" = PyQuery(pattern, pyflags)

    def finditer(self, document: str) -> MatchIterator:
        return MatchIterator(document, self._pyquery)

    def findone(self, document: str) -> "Match":
        pymatch = self._pyquery.findone(document)
        if pymatch:
            return Match(self._pyquery.findone(document))
        return None

    def check(self, document: str) -> bool:
        return self._pyquery.check(document)


class MultiMatch:
    def __init__(self, pymatch: "PyMultiMatch"):
        self._pymatch = pymatch

    def spans(self, variable: Union[str, int]) -> List[Tuple[int, int]]:
        return self._pymatch.spans(variable)

    def groups(self, variable: Union[str, int]) -> List[str]:
        return self._pymatch.groups(variable)

    def submatch(self, span: Tuple[int, int]) -> "MultiMatch":
        return MultiMatch(self._pymatch.submatch(span))

    def empty(self) -> bool:
        return self._pymatch.empty()


class MultiMatchIterator:
    def __init__(self, document: str, pyquery: "PyMultiQuery"):
        self._match_iterator: "PyMultiMatchIterator" = pyquery.finditer(document)

    def __iter__(self) -> "MultiMatchIterator":
        return self

    def __next__(self) -> MultiMatch:
        pymatch = self._match_iterator.next()
        if pymatch:
            return MultiMatch(pymatch)
        raise StopIteration

    def variables(self) -> List[str]:
        return self._match_iterator.variables()


class MultiQuery:
    def __init__(
        self,
        pattern: str,
        flags: Flags = Flags.NOFLAG,
        max_mempool_duplications=8,
        max_deterministic_states=1000,
    ):
        pyflags = flags._get_pyflags()
        pyflags.max_mempool_duplications = max_mempool_duplications
        pyflags.max_deterministic_states = max_deterministic_states
        self._pyquery: "PyMultiQuery" = PyMultiQuery(pattern, pyflags)

    def finditer(self, document: str) -> MultiMatchIterator:
        return MultiMatchIterator(document, self._pyquery)

    def findone(self, document: str) -> "MultiMatch":
        pymatch = self._pyquery.findone(document)
        if pymatch:
            return MultiMatch(pymatch)
        return None

    def check(self, document: str) -> bool:
        return self._pyquery.check(document)
