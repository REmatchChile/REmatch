package javacpp;

import org.bytedeco.javacpp.*;
import org.bytedeco.javacpp.annotation.*;


@Platform(include = {"REmatch/match.hpp", "REmatch/span.hpp"}, link="REmatch")
@Namespace("REmatch::library_interface")
@Name("Match")


public class Match extends Pointer {

    // static { System.loadLibrary("jniREmatch"); }
    static { JNILoader.load("jniREmatch"); }

    public Match(Pointer p) { super(p); }


    public native @Cast("int64_t") long start(@StdString String variable_name);
    public native @Cast("int64_t") long start(@Cast("uint_fast32_t") int variable_id);

    public native @Cast("int64_t") long end(@StdString String variable_name);
    public native @Cast("int64_t") long end(@Cast("uint_fast32_t") int variable_id);

    public native @StdString String group(@StdString String variable_name);
    public native @StdString String group(@Cast("uint_fast32_t") int variable_id);

    public native @ByVal Span span(@StdString String variable_name);
    public native @ByVal Span span(@Cast("uint_fast32_t") int variable_id);

    public native @ByVal StringSpanMap groupdict();

    public native @ByVal StringVector variables();

    public native @Cast("bool") boolean empty();

    public native @StdString String to_string();


    public static void main(String[] args) {
        try {

            // Prueba los métodos básicos
            System.out.println("Testing Match methods...");


        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}