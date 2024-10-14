import enum
from typing import List, Tuple, Union

from ._pyrematch import (
    DEFAULT_MAX_DETERMINISTIC_STATES,
    DEFAULT_MAX_MEMPOOL_DUPLICATIONS,
    cppFlags,
    cppMatch,
    cppMatchIterator,
    cppmulti_reql,
    cppMultiMatch,
    cppMultiMatchIterator,
    cppMultiQuery,
    cppQuery,
    cppreql,
)


class Flags(enum.Flag):
    NONE = enum.auto()
    LINE_BY_LINE = enum.auto()

    def _as_cpp_flags(self) -> cppFlags:
        cpp_flags = cppFlags.NONE
        if Flags.LINE_BY_LINE in self:
            cpp_flags |= cppFlags.LINE_BY_LINE
        return cpp_flags


class Match:
    def __init__(self, cpp_match: cppMatch):
        self._cpp_match: cppMatch = cpp_match

    def start(self, key: Union[str, int]) -> int:
        return self._cpp_match.start(key)

    def end(self, key: Union[str, int]) -> int:
        return self._cpp_match.end(key)

    def span(self, key: Union[str, int]) -> Tuple[int, int]:
        return self._cpp_match.span(key)

    def group(self, key: Union[str, int]) -> str:
        return self._cpp_match.group(key)

    def groupdict(self) -> dict:
        res = {}
        for variable in self._cpp_match.variables():
            res[variable] = self.group(variable)
        return res

    def variables(self) -> List[str]:
        return self._cpp_match.variables()

    def empty(self) -> bool:
        return self._cpp_match.empty()

    def __str__(self) -> str:
        return self._cpp_match.to_string()

    def __repr__(self) -> str:
        return str(self)


class MatchIterator:
    def __init__(self, cpp_match_iterator: cppMatchIterator):
        self._cpp_match_iterator: cppMatchIterator = cpp_match_iterator

    def __iter__(self) -> "MatchIterator":
        return self

    def __next__(self) -> Match:
        cpp_match = self._cpp_match_iterator.next()
        if cpp_match:
            return Match(cpp_match)
        raise StopIteration

    def variables(self) -> List[str]:
        return self._cpp_match_iterator.variables()


class Query:
    def __init__(
        self,
        cpp_query: cppQuery,
    ):
        self._cpp_query: cppQuery = cpp_query

    def findone(self, document: str) -> Match:
        cpp_match = self._cpp_query.findone(document)
        if cpp_match:
            return Match(cpp_match)
        return None

    def findmany(self, document: str, limit: int):
        return [
            Match(cpp_match) for cpp_match in self._cpp_query.findmany(document, limit)
        ]

    def findall(self, document: str) -> List[Match]:
        return [Match(cpp_match) for cpp_match in self._cpp_query.findall(document)]

    def finditer(self, document: str) -> MatchIterator:
        cpp_match_iterator = self._cpp_query.finditer(document)
        return MatchIterator(cpp_match_iterator)

    def check(self, document: str) -> bool:
        return self._cpp_query.check(document)


def reql(
    pattern: str,
    flags: Flags = Flags.NONE,
    max_mempool_duplications=DEFAULT_MAX_MEMPOOL_DUPLICATIONS,
    max_deterministic_states=DEFAULT_MAX_DETERMINISTIC_STATES,
) -> Query:
    cpp_query = cppreql(
        pattern,
        flags._as_cpp_flags(),
        max_mempool_duplications,
        max_deterministic_states,
    )
    return Query(cpp_query)


class MultiMatch:
    def __init__(self, cpp_multi_match: cppMultiMatch):
        self._cpp_multi_match: cppMultiMatch = cpp_multi_match

    def spans(self, key: Union[str, int]) -> List[Tuple[int, int]]:
        return self._cpp_multi_match.spans(key)

    def groups(self, key: Union[str, int]) -> List[str]:
        return self._cpp_multi_match.groups(key)

    def groupdict(self) -> dict:
        pass

    def submatch(self, span: Tuple[int, int]) -> "MultiMatch":
        return MultiMatch(self._cpp_multi_match.submatch(span))

    def empty(self) -> bool:
        return self._cpp_multi_match.empty()

    def variables(self) -> List[str]:
        return self._cpp_multi_match.variables()

    def __str__(self) -> str:
        return self._cpp_multi_match.to_string()

    def __repr__(self) -> str:
        return str(self)


class MultiMatchIterator:
    def __init__(self, cpp_multi_match_iterator: cppMultiMatchIterator):
        self._multi_match_iterator: cppMultiMatchIterator = cpp_multi_match_iterator

    def __iter__(self) -> "MultiMatchIterator":
        return self

    def __next__(self) -> MultiMatch:
        cpp_multi_match = self._multi_match_iterator.next()
        if cpp_multi_match:
            return MultiMatch(cpp_multi_match)
        raise StopIteration

    def variables(self) -> List[str]:
        return self._multi_match_iterator.variables()


class MultiQuery:
    def __init__(
        self,
        cpp_multi_query: cppMultiQuery,
    ):
        self._cpp_multi_query: cppMultiQuery = cpp_multi_query

    def findone(self, document: str) -> MultiMatch:
        cpp_multi_match = self._cpp_multi_query.findone(document)
        if cpp_multi_match:
            return MultiMatch(cpp_multi_match)
        return None

    def findmany(self, document: str, limit: int):
        return [
            MultiMatch(cpp_multi_match)
            for cpp_multi_match in self._cpp_multi_query.findmany(document, limit)
        ]

    def findall(self, document: str) -> List[Match]:
        return [
            MultiMatch(cpp_multi_match)
            for cpp_multi_match in self._cpp_multi_query.findall(document)
        ]

    def finditer(self, document: str) -> MultiMatchIterator:
        cpp_multi_match_iterator = self._cpp_multi_query.finditer(document)
        return MultiMatchIterator(cpp_multi_match_iterator)

    def check(self, document: str) -> bool:
        return self._cpp_multi_query.check(document)


def multi_reql(
    pattern: str,
    flags: Flags = Flags.NONE,
    max_mempool_duplications=DEFAULT_MAX_MEMPOOL_DUPLICATIONS,
    max_deterministic_states=DEFAULT_MAX_DETERMINISTIC_STATES,
) -> MultiQuery:
    cpp_multi_query = cppmulti_reql(
        pattern,
        flags._as_cpp_flags(),
        max_mempool_duplications,
        max_deterministic_states,
    )
    return MultiQuery(cpp_multi_query)
