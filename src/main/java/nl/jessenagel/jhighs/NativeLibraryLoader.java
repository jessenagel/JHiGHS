package nl.jessenagel.jhighs;


import java.io.*;
import java.nio.file.*;

public class NativeLibraryLoader {
    private static boolean librariesLoaded = false;

    public static synchronized void loadNativeLibraries() {
        if (librariesLoaded) {
            return;
        }
        try{
            System.loadLibrary("jhighs");
        }catch(UnsatisfiedLinkError error) {
            try {
                String platform = getPlatform();
                String[] libraryNames = getLibraryNames(platform);

                // Create temporary directory for extracted libraries
                Path tempDir = Files.createTempDirectory("jhighs_natives");
                tempDir.toFile();

                // Extract and load libraries in correct order
                for (String libName : libraryNames) {
                    String resourcePath = "/natives/" + platform + "/" + libName;
                    extractAndLoadLibrary(resourcePath, tempDir, libName);
                }

                librariesLoaded = true;

            } catch (Exception e) {
                throw new RuntimeException("Failed to load native libraries", e);
            }
        }
    }

    private static String getPlatform() {
        String os = System.getProperty("os.name").toLowerCase();
        String arch = System.getProperty("os.arch").toLowerCase();

        if (os.contains("win")) {
            return "windows-x86_64";
        } else if (os.contains("mac") || os.contains("darwin")) {
            return "darwin-x86_64";
        } else if (os.contains("linux")) {
            return "linux-x86_64";
        } else {
            throw new RuntimeException("Unsupported platform: " + os + " " + arch);
        }
    }

    private static String[] getLibraryNames(String platform) {
        switch (platform) {
            case "linux-x86_64":
                return new String[]{"libhighs.so", "libhighs_jni.so"};
            case "darwin-x86_64":
                return new String[]{"libhighs.dylib", "libjhighs.dylib"};
            case "windows-x86_64":
                return new String[]{"highs.dll", "jhighs.dll"};
            default:
                throw new RuntimeException("Unsupported platform: " + platform);
        }
    }

    private static void extractAndLoadLibrary(String resourcePath, Path tempDir, String libName) throws IOException {
        try (InputStream is = NativeLibraryLoader.class.getResourceAsStream(resourcePath)) {
            if (is == null) {
                throw new RuntimeException("Native library not found: " + resourcePath);
            }

            Path libFile = tempDir.resolve(libName);
            Files.copy(is, libFile, StandardCopyOption.REPLACE_EXISTING);

            // Make executable on Unix systems
            if (!System.getProperty("os.name").toLowerCase().contains("win")) {
                libFile.toFile().setExecutable(true);
            }

            System.out.println("Extracting " + libName + " to " + libFile);

            try {
                LddRunner.runLdd(libFile.toAbsolutePath().toString());
            } catch (InterruptedException e) {
                // Just log this, don't fail
                System.err.println("LDD check interrupted: " + e.getMessage());
            }

            // If this is the first library (libhighs.so), set LD_LIBRARY_PATH to include temp directory
            if (libName.equals("libhighs.so")) {
                System.setProperty("java.library.path", tempDir.toString() +
                        ":" + System.getProperty("java.library.path"));
            }

            // Load the library using absolute path
            System.load(libFile.toAbsolutePath().toString());
        }
    }

    public static class LddRunner {
        public static void runLdd(String libPath) throws IOException, InterruptedException {
            ProcessBuilder pb = new ProcessBuilder("ldd", libPath);
            pb.redirectErrorStream(true);
            Process process = pb.start();

            try (BufferedReader reader = new BufferedReader(
                    new InputStreamReader(process.getInputStream()))) {
                String line;
                while ((line = reader.readLine()) != null) {
                    System.out.println(line);
                }
            }
            int exitCode = process.waitFor();
            System.out.println("ldd exited with code: " + exitCode);
        }
    }
}
