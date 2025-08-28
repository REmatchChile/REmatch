package javacpp;

import org.bytedeco.javacpp.*;
import org.bytedeco.javacpp.annotation.*;

@Platform(include = {
        "REmatch/match.hpp",
        "REmatch/query.hpp",
        "REmatch/constants.hpp",
        "REmatch/match_generator.hpp"
    },
    link = {"REmatch", "stdc++"}
)
@Namespace("REmatch::library_interface")
@Name("MatchGenerator")
public class MatchGenerator extends Pointer {
    
    // static { System.loadLibrary("jniREmatch"); }
    static { JNILoader.load("jniREmatch"); }

    public MatchGenerator(Pointer p) { 
        super(p); 
    }

    // Método para obtener el iterador al inicio
    public native @ByVal MatchGeneratorIterator begin();

    // Método para obtener el iterador al final
    public native @ByVal MatchGeneratorIterator end();

    public static void main(String[] args){
        System.out.println("Hello, World!");
    
    }
}
