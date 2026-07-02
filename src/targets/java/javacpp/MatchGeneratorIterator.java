package javacpp;

import org.bytedeco.javacpp.*;
import org.bytedeco.javacpp.annotation.*;

@Platform(include = {
        "REmatch/match.hpp", 
        "REmatch/match_generator.hpp"
        },
        link = "REmatch"
)
@Namespace("REmatch::library_interface::MatchGenerator")
@Name("iterator")
public class MatchGeneratorIterator extends Pointer {

    // static { System.loadLibrary("jniREmatch"); }
    static { JNILoader.load("jniREmatch"); }

    public MatchGeneratorIterator() { allocate(); }

    private native void allocate();

    @Name("operator*")
    public native @ByRef Match operator_star();

    @Name("operator->")
    public native @ByPtr Match operator_arrow();

    @Name("operator++")
    public native @ByRef MatchGeneratorIterator operator_increment();

    @Name("operator++")
    public native void operator_increment_postfix();

    // Comparación de igualdad (==)
    @Name("operator==")
    public native boolean operatorEquals(@ByRef MatchGeneratorIterator other);

    // Comparación de desigualdad (!=)
     @Name("operator!=")
    public native boolean operatorNotEquals(@ByRef MatchGeneratorIterator other);

    public static void main(String[] args){
        try {
            MatchGeneratorIterator it = new MatchGeneratorIterator();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}

