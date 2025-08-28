package javacpp;

import org.bytedeco.javacpp.*;
import org.bytedeco.javacpp.annotation.*;

@Platform(include = {"REmatch/span.hpp"}, link = "REmatch")
@Name("std::pair<int64_t, int64_t>")

@Opaque
public class Span extends Pointer {
    
    // static { System.loadLibrary("jniREmatch"); }
    static { JNILoader.load("jniREmatch"); }

    // Constructor vacío
    public Span() { allocate(); }
    private native void allocate();

    // Constructor con valores
    public Span(@Cast("int64_t") long first, @Cast("int64_t") long second) {
        allocate(first, second);
    }
    private native void allocate(@Cast("int64_t") long first, @Cast("int64_t") long second);

    // Métodos de acceso para first y second
    @MemberGetter public native @Cast("int64_t") long first();
    @MemberSetter public native void first(@Cast("int64_t") long value);

    @MemberGetter public native @Cast("int64_t") long second();
    @MemberSetter public native void second(@Cast("int64_t") long value);

    public static void main(String[] args) {
        Span span = new Span(1, 2);
        System.out.println("First: " + span.first());
        System.out.println("Second: " + span.second());
    }
}
