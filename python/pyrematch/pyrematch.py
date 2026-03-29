import enum
from typing import Union

from ._pyrematch import (
    DEFAULT_MAX_DETERMINISTIC_STATES,
    DEFAULT_MAX_MEMPOOL_DUPLICATIONS,
    DEFAULT_STREAM_BUFFER_SIZE,
    cppFlags,
    cppMatch,
    cppMatchGenerator,
    cppIterator,
    cppmulti_reql,
    cppMultiMatch,
    cppMultiMatchGenerator,
    cppMultiIterator,
    cppMultiQuery,
    cppQuery,
    cppreql,
    cppReaderWrapper,
)


class Flags(enum.Flag):
    NONE = enum.auto()
    LINE_BY_LINE = enum.auto()

    def _as_cpp_flags(self) -> cppFlags:
        cpp_flags = cppFlags.NONE
        if Flags.LINE_BY_LINE in self:
            cpp_flags |= cppFlags.LINE_BY_LINE
        return cpp_flags


class Reader:
    def __init__(self, path):
        self._cpp_reader = cppReaderWrapper(path)


class Match:
    def __init__(self, cpp_match: cppMatch):
        self._cpp_match: cppMatch = cpp_match

    def start(self, key: Union[str, int]) -> int:
        return self._cpp_match.start(key)

    def end(self, key: Union[str, int]) -> int:
        return self._cpp_match.end(key)

    def span(self, key: Union[str, int]) -> tuple[int, int]:
        return self._cpp_match.span(key)

    def group(self, key: Union[str, int]) -> str:
        return self._cpp_match.group(key)

    def groupdict(self) -> dict:
        res = {}
        for variable in self._cpp_match.variables():
            res[variable] = self.group(variable)
        return res

    def variables(self) -> list[str]:
        return self._cpp_match.variables()

    def empty(self) -> bool:
        return self._cpp_match.empty()

    def __str__(self) -> str:
        return self._cpp_match.to_string()

    def __repr__(self) -> str:
        return str(self)


class MatchGenerator:
    def __init__(self, cpp_match_generator: cppMatchGenerator):
        self._cpp_it: cppIterator = cpp_match_generator.begin()

    def __iter__(self) -> "MatchGenerator":
        return self

    def __next__(self) -> Match:
        if self._cpp_it.has_value():
            cpp_match = self._cpp_it.get()
            self._cpp_it.next()
            return Match(cpp_match)
        raise StopIteration


class Query:
    def __init__(
        self,
        cpp_query: cppQuery,
    ):
        self._cpp_query: cppQuery = cpp_query

    def findone(self, document: Union[str, Reader]) -> Match:
        if isinstance(document, Reader):
            cpp_match = self._cpp_query.findone(document._cpp_reader)
        else:
            cpp_match = self._cpp_query.findone(document)
        return Match(cpp_match)

    def findmany(self, document: Union[str, Reader], limit: int):
        if isinstance(document, Reader):
            cpp_matches = self._cpp_query.findmany(document._cpp_reader, limit)
        else:
            cpp_matches = self._cpp_query.findmany(document, limit)
        return [Match(cpp_match) for cpp_match in cpp_matches]

    def findall(self, document: Union[str, Reader]) -> list[Match]:
        if isinstance(document, Reader):
            cpp_matches = self._cpp_query.findall(document._cpp_reader)
        else:
            cpp_matches = self._cpp_query.findall(document)
        return [Match(cpp_match) for cpp_match in cpp_matches]

    def finditer(self, document: Union[str, Reader]) -> MatchGenerator:
        if isinstance(document, Reader):
            cpp_match_generator = self._cpp_query.finditer(document._cpp_reader)
        else:
            cpp_match_generator = self._cpp_query.finditer(document)
        return MatchGenerator(cpp_match_generator)

    def check(self, document: Union[str, Reader]) -> bool:
        if isinstance(document, Reader):
            return self._cpp_query.check(document._cpp_reader)
        return self._cpp_query.check(document)

    def variables(self) -> list[str]:
        return self._cpp_query.variables()


