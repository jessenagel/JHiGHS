package nl.jessenagel.jhighs;

public class Solution {
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
