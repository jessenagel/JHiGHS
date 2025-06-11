package nl.jessenagel.jhighs;

// Status enums
enum HighsStatus {
    kOk(0),
    kWarning(1),
    kError(-1);

    private final int value;

    HighsStatus(int value) {
        this.value = value;
    }

    public static HighsStatus fromInt(int value) {
        for (HighsStatus status : values()) {
            if (status.value == value) return status;
        }
        throw new IllegalArgumentException("Unknown status: " + value);
    }
}


enum ModelStatus {
    kNotset(0),
    kLoadError(1),
    kModelError(2),
    kPresolveError(3),
    kSolveError(4),
    kPostsolveError(5),
    kModelEmpty(6),
    kOptimal(7),
    kInfeasible(8),
    kUnboundedOrInfeasible(9),
    kUnbounded(10),
    kObjectiveBound(11),
    kObjectiveTarget(12),
    kTimeLimit(13),
    kIterationLimit(14),
    kUnknown(15);

    private final int value;

    ModelStatus(int value) {
        this.value = value;
    }

    public static ModelStatus fromInt(int value) {
        for (ModelStatus status : values()) {
            if (status.value == value) return status;
        }
        throw new IllegalArgumentException("Unknown model status: " + value);
    }
}

// Main Java wrapper class for HiGHS solver
public class HiGHS {

    // Load native library (HiGHS C++ compiled as JNI)
    static {
        System.loadLibrary("highs_jni");
    }

    // Instance variables
    private long solverHandle;
    private boolean initialized = false;

    // Constructor
    public HiGHS() {
        this.solverHandle = createSolver();
        this.initialized = true;
    }

    // ===== Model Management =====
    private native int clearModel(long solverPtr);

    private native int clearSolver(long solverPtr);

    private native int writeModel(long solverPtr, String filename);

    private native int writeSolution(long solverPtr, String filename);

    private native long createSolver();

    private native void destroySolver(long solverPtr);

    private native int readModel(long solverPtr, String filename);

    private native int run(long solverPtr);

    private native int getModelStatus(long solverPtr);

    // ===== Variable Manipulation =====
    private native int addVariable(long solverPtr, double lowerBound, double upperBound, double cost);

    private native int addVariables(long solverPtr, int count, double[] lowerBounds,
                                    double[] upperBounds, double[] costs);

    private native int deleteVariable(long solverPtr, int col);

    private native int changeVariableBounds(long solverPtr, int col, double lower, double upper);

    private native int fixVariable(long solverPtr, int col, double value);

    // ===== Constraint Manipulation =====
    private native int deleteConstraint(long solverPtr, int row);

    private native int changeConstraintBounds(long solverPtr, int row, double lower, double upper);

    private native int addConstraint(long solverPtr, double[] coefficients, int[] variableIndices,
                                     double lowerBound, double upperBound);

    // ===== Objective Function =====
    private native int setObjective(long solverPtr, double[] coefficients, int[] variableIndices,
                                    boolean sense, double offset);

    // ===== Solution Management =====
    private native int getBasis(long solverPtr, int[] colStatus, int[] rowStatus);

    private native int getReducedCosts(long solverPtr, double[] reducedCosts);

    private native int getDualSolution(long solverPtr, double[] dualSolution);

    private native int getRowValues(long solverPtr, double[] rowValues);

    private native double getObjectiveValue(long solverPtr);

    private native double[] getSolutionValues(long solverPtr);

    // ===== Parameter Management =====
    private native int getIntegerParameter(long solverPtr, String paramName);

    private native double getDoubleParameter(long solverPtr, String paramName);

    private native String getStringParameter(long solverPtr, String paramName);

    private native int setIntegerParameter(long solverPtr, String paramName, int value);

    private native int setDoubleParameter(long solverPtr, String paramName, double value);

    private native int setStringParameter(long solverPtr, String paramName, String value);


    // Public API methods

    // ===== Model Management ====

    public HighsStatus clearModel() {
        if (!initialized) throw new IllegalStateException("Solver not initialized");
        int status = clearModel(solverHandle);
        return HighsStatus.fromInt(status);
    }

    public HighsStatus clearSolver() {
        if (!initialized) throw new IllegalStateException("Solver not initialized");
        int status = clearSolver(solverHandle);
        return HighsStatus.fromInt(status);
    }

