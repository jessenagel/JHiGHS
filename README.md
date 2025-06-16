# JHiGHS
JHiGHS is a Java Native Interface (JNI) wrapper for HiGHS, a high-performance linear optimization solver. This library allows Java applications to use the powerful HiGHS solver for linear programming and mixed-integer programming problems.

# Disclaimer
This project is not affiliated with the HiGHS project. It is an independent JNI wrapper that provides a Java interface to the HiGHS solver.
This project is a minimal product and does not include all features of HiGHS. 
It currently supports basic linear programming and mixed-integer programming functionalities.
It does not include advanced features such as presolve, scaling, or advanced output options.
# Features
- Simple and efficient JNI wrapper for HiGHS
- Supports basic linear programming (LP) and mixed-integer programming (MIP)
- Easy to integrate into Java applications
- Cross-platform support (Linux, Windows, macOS)
# Installation
On (x86_64) Linux, you can simply download the latest .jar file from releases and add it as a dependency in your Java project. For other platforms, you will need to build the library from source.

# Prerequisites 
- Java Development Kit (JDK) 
- CMake 
- C++ compiler (e.g., GCC, Clang, or MSVC)
- HiGHS library 
- Maven (for building the project)
# Building from Source
1. Clone the repository:
   ```bash
   git clone
   ```
2. Navigate to the project directory:
   ```bash
   cd JHiGHS
   ```
3. Run the build script:
   ```bash
   chmod +x ./scripts/build-native.sh
   ./scripts/build-native.sh
   ```
5. Package the project using Maven:
   ```bash
   mvn clean package
   ```
6. The built JAR file will be located in the `target` directory.

# Usage
To use JHiGHS in your Java application, include the JAR file in your classpath.
Example usage:
```java
import nl.jessenagel.jhighs.HiGHS;
import nl.jessenagel.jhighs.ModelStatus;
import nl.jessenagel.jhighs.Solution;
import nl.jessenagel.jhighs.VarType;

import java.io.File;
import java.nio.file.Files;
import java.nio.file.Path;

public class Main {
    public static void main(String[] args) {
        try {


            HiGHS solver = new HiGHS();
            try {
                //minimize    f  =  x0 +  x1
                //subject to              x1 <= 7
                //            5 <=  x0 + 2x1 <= 15
                //            6 <= 3x0 + 2x1
                //            0 <= x0 <= 4; 1 <= x1
                // X0, x1 are integer variables
                // Add variables
                solver.addVar(0.0, 4.0);
                solver.addVar(1.0, Double.POSITIVE_INFINITY);

                // Add constraints
                double[] coeffs1 = {1.0};
                int[] vars1 = {1};
                solver.addConstraint(coeffs1, vars1, Double.NEGATIVE_INFINITY, 7);
                double[] coeffs2 = {1.0, 2.0};
                int[] vars2 = {0, 1};
                solver.addConstraint(coeffs2, vars2, 5.0, 15.0);
                double[] coeffs3 = {3.0, 2.0};
                int[] vars3 = {0, 1};
                solver.addConstraint(coeffs3, vars3, 6.0, Double.POSITIVE_INFINITY);

                // Set objective function
                double[] objCoeffs = {1.0, 1.0};
                int[] objVars = {0, 1};
                solver.setObjectiveFunction(objCoeffs, objVars, true, 0.0);

                // Set integer variables
                System.out.println(solver.changeColIntegrality(0, VarType.kInteger));
                solver.changeColIntegrality(1, VarType.kInteger);
                // Solve the model
                solver.solve();

                ModelStatus modelStatus = solver.getModelStatus();
                // Check solution status
                if (modelStatus == ModelStatus.kOptimal) {
                    Solution solution = solver.getSolution();
                    // Print variable values
                    double[] values = solution.getVariableValues();
                    double[] expectedValues = {0.0, 3.0};
                    for (int i = 0; i < Math.min(values.length, 10); i++) {
                        System.out.println(values[i]);
                    }

                }

            } finally {
                solver.dispose();
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

}
```