def reql(
    pattern: str,
    flags: Flags = Flags.NONE,
    max_mempool_duplications=DEFAULT_MAX_MEMPOOL_DUPLICATIONS,
    max_deterministic_states=DEFAULT_MAX_DETERMINISTIC_STATES,
    stream_buffer_size=DEFAULT_STREAM_BUFFER_SIZE,
) -> Query:
    cpp_query = cppreql(
        pattern,
        flags._as_cpp_flags(),
        max_mempool_duplications,
        max_deterministic_states,
        stream_buffer_size,
    )
    return Query(cpp_query)


class MultiMatch:
    def __init__(self, cpp_multi_match: cppMultiMatch):
        self._cpp_multi_match: cppMultiMatch = cpp_multi_match

    def spans(self, key: Union[str, int]) -> list[tuple[int, int]]:
        return self._cpp_multi_match.spans(key)

    def groups(self, key: Union[str, int]) -> list[str]:
        return self._cpp_multi_match.groups(key)

    def groupdict(self) -> dict:
        pass

    def submatch(self, span: tuple[int, int]) -> "MultiMatch":
        return MultiMatch(self._cpp_multi_match.submatch(span))

    def empty(self) -> bool:
        return self._cpp_multi_match.empty()

    def variables(self) -> list[str]:
        return self._cpp_multi_match.variables()

    def __str__(self) -> str:
        return self._cpp_multi_match.to_string()

    def __repr__(self) -> str:
        return str(self)


class MultiMatchGenerator:
    def __init__(self, cpp_multi_match_generator: cppMultiMatchGenerator):
        self._cpp_it: cppMultiIterator = cpp_multi_match_generator.begin()

    def __iter__(self) -> "MatchGenerator":
        return self

    def __next__(self) -> Match:
        if self._cpp_it.has_value():
            cpp_multi_match = self._cpp_it.get()
            self._cpp_it.next()
            return MultiMatch(cpp_multi_match)
        raise StopIteration


class MultiQuery:
    def __init__(
        self,
        cpp_multi_query: cppMultiQuery,
    ):
        self._cpp_multi_query: cppMultiQuery = cpp_multi_query

    def findone(self, document: str) -> MultiMatch:
        if isinstance(document, Reader):
            cpp_multi_match = self._cpp_multi_query.findone(document._cpp_reader)
        else:
            cpp_multi_match = self._cpp_multi_query.findone(document)
        return MultiMatch(cpp_multi_match)

    def findmany(self, document: Union[str, Reader], limit: int):
        if isinstance(document, Reader):
            cpp_matches = self._cpp_multi_query.findmany(document._cpp_reader, limit)
        else:
            cpp_matches = self._cpp_multi_query.findmany(document, limit)
        return [MultiMatch(cpp_match) for cpp_match in cpp_matches]

    def findall(self, document: Union[str, Reader]) -> list[MultiMatch]:
        if isinstance(document, Reader):
            cpp_matches = self._cpp_multi_query.findall(document._cpp_reader)
        else:
            cpp_matches = self._cpp_multi_query.findall(document)
        return [MultiMatch(cpp_match) for cpp_match in cpp_matches]

    def finditer(self, document: Union[str, Reader]) -> MultiMatchGenerator:
        if isinstance(document, Reader):
            cpp_match_generator = self._cpp_multi_query.finditer(document._cpp_reader)
        else:
            cpp_match_generator = self._cpp_multi_query.finditer(document)
        return MultiMatchGenerator(cpp_match_generator)

    def check(self, document: Union[str, Reader]) -> bool:
        if isinstance(document, Reader):
            return self._cpp_multi_query.check(document._cpp_reader)
        return self._cpp_multi_query.check(document)

    def variables(self) -> list[str]:
        return self._cpp_multi_query.variables()


def multi_reql(
    pattern: str,
    flags: Flags = Flags.NONE,
    max_mempool_duplications=DEFAULT_MAX_MEMPOOL_DUPLICATIONS,
    max_deterministic_states=DEFAULT_MAX_DETERMINISTIC_STATES,
    stream_buffer_size=DEFAULT_STREAM_BUFFER_SIZE,
) -> MultiQuery:
    cpp_multi_query = cppmulti_reql(
        pattern,
        flags._as_cpp_flags(),
        max_mempool_duplications,
        max_deterministic_states,
        stream_buffer_size,
    )
    return MultiQuery(cpp_multi_query)
