import enum
from typing import List, Tuple, Union

from ._pyrematch import (
    PyFlags,
    PyMatch,
    PyMatchIterator,
    PyMultiRegex,
    PyMultiMatch,
    PyMultiMatchIterator,
    PyRegex,
)


class Flags(enum.Flag):
    NOFLAG = enum.auto()
    LINE_BY_LINE = enum.auto()
    EARLY_OUTPUT = enum.auto()

    def _get_pyflags(self) -> "PyFlags":
        pyflags = PyFlags()
        pyflags.line_by_line = Flags.LINE_BY_LINE in self
        pyflags.early_output = Flags.EARLY_OUTPUT in self

        return pyflags


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
    def __init__(self, document: str, pyregex: "PyRegex"):
        self._match_iterator: "PyMatchIterator" = pyregex.finditer(document)

    def __iter__(self) -> "MatchIterator":
        return self

    def __next__(self) -> Match:
        pymatch = self._match_iterator.next()
        if pymatch:
            return Match(pymatch)
        raise StopIteration

    def variables(self) -> List[str]:
        return self._match_iterator.variables()


class Regex:
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
        self._pyregex: "PyRegex" = PyRegex(pattern, pyflags)

    def finditer(self, document: str) -> MatchIterator:
        return MatchIterator(document, self._pyregex)

    def findone(self, document: str) -> "Match":
        return Match(self._pyregex.findone(document))

    def check(self, document: str) -> bool:
        return self._pyregex.check(document)


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
    def __init__(self, document: str, pyregex: "PyMultiRegex"):
        self._match_iterator: "PyMultiMatchIterator" = pyregex.finditer(document)

    def __iter__(self) -> "MultiMatchIterator":
        return self

    def __next__(self) -> MultiMatch:
        pymatch = self._match_iterator.next()
        if pymatch:
            return MultiMatch(pymatch)
        raise StopIteration

    def variables(self) -> List[str]:
        return self._match_iterator.variables()


class MultiRegex:
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
        self._pyregex: "PyMultiRegex" = PyMultiRegex(pattern, pyflags)

    def finditer(self, document: str) -> MultiMatchIterator:
        return MultiMatchIterator(document, self._pyregex)

    def findone(self, document: str) -> "MultiMatch":
        return MultiMatch(self._pyregex.findone(document))

    def check(self, document: str) -> bool:
        return self._pyregex.check(document)
