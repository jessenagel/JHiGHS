package nl.jessenagel.jhighs;


import java.io.*;
import java.nio.file.*;

public class NativeLibraryLoader {
    private static boolean librariesLoaded = false;

    public static synchronized void loadNativeLibraries() {
        if (librariesLoaded) {
            return;
        }
        System.loadLibrary("jhighs");
    }
}