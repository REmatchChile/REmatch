package javacpp;

import org.bytedeco.javacpp.*;
import org.bytedeco.javacpp.annotation.*;

@Platform(include = {
    "REmatch/multi_match.hpp",
    "REmatch/span.hpp"
}, link = "REmatch")
@Namespace("REmatch::library_interface")
@Name("MultiMatch")
public class MultiMatch extends Pointer {

    // static { System.loadLibrary("jniREmatch"); }
    static { JNILoader.load("jniREmatch"); }

    public MultiMatch(Pointer p) {
        super(p);
    }

    public native @ByVal SpanVector spans(@Cast("uint_fast32_t") int variable_id);
    public native @ByVal SpanVector spans(@StdString String variable_name);

    public native @ByVal StringVector groups(@Cast("uint_fast32_t") int variable_id);
    public native @ByVal StringVector groups(@StdString String variable_name);

    public native @ByVal MultiMatch submatch(@ByVal Span span);

    public native @Cast("bool") boolean empty();

    public native @ByVal StringVector variables();

    @Name("operator==")
    public native @Cast("bool") boolean operatorEquals(@ByRef MultiMatch other);

    public native @StdString String to_string();

    // Método para pruebas
    public static void main(String[] args) {
        try {
            System.out.println("Testing MultiMatch methods...");
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
