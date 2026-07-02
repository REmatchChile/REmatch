package rematch;

import java.util.HashMap;
import java.util.Map;

public class Match {
    private final javacpp.Match cppMatch;

    public Match(javacpp.Match cppMatch) {
        this.cppMatch = cppMatch;
    }

    public int start(String variableName) {
        return (int) this.cppMatch.start(variableName);
    }

    public int start(int variableId) {
        return (int) this.cppMatch.start(variableId);
    }

    public int end(String variableName) {
        return (int) this.cppMatch.end(variableName);
    }

    public int end(int variableId) {
        return (int) this.cppMatch.end(variableId);
    }

    public String group(String variableName) {
        return this.cppMatch.group(variableName);
    }

    public String group(int variableId) {
        return this.cppMatch.group(variableId);
    }

    public javacpp.Span span(int variableId) {
        return this.cppMatch.span(variableId);
    }

    public javacpp.Span span(String variableName) {
        return this.cppMatch.span(variableName);
    }

   
    public String[] variables() {
        javacpp.StringVector cppVariables = this.cppMatch.variables();
        String[] variables = new String[(int) cppVariables.size()];

        for (int i = 0; i < cppVariables.size(); i++) {
            variables[i] = cppVariables.at(i);
        }

        return variables;
    }

    public Map<String, javacpp.Span> groupdict() {
        javacpp.StringSpanMap cppGroupDict = this.cppMatch.groupdict();
        javacpp.StringVector vars = this.cppMatch.variables();
        Map<String, javacpp.Span> groupDict = new HashMap<>();
        
        for (long i = 0; i < vars.size(); i++) {
            String key = vars.at(i).toString();
            javacpp.Span span = cppGroupDict.at(key);
            groupDict.put(key, span);
        }
        //  result.groupdict().at("x").first() + ", " + result.groupdict().at("x").second());

        return groupDict;
    }




    public boolean empty() {
        return this.cppMatch.empty();
    }

    public String toString() {
        return this.cppMatch.to_string();
    }

    
}
