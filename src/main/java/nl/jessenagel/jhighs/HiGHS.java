package nl.jessenagel.jhighs;



// Main Java wrapper class for HiGHS solver
public class HiGHS {

    // Load native library (HiGHS C++ compiled as JNI)
    static {
        try {
            NativeLibraryLoader.loadNativeLibraries();
        } catch (Exception e) {
            throw new RuntimeException("Failed to initialize HiGHS native libraries", e);
        }
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
    private native int addVariable(long solverPtr, double lowerBound, double upperBound);

    private native int addVariables(long solverPtr, int count, double[] lowerBounds,
                                    double[] upperBounds);

    private native int deleteVariable(long solverPtr, int col);

    private native int changeVariableBounds(long solverPtr, int col, double lower, double upper);

    private native int changeColIntegrality(long solverPtr, int col, int type);

    // ===== Constraint Manipulation =====
    private native int deleteConstraint(long solverPtr, int row);

    private native int changeConstraintBounds(long solverPtr, int row, double lower, double upper);

    private native int addConstraint(long solverPtr, double[] coefficients, int[] variableIndices,
                                     double lowerBound, double upperBound);

    // ===== Objective Function =====
    private native int setObjective(long solverPtr, double[] coefficients, int[] variableIndices,
                                    boolean sense, double offset);

    // ===== Solution Management =====

//    private native int getReducedCosts(long solverPtr, double[] reducedCosts);
//
//    private native int getDualSolution(long solverPtr, double[] dualSolution);
//
//    private native int getRowValues(long solverPtr, double[] rowValues);

    private native double getObjectiveValue(long solverPtr);

    private native double[] getSolutionValues(long solverPtr);

//    // ===== Parameter Management =====
//    private native int getIntegerParameter(long solverPtr, String paramName);
//
//    private native double getDoubleParameter(long solverPtr, String paramName);
//
//    private native String getStringParameter(long solverPtr, String paramName);
//
//    private native int setIntegerParameter(long solverPtr, String paramName, int value);
//
//    private native int setDoubleParameter(long solverPtr, String paramName, double value);
//
//    private native int setStringParameter(long solverPtr, String paramName, String value);


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
    public int addVar(double lowerBound, double upperBound) {
        if (!initialized) throw new IllegalStateException("Solver not initialized");
        return addVariable(solverHandle, lowerBound, upperBound);
    }

    public HighsStatus addVars(int count, double[] lowerBounds, double[] upperBounds, double[] costs) {
        if (!initialized) throw new IllegalStateException("Solver not initialized");
        if (lowerBounds.length != count || upperBounds.length != count || costs.length != count) {
            throw new IllegalArgumentException("Array lengths must match variable count");
        }
        int status = addVariables(solverHandle, count, lowerBounds, upperBounds);
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

    public HighsStatus changeColIntegrality(int colIndex, VarType varType) {
        if (!initialized) throw new IllegalStateException("Solver not initialized");
        if (varType == null) {
            throw new IllegalArgumentException("Variable type cannot be null");
        }
        int status = changeColIntegrality(solverHandle, colIndex, varType.getValue());
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

    //    public double[] getReducedCosts() {
//        if (!initialized) throw new IllegalStateException("Solver not initialized");
//
//        // Get dimension
//        Solution solution = getSolution();
//        int numVars = solution.getNumVariables();
//
//        double[] reducedCosts = new double[numVars];
//        getReducedCosts(solverHandle, reducedCosts);
//        return reducedCosts;
//    }
//
//    public double[] getDualSolution() {
//        if (!initialized) throw new IllegalStateException("Solver not initialized");
//
//        // This needs the number of constraints, which we should get from the model
//        int numConstraints = 100; // Placeholder
//
//        double[] dualSolution = new double[numConstraints];
//        getDualSolution(solverHandle, dualSolution);
//        return dualSolution;
//    }
//
//    public double[] getRowValues() {
//        if (!initialized) throw new IllegalStateException("Solver not initialized");
//
//        // This needs the number of constraints, which we should get from the model
//        int numConstraints = 100; // Placeholder
//
//        double[] rowValues = new double[numConstraints];
//        getRowValues(solverHandle, rowValues);
//        return rowValues;
//    }
//    // ===== Parameter Management ====
//
//    public int getIntParameter(String paramName) {
//        if (!initialized) throw new IllegalStateException("Solver not initialized");
//        return getIntegerParameter(solverHandle, paramName);
//    }
//
//    public double getDoubleParameter(String paramName) {
//        if (!initialized) throw new IllegalStateException("Solver not initialized");
//        return getDoubleParameter(solverHandle, paramName);
//    }
//
//    public String getStringParameter(String paramName) {
//        if (!initialized) throw new IllegalStateException("Solver not initialized");
//        return getStringParameter(solverHandle, paramName);
//    }
//
//    public HighsStatus setIntParameter(String paramName, int value) {
//        if (!initialized) throw new IllegalStateException("Solver not initialized");
//        int status = setIntegerParameter(solverHandle, paramName, value);
//        return HighsStatus.fromInt(status);
//    }
//
//    public HighsStatus setDoubleParameter(String paramName, double value) {
//        if (!initialized) throw new IllegalStateException("Solver not initialized");
//        int status = setDoubleParameter(solverHandle, paramName, value);
//        return HighsStatus.fromInt(status);
//    }
//
//    public HighsStatus setStringParameter(String paramName, String value) {
//        if (!initialized) throw new IllegalStateException("Solver not initialized");
//        int status = setStringParameter(solverHandle, paramName, value);
//        return HighsStatus.fromInt(status);
//    }
    // Cleanup
    public void dispose() {
        if (initialized) {
            destroySolver(solverHandle);
            initialized = false;
        }
    }

}

