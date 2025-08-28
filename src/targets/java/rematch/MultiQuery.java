package rematch;

import java.util.ArrayList;
import java.util.List;

public class MultiQuery {

    private final javacpp.MultiQuery cppMultiQuery;

    public MultiQuery(String pattern, Flags flags, int maxMempoolDuplications, int maxDeterministicStates) {
        this.cppMultiQuery = new javacpp.MultiQuery(pattern, flags.getValue(), maxMempoolDuplications, maxDeterministicStates);
    }

    public MultiQuery(String pattern, Flags flags) {
        this(pattern, flags, 8, 2000);
    }

    public boolean check(String document) {
        return cppMultiQuery.check(document);
    }

    public String[] variables() {
        javacpp.StringVector cppVariables = cppMultiQuery.variables();
        String[] variables = new String[(int) cppVariables.size()];
        for (int i = 0; i < cppVariables.size(); i++) {
            variables[i] = cppVariables.at(i).toString();
        }
        return variables;
    }

    public MultiMatch findOne(String document) {
        return new MultiMatch(cppMultiQuery.findone(document));
    }

    public List<MultiMatch> findMany(String document, int limit) {
        javacpp.MultiMatchVector matches = cppMultiQuery.findmany(document, limit);
        List<MultiMatch> results = new ArrayList<>();
        for (int i = 0; i < matches.size(); i++) {
            results.add(new MultiMatch(matches.at(i)));
        }
        return results;
    }

    public List<MultiMatch> findAll(String document) {
        javacpp.MultiMatchVector matches = cppMultiQuery.findall(document);
        List<MultiMatch> results = new ArrayList<>();
        for (int i = 0; i < matches.size(); i++) {
            results.add(new MultiMatch(matches.at(i)));
        }
        return results;
    }

    public MultiMatchGenerator findIter(String document) {
        javacpp.MultiMatchGenerator generator = cppMultiQuery.finditer(document);
        return generator != null ? new MultiMatchGenerator(generator) : null;
    }
}
