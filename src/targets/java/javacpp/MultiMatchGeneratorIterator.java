package javacpp;

import org.bytedeco.javacpp.*;
import org.bytedeco.javacpp.annotation.*;

@Platform(include = {
        "REmatch/multi_match.hpp",
        "REmatch/multi_match_generator.hpp"
        }, 
        link = "REmatch"
)
@Namespace("REmatch::library_interface::MultiMatchGenerator")
@Name("iterator")
public class MultiMatchGeneratorIterator extends Pointer {

    // static { System.loadLibrary("jniREmatch"); }
    static { JNILoader.load("jniREmatch"); }

    public MultiMatchGeneratorIterator() { allocate(); }

    private native void allocate();

    @Name("operator*")
    public native @ByRef MultiMatch operator_star();

    @Name("operator->")
    public native @ByPtr MultiMatch operator_arrow();

    @Name("operator++")
    public native @ByRef MultiMatchGeneratorIterator operator_increment();

    @Name("operator++")
    public native void operator_increment_postfix();

    // Comparar iteradores (it == other)
    @Name("operator==")
    public native boolean operatorEquals(@ByRef MultiMatchGeneratorIterator other);

    // Comparar iteradores (it != other)
    @Name("operator!=")
    public native boolean operatorNotEquals(@ByRef MultiMatchGeneratorIterator other);

    public static void main(String[] args){
        try {
            MultiMatchGeneratorIterator it = new MultiMatchGeneratorIterator();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
