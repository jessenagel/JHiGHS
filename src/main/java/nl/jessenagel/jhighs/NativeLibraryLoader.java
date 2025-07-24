package nl.jessenagel.jhighs;


import java.io.*;
import java.nio.file.*;

public class NativeLibraryLoader {
    private static boolean librariesLoaded = false;
    private static Path tempDir = null;

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
                tempDir = Files.createTempDirectory("jhighs_natives");

                // Register shutdown hook to clean up
                Runtime.getRuntime().addShutdownHook(new Thread(NativeLibraryLoader::cleanupTempDirectory));

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
            if (arch.contains("amd64") || arch.contains("x86_64")) {
                return "windows-x86_64";
            } else {
                throw new RuntimeException("Unsupported architecture on Windows: " + arch);
            }
        } else if (os.contains("mac") || os.contains("darwin")) {
            if (arch.contains("amd64") || arch.contains("x86_64")) {
                return "darwin-x86_64";
            } else if (arch.contains("arm64") || arch.contains("aarch64")) {
                return "darwin-arm64";
            } else {
                throw new RuntimeException("Unsupported architecture on macOS: " + arch);
            }
        } else if (os.contains("linux")) {
            if (arch.contains("amd64") || arch.contains("x86_64")) {
                return "linux-x86_64";
            } else if (arch.contains("arm64") || arch.contains("aarch64")) {
                return "linux-arm64";
            } else {
                throw new RuntimeException("Unsupported architecture on Linux: " + arch);
            }
        } else {
            throw new RuntimeException("Unsupported platform: " + os + " " + arch);
        }
    }

    private static String[] getLibraryNames(String platform) {
        return switch (platform) {
            case "linux-x86_64", "linux-arm64" -> new String[]{"libhighs.so", "libjhighs.so"};
            case "darwin-arm64", "darwin-x86_64" -> new String[]{"libhighs.dylib", "libjhighs.dylib"};
            case "windows-x86_64" -> new String[]{"highs.dll", "jhighs.dll"};
            default -> throw new RuntimeException("Unsupported platform: " + platform);
        };
    }

    private static void cleanupTempDirectory() {
        if (tempDir != null) {
            try {
                Files.walk(tempDir)
                        .sorted((p1, p2) -> -p1.compareTo(p2)) // Reverse order to delete files before directories
                        .forEach(path -> {
                            try {
                                Files.delete(path);
                            } catch (IOException e) {
                                System.err.println("Failed to delete: " + path + ": " + e.getMessage());
                            }
                        });
                System.out.println("Cleaned up temporary directory: " + tempDir);
            } catch (IOException e) {
                System.err.println("Failed to clean up temporary directory: " + e.getMessage());
            }
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


            // If this is the first library (libhighs.so), set LD_LIBRARY_PATH to include temp directory
            if (libName.equals("libhighs.so")) {
                System.setProperty("java.library.path", tempDir +
                        ":" + System.getProperty("java.library.path"));
            }

            // Load the library using absolute path
            System.load(libFile.toAbsolutePath().toString());
        }
    }

}
