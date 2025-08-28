package javacpp;

import org.bytedeco.javacpp.*;
import org.bytedeco.javacpp.annotation.*;

@Platform(include = "REmatch/flags.hpp", link = "REmatch")
@Namespace("REmatch::library_interface") 
@Name("Flags")



public class Flags extends Pointer {

    // static { System.loadLibrary("jniREmatch"); }
    static { JNILoader.load("jniREmatch"); }

    // Constantes equivalentes a los valores del enum C++
    public static final int NONE = 0;
    public static final int LINE_BY_LINE = 1;

    private int value;

    // Constructor predeterminado que asigna el valor de NONE
    public Flags() {
        this.value = NONE;
        allocate();
    }

    private native void allocate();
        
    // Constructor que toma un valor entero
    public Flags(int value) {
        this.value = value;
    }

    public int getVal() {
        return this.value;
    }

    // Métodos para crear los valores predefinidos del enum
    public static Flags NONE() {
        return new Flags(NONE);
    }

    public static Flags LINE_BY_LINE() {
        return new Flags(LINE_BY_LINE);
    }

    public static void main(String[] args) {
        Flags flags = new Flags();
        
        // Usa las constantes
        System.out.println("NONE: " + Flags.NONE);
        System.out.println("LINE_BY_LINE: " + Flags.LINE_BY_LINE);
        System.out.println("Flags: " + flags.getVal());
    }
    
}


