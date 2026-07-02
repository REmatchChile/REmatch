package javacpp;

import java.io.IOException;

public class JNILoader {

    public static void load(String libName) {
        try {
            System.loadLibrary(libName);
        } catch (UnsatisfiedLinkError e) {
            try {
                String pathInJar = "/lib" + libName + ".so";
                NativeLoader.loadLibraryFromJar(pathInJar);
            } catch (IOException io) {
                throw new RuntimeException("No se pudo cargar " + libName, io);
            }
        }
    }
}
