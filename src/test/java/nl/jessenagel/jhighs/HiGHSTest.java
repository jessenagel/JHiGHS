package nl.jessenagel.jhighs;

import org.junit.jupiter.api.Test;

import java.util.Random;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import static org.junit.jupiter.api.Assertions.*;
public class HiGHSTest {

    @Test
    void buildModelExample() {
        System.out.println("=== Building Model Programmatically ===");

        HiGHS solver = new HiGHS();
        try {
            // Add variables
            solver.addVar( 0.0, Double.POSITIVE_INFINITY, 1.0);
            solver.addVar( 0.0, Double.POSITIVE_INFINITY, 2.0);

            // Add constraints
            double[] coeffs1 = {1.0, 2.0};
            int[] vars1 = {0, 1};
            solver.addConstraint(coeffs1, vars1, 0.0, 10.0);
            double[] coeffs2 = {3.0, 1.0};
            int[] vars2 = {0, 1};
            solver.addConstraint(coeffs2, vars2, 0.0, 15.0);


            // Set objective function
            double[] objCoeffs = {1.0, 2.0};
            int[] objVars = {0, 1};
            solver.setObjectiveFunction(objCoeffs, objVars, true, 0.0);

            // Solve the model
            HighsStatus status = solver.solve();
            if (status != HighsStatus.kOk) {
                System.err.println("Solver failed: " + status);
                return;
            }

            // Check solution status
            ModelStatus modelStatus = solver.getModelStatus();
            System.out.println("Model status: " + modelStatus);

            if (modelStatus == ModelStatus.kOptimal) {
                Solution solution = solver.getSolution();
                System.out.println("Optimal objective value: " + solution.getObjectiveValue());

                // Print variable values
                double[] values = solution.getVariableValues();
                for (int i = 0; i < Math.min(values.length, 10); i++) {
                    System.out.printf("x%d = %.6f%n", i + 1, values[i]);
                }
                if (values.length > 10) {
                    System.out.println("... (" + values.length + " variables total)");
                }
            }

        } finally {
            solver.dispose();
        }
    }
}
