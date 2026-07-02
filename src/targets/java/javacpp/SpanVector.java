package javacpp;

import org.bytedeco.javacpp.*;
import org.bytedeco.javacpp.annotation.*;


@Platform(include = {"<vector>", "REmatch/span.hpp"}, link = {"REmatch", "stdc++"})
@Namespace("std")
@Name("vector<std::pair<int64_t, int64_t>>")
public class SpanVector extends Pointer {

    // static { System.loadLibrary("jniREmatch"); }
    static { JNILoader.load("jniREmatch"); }

    public SpanVector() { 
        allocate(); 
    }

    private native void allocate();

    public native @Cast("size_t") long size();

    public native @ByVal Span at(@Cast("size_t") long index);

    public static void main(String[] args) {
        
        SpanVector vector = new SpanVector();
        System.out.println("Vector size: " + vector.size());
    }
}
