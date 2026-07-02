package rematch;
import java.util.Iterator;

public class MultiMatchGeneratorIterator implements Iterator<MultiMatch> {

    private final javacpp.MultiMatchGeneratorIterator current;
    private final javacpp.MultiMatchGeneratorIterator end;

    public MultiMatchGeneratorIterator(javacpp.MultiMatchGeneratorIterator begin, javacpp.MultiMatchGeneratorIterator end) {
        this.current = begin;
        this.end = end;
    }

    @Override
    public boolean hasNext() {
        return !current.operatorEquals(end);
    }

    @Override
    public MultiMatch next() {
        javacpp.MultiMatch cppMatch = current.operator_star();
        current.operator_increment();
        return new MultiMatch(cppMatch);
    }

    public MultiMatch operatorStar() {
        current.operator_star();  
        return new MultiMatch(current.operator_star());
    }

    public boolean operatorEquals(MultiMatchGeneratorIterator other) {
        return current.operatorEquals(other.current);
    }

    public boolean operatorNotEquals(MultiMatchGeneratorIterator other) {
        return current.operatorNotEquals(other.current);
    }
}
