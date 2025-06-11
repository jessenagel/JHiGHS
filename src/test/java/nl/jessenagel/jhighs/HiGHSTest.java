package nl.jessenagel.jhighs;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.*;
public class HiGHSTest {

    @Test
    void solveLP() {
        HiGHS solver = new HiGHS();
        try {
            //minimize    f  =  x0 +  x1
            //subject to              x1 <= 7
            //            5 <=  x0 + 2x1 <= 15
            //            6 <= 3x0 + 2x1
            //            0 <= x0 <= 4; 1 <= x1
            // Add variables
            solver.addVar( 0.0, 4.0);
            solver.addVar( 1.0, Double.POSITIVE_INFINITY);

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

            // Solve the model
            HighsStatus status = solver.solve();
            if (status != HighsStatus.kOk) {
                System.err.println("Solver failed: " + status);
                return;
            }
            // Check solution status
            ModelStatus modelStatus = solver.getModelStatus();
            assertEquals(ModelStatus.kOptimal, modelStatus);
            if (modelStatus == ModelStatus.kOptimal) {
                Solution solution = solver.getSolution();
                // Print variable values
                double[] values = solution.getVariableValues();
                double[] expectedValues = {0.5, 2.25};
                for (int i = 0; i < Math.min(values.length, 10); i++) {
                    assertEquals(expectedValues[i], values[i], 1e-6, "Variable value mismatch at index " + i);
                }

            }

        } finally {
            solver.dispose();
        }
    }

    @Test
    void solveIP() {
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
            System.out.println(solver.changeColIntegrality(0,   VarType.kInteger));
            solver.changeColIntegrality(1,   VarType.kInteger);
            // Solve the model
            HighsStatus status = solver.solve();
            if (status != HighsStatus.kOk) {
                System.err.println("Solver failed: " + status);
                return;
            }

            // Check solution status
            ModelStatus modelStatus = solver.getModelStatus();
            assertEquals(ModelStatus.kOptimal, modelStatus);
            if (modelStatus == ModelStatus.kOptimal) {
                Solution solution = solver.getSolution();
                // Print variable values
                double[] values = solution.getVariableValues();
                double[] expectedValues = {0.0, 3.0};
                for (int i = 0; i < Math.min(values.length, 10); i++) {
                    assertEquals(expectedValues[i], values[i], 1e-6, "Variable value mismatch at index " + i);
                }

            }

        } finally {
            solver.dispose();
        }
    }
}