    public HighsStatus writeModel(String filename) {
        if (!initialized) throw new IllegalStateException("Solver not initialized");
        int status = writeModel(solverHandle, filename);
        return HighsStatus.fromInt(status);
    }

    public HighsStatus writeSolution(String filename) {
        if (!initialized) throw new IllegalStateException("Solver not initialized");
        int status = writeSolution(solverHandle, filename);
        return HighsStatus.fromInt(status);
    }

    public HighsStatus readModel(String filename) {
        if (!initialized) throw new IllegalStateException("Solver not initialized");
        int status = readModel(solverHandle, filename);
        return HighsStatus.fromInt(status);
    }

    public HighsStatus solve() {
        if (!initialized) throw new IllegalStateException("Solver not initialized");
        int status = run(solverHandle);
        return HighsStatus.fromInt(status);
    }

    public ModelStatus getModelStatus() {
        if (!initialized) throw new IllegalStateException("Solver not initialized");
        int status = getModelStatus(solverHandle);
        return ModelStatus.fromInt(status);
    }

    // ===== Variable Manipulation ====
    public int addVar(double lowerBound, double upperBound, double objectiveCoeff) {
        if (!initialized) throw new IllegalStateException("Solver not initialized");
        return addVariable(solverHandle, lowerBound, upperBound, objectiveCoeff);
    }

    public HighsStatus addVars(int count, double[] lowerBounds, double[] upperBounds, double[] costs) {
        if (!initialized) throw new IllegalStateException("Solver not initialized");
        if (lowerBounds.length != count || upperBounds.length != count || costs.length != count) {
            throw new IllegalArgumentException("Array lengths must match variable count");
        }
        int status = addVariables(solverHandle, count, lowerBounds, upperBounds, costs);
        return HighsStatus.fromInt(status);
    }

    public HighsStatus deleteVar(int colIndex) {
        if (!initialized) throw new IllegalStateException("Solver not initialized");
        int status = deleteVariable(solverHandle, colIndex);
        return HighsStatus.fromInt(status);
    }

    public HighsStatus changeVarBounds(int colIndex, double lower, double upper) {
        if (!initialized) throw new IllegalStateException("Solver not initialized");
        int status = changeVariableBounds(solverHandle, colIndex, lower, upper);
        return HighsStatus.fromInt(status);
    }

    public HighsStatus fixVar(int colIndex, double value) {
        if (!initialized) throw new IllegalStateException("Solver not initialized");
        int status = fixVariable(solverHandle, colIndex, value);
        return HighsStatus.fromInt(status);
    }

    // ===== Constraint Management ====
    public HighsStatus addConstraint(double[] coefficients, int[] variableIndices,
                                     double lowerBound, double upperBound) {
        if (!initialized) throw new IllegalStateException("Solver not initialized");
        int status = addConstraint(solverHandle, coefficients, variableIndices, lowerBound, upperBound);
        return HighsStatus.fromInt(status);
    }

    public HighsStatus deleteConstraint(int rowIndex) {
        if (!initialized) throw new IllegalStateException("Solver not initialized");
        int status = deleteConstraint(solverHandle, rowIndex);
        return HighsStatus.fromInt(status);
    }

    public HighsStatus changeConstraintBounds(int rowIndex, double lower, double upper) {
        if (!initialized) throw new IllegalStateException("Solver not initialized");
        int status = changeConstraintBounds(solverHandle, rowIndex, lower, upper);
        return HighsStatus.fromInt(status);
    }

    // ===== Objective Function Management ====
    public HighsStatus setObjectiveFunction(double[] coefficients, int[] variableIndices,
                                            boolean minimize, double constant) {
        if (!initialized) throw new IllegalStateException("Solver not initialized");
        if (coefficients.length != variableIndices.length) {
            throw new IllegalArgumentException("Coefficient and index arrays must have the same length");
        }

        int status = setObjective(solverHandle, coefficients, variableIndices, minimize, constant);
        return HighsStatus.fromInt(status);
    }

