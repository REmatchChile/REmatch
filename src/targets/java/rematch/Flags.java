package rematch;


public class Flags {
    private final javacpp.Flags cppFlags;

    public enum Mode {
        NONE,
        LINE_BY_LINE
    }

    public Flags(Mode mode) {
        switch (mode) {
            case LINE_BY_LINE:
                this.cppFlags = javacpp.Flags.LINE_BY_LINE();
                break;
            case NONE:
            default:
                this.cppFlags = javacpp.Flags.NONE();
                break;
        }
    }

    public int getValue() {
        return this.cppFlags.getVal();
    }

    public static Flags none() {
        return new Flags(Mode.NONE);
    }

    public static Flags lineByLine() {
        return new Flags(Mode.LINE_BY_LINE);
    }
}
