package nl.jessenagel.jhighs;

public enum ModelStatus {
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
