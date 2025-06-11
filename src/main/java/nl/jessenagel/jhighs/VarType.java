package nl.jessenagel.jhighs;

public enum VarType {
    kContinuous(0),
    kInteger(1),
    kSemiContinuous(2),
    kSemiInteger(3),
    kSemiSemiInteger(4),
    kBinary(5);

    private final int value;

    VarType(int value) {
        this.value = value;
    }

    public static VarType fromInt(int value) {
        for (VarType type : values()) {
            if (type.value == value) return type;
        }
        throw new IllegalArgumentException("Unknown variable type: " + value);
    }

    public int getValue() {
        return value;
    }
}
