package javacpp;

import org.bytedeco.javacpp.*;
import org.bytedeco.javacpp.annotation.*;

@Platform(include = {"<map>", "<string>", "REmatch/span.hpp"}, link = "REmatch")
@Name("std::map<std::string, Span>")

public class StringSpanMap extends Pointer {
    
    // static { System.loadLibrary("jniREmatch"); }
    static { JNILoader.load("jniREmatch"); }

    public StringSpanMap() { allocate(); }
    private native void allocate();

    public native @ByRef Span at(@StdString String key);

    public native @Cast("size_t") long size();

    public native boolean count(@StdString String key);

    public static void main(String[] args) {
        System.out.println("Testing StringSpanMap methods...");
    }
}


// No se tiene el método put/insert porque no se puede modificar el mapa desde Java y no se necesita para el uso de la librería.