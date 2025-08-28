package rematch;
import java.util.Iterator;

public class MatchGeneratorIterator implements Iterator<Match> {
    
    private final javacpp.MatchGeneratorIterator current;
    private final javacpp.MatchGeneratorIterator end;

    public MatchGeneratorIterator(javacpp.MatchGeneratorIterator begin, javacpp.MatchGeneratorIterator end) {
        this.current = begin;
        this.end = end;
    }

    @Override
    public boolean hasNext() {
        return !current.operatorEquals(end);  // operator==
    }

    @Override
    public Match next() {
        javacpp.Match cppMatch = current.operator_star();  // equivalente a *it en C++
        current.operator_increment();              // it++
        return new Match(cppMatch);
    }

    public Match operatorStar() {
        current.operator_star();         // equivalente a *it en C++
        return new Match(current.operator_star());  // devuelve el valor actual
    }

    public boolean operatorEquals(MatchGeneratorIterator other) {
        return current.operatorEquals(other.current);  // operador==
    }

    public boolean operatorNotEquals(MatchGeneratorIterator other) {
        return current.operatorNotEquals(other.current);  // operador!=
    }
}
