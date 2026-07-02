package rematch;


import java.util.Iterator;

public class MatchGenerator implements Iterable<Match> {
    private final javacpp.MatchGenerator generator;

    public MatchGenerator(javacpp.MatchGenerator generator) {
        this.generator = generator;
    }

    @Override
    public MatchGeneratorIterator iterator() {
        return new MatchGeneratorIterator(generator.begin(), generator.end());
    }
}
