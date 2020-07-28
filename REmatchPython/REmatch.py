import sys
sys.path.insert(0, "../REmatchEngine/bin/SWIG")
import rematch

class Match:

    def __init__(self, match_object):
        self.match_object = match_object
        self.variables = match_object.variables()
        # self.__span = span
        # self.__start = start
        # self.__end = end

    def start(self, capture):
        if str(capture).isnumeric():
            capture = self.variables[capture - 1]
        return self.match_object.start(capture)

    def end(self, capture):
        if str(capture).isnumeric():
            capture = self.variables[capture - 1]
        return self.match_object.end(capture)

    def span(self, capture):
        if str(capture).isnumeric():
            capture = self.variables[capture - 1]
        return self.match_object.span(capture)

    def group(self, capture):
        if str(capture).isnumeric():
            capture = self.variables[capture - 1]
        return self.match_object.group(capture)

    def groups(self):
        matches = []
        for var in self.variables:
            matches.append(self.match_object.group(var))
        return tuple(matches)
    
    def groupdict(self):
        matches = dict()
        for var in self.variables:
            matches[var] = self.match_object.group(var)
        return matches

class Regex:

    def __init__(self, pattern, **flags): # Flags del estilo: save_anchors=True, early_output=True
        self.pattern = pattern
        self.rgx_opts = self.set_flags(flags) #Probar si se pasan todas las flags o es necesario poner **
        self.RegEx = rematch.RegEx(pattern, self.rgx_opts)

    def set_flags(self, flags):
        rgx_opts = rematch.RegExOptions()
        for key, value in flags.items():
            if key == "save_anchors":
                rgx_opts.set_save_anchors(value)
            elif key == "multi_line":
                rgx_opts.set_multi_line(value)
            elif key == "line_by_line":
                rgx_opts.set_line_by_line(value)
            elif key == "dot_nl":
                rgx_opts.set_dot_nl(value)
            elif key == "early_output":
                rgx_opts.set_early_output(value)
        return rgx_opts

    def find(self, string):
        return Match(self.RegEx.find(string))

    def findall(self, string): # Retornar objetos match 
        matches = list()
        while True:
            match = self.RegEx.findIter(string)
            if match:
                matches.append(Match(match)) 
            else:
                break
        return matches

    def finditer(self, string):
        matches = list()
        while True:
            match = self.RegEx.findIter(string) #yield
            if not match:
                break
            else:
                yield match
        # return match

    def search(self, string):
        return self.find(string)

    def match(self, string):
        self.rgx_opts.set_save_anchors(True)
        self.RegEx = rematch.RegEx(self.pattern, self.rgx_opts)
        match = self.RegEx.find(string)
        if match.group(0) == string[:len(match.gruop(0))]:
            return Match(match)
        return None

    def fullmatch(self, string):
        self.rgx_opts.set_save_anchors(True)
        self.RegEx = rematch.RegEx(self.pattern, self.rgx_opts)
        match = self.RegEx.find(string)
        if match.group(0) == string:
            return Match(match)
        return None

def compile(pattern, **flags):
    return Regex(pattern, **flags)