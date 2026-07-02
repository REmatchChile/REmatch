package javacpp;

import org.bytedeco.javacpp.*;
import org.bytedeco.javacpp.annotation.*;


@Platform(include = {"<vector>", "REmatch/multi_match.hpp"}, link = {"REmatch", "stdc++"})
@Namespace("std") 
@Name("vector<REmatch::library_interface::MultiMatch>") 
public class MultiMatchVector extends Pointer {

    // static { System.loadLibrary("jniREmatch"); }
    static { JNILoader.load("jniREmatch"); }

    public MultiMatchVector() { 
        allocate(); 
    }

    private native void allocate();

    public native @Cast("size_t") long size();

    public native @ByVal MultiMatch at(@Cast("size_t") long index);

    public native void push_back(@ByVal MultiMatch value);


    public static void main(String[] args) {

        System.out.println("Testing MultiMatchVector...");

        MultiMatchVector vector = new MultiMatchVector();
        System.out.println("Vector size: " + vector.size());

    }
}