    // ===== Solution Management ====
    public Solution getSolution() {
        if (!initialized) throw new IllegalStateException("Solver not initialized");
        double[] values = getSolutionValues(solverHandle);
        double objective = getObjectiveValue(solverHandle);
        return new Solution(values, objective);
    }
    public Basis getBasis() {
        if (!initialized) throw new IllegalStateException("Solver not initialized");

        // Get model dimensions
        Solution solution = getSolution();
        int numVars = solution.getNumVariables();
        // We need to get the number of constraints from somewhere
        // For now, use a placeholder size
        int numConstraints = 100; // This should come from the model

        int[] colStatus = new int[numVars];
        int[] rowStatus = new int[numConstraints];

        int status = getBasis(solverHandle, colStatus, rowStatus);
        if (status != HighsStatus.kOk.ordinal()) {
            return null;
        }

        return new Basis(colStatus, rowStatus);
    }

    public double[] getReducedCosts() {
        if (!initialized) throw new IllegalStateException("Solver not initialized");

        // Get dimension
        Solution solution = getSolution();
        int numVars = solution.getNumVariables();

        double[] reducedCosts = new double[numVars];
        getReducedCosts(solverHandle, reducedCosts);
        return reducedCosts;
    }

    public double[] getDualSolution() {
        if (!initialized) throw new IllegalStateException("Solver not initialized");

        // This needs the number of constraints, which we should get from the model
        int numConstraints = 100; // Placeholder

        double[] dualSolution = new double[numConstraints];
        getDualSolution(solverHandle, dualSolution);
        return dualSolution;
    }

    public double[] getRowValues() {
        if (!initialized) throw new IllegalStateException("Solver not initialized");

        // This needs the number of constraints, which we should get from the model
        int numConstraints = 100; // Placeholder

        double[] rowValues = new double[numConstraints];
        getRowValues(solverHandle, rowValues);
        return rowValues;
    }
    // ===== Parameter Management ====

    public int getIntParameter(String paramName) {
        if (!initialized) throw new IllegalStateException("Solver not initialized");
        return getIntegerParameter(solverHandle, paramName);
    }

    public double getDoubleParameter(String paramName) {
        if (!initialized) throw new IllegalStateException("Solver not initialized");
        return getDoubleParameter(solverHandle, paramName);
    }

    public String getStringParameter(String paramName) {
        if (!initialized) throw new IllegalStateException("Solver not initialized");
        return getStringParameter(solverHandle, paramName);
    }

    public HighsStatus setIntParameter(String paramName, int value) {
        if (!initialized) throw new IllegalStateException("Solver not initialized");
        int status = setIntegerParameter(solverHandle, paramName, value);
        return HighsStatus.fromInt(status);
    }

    public HighsStatus setDoubleParameter(String paramName, double value) {
        if (!initialized) throw new IllegalStateException("Solver not initialized");
        int status = setDoubleParameter(solverHandle, paramName, value);
        return HighsStatus.fromInt(status);
    }

    public HighsStatus setStringParameter(String paramName, String value) {
        if (!initialized) throw new IllegalStateException("Solver not initialized");
        int status = setStringParameter(solverHandle, paramName, value);
        return HighsStatus.fromInt(status);
    }
    // Cleanup
    public void dispose() {
        if (initialized) {
            destroySolver(solverHandle);
            initialized = false;
        }
    }

}

// Solution container class
class Solution {
    private final double[] variableValues;
    private final double objectiveValue;

    public Solution(double[] variableValues, double objectiveValue) {
        this.variableValues = variableValues.clone();
        this.objectiveValue = objectiveValue;
    }

    public double[] getVariableValues() {
        return variableValues.clone();
    }

    public double getVariableValue(int index) {
        return variableValues[index];
    }

    public double getObjectiveValue() {
        return objectiveValue;
    }

    public int getNumVariables() {
        return variableValues.length;
    }
}

class Basis {
    private final int[] columnStatus;
    private final int[] rowStatus;

    public Basis(int[] columnStatus, int[] rowStatus) {
        this.columnStatus = columnStatus.clone();
        this.rowStatus = rowStatus.clone();
    }

    public int[] getColumnStatus() {
        return columnStatus.clone();
    }

    public int[] getRowStatus() {
        return rowStatus.clone();
    }

    public int getColumnStatus(int index) {
        return columnStatus[index];
    }

    public int getRowStatus(int index) {
        return rowStatus[index];
    }
}