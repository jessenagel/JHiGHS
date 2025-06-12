package nl.jessenagel.jhighs;

// Status enums
public enum HighsStatus {
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
