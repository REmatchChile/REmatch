package rematch;


import java.util.ArrayList;
import java.util.List;

public class Query {

    private final javacpp.Query cppQuery;

    public Query(String pattern, Flags flags, int maxMempoolDuplications, int maxDeterministicStates) {
        this.cppQuery = new javacpp.Query(pattern, flags.getValue(), maxMempoolDuplications, maxDeterministicStates);
    }

    public Query(String pattern, Flags flags) {
        this(pattern, flags, 8, 2000); // valores por defecto
    }

    public boolean check(String document) {
        return cppQuery.check(document);
    }

    public String[] variables() {
        javacpp.StringVector cppVariables = this.cppQuery.variables();
        String[] variables = new String[(int) cppVariables.size()];
        for (int i = 0; i < cppVariables.size(); i++) {
            variables[i] = cppVariables.at(i);
        }
        return variables;
    }


    public Match findOne(String document) {
        javacpp.Match match = cppQuery.findone(document);
        return new Match(match);
    }

    public List<Match> findMany(String document, int limit) {
        javacpp.MatchVector cppMatches = cppQuery.findmany(document, limit);
        List<Match> result = new ArrayList<>();
        for (int i = 0; i < cppMatches.size(); i++) {
            result.add(new Match(cppMatches.at(i)));
        }
        return result;
    }

    public List<Match> findAll(String document) {
        javacpp.MatchVector cppMatches = cppQuery.findall(document);
        List<Match> result = new ArrayList<>();
        for (int i = 0; i < cppMatches.size(); i++) {
            result.add(new Match(cppMatches.at(i)));
        }
        return result;
    }

    public MatchGenerator findIter(String document) {
        javacpp.MatchGenerator generator = cppQuery.finditer(document);
        return generator != null ? new MatchGenerator(generator) : null;
    }
}
