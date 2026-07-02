package javacpp;

import org.bytedeco.javacpp.*;
import org.bytedeco.javacpp.annotation.*;

@Platform(include = {"<vector>", "<string>"}, link = {"REmatch", "stdc++"})
@Name("std::vector<std::string>")
public class StringVector extends Pointer {
    
    // static { System.loadLibrary("jniREmatch"); }
    static { JNILoader.load("jniREmatch"); }

    public StringVector() {
        allocate();
    }

    private native void allocate();

    public native @Cast("size_t") long size();
    public native @StdString String at(@Cast("size_t") long i);
    public native void push_back(@StdString String value);

    @Override
    public String toString() {
        StringBuilder sb = new StringBuilder();
        sb.append("[");
        for (long i = 0; i < size(); i++) {
            sb.append("\"").append(at(i)).append("\"");
            if (i < size() - 1) {
                sb.append(", ");
            }
        }
        sb.append("]");
        return sb.toString();
    }
    
    public static void main(String[] args) {

        // // Crear una instancia de StringVector
        StringVector vector = new StringVector();
        vector.push_back("Hello");
        vector.push_back("World");

        // Imprimir los elementos
        System.out.println("Vector size: " + vector.size());
        System.out.println("Element 0: " + vector.at(0));
        System.out.println("Element 1: " + vector.at(1));
        System.out.println("StringVector content: " + vector);

    }
}
