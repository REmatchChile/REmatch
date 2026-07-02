package javacpp;

import org.bytedeco.javacpp.*;
import org.bytedeco.javacpp.annotation.*;

@Platform(include = {
        "REmatch/multi_match.hpp", 
        "REmatch/multi_query.hpp", 
        "REmatch/constants.hpp", 
        "REmatch/multi_match_generator.hpp"
    },
    link = {"REmatch", "stdc++"}
)
@Namespace("REmatch::library_interface")
@Name("MultiMatchGenerator")
public class MultiMatchGenerator extends Pointer {
    
    // static { System.loadLibrary("jniREmatch"); }
    static { JNILoader.load("jniREmatch"); }

    public MultiMatchGenerator(Pointer p) { 
        super(p); 
    }

    // Método para obtener el iterador al inicio
    public native @ByVal MultiMatchGeneratorIterator begin();

    // Método para obtener el iterador al final
    public native @ByVal MultiMatchGeneratorIterator end();

    public static void main(String[] args) {
        System.out.println("Testing MultiMatchGenerator...");
    }
}
