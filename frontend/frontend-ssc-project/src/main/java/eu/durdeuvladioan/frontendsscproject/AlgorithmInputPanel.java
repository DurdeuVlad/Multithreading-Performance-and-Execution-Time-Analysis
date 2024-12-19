package eu.durdeuvladioan.frontendsscproject;

import javafx.scene.chart.LineChart;
import javafx.scene.chart.NumberAxis;
import javafx.scene.chart.XYChart;
import javafx.scene.control.TextArea;
import javafx.scene.control.TextField;
import javafx.scene.layout.HBox;

public class AlgorithmInputPanel {
    private TextField algorithmField;
    private TextField fireStartField;
    private TextField fireEndField;
    private TextField dataSizeStartField;
    private TextField dataSizeEndField;

    public AlgorithmInputPanel() {
        algorithmField = new TextField();
        algorithmField.setPromptText("Algorithm (e.g., quick_sort)");

        fireStartField = new TextField();
        fireStartField.setPromptText("Fire Start");

        fireEndField = new TextField();
        fireEndField.setPromptText("Fire End");

        dataSizeStartField = new TextField();
        dataSizeStartField.setPromptText("Data Size Start");

        dataSizeEndField = new TextField();
        dataSizeEndField.setPromptText("Data Size End");
    }

    public HBox getInputFields() {
        return new HBox(10, algorithmField, fireStartField, fireEndField, dataSizeStartField, dataSizeEndField);
    }

    public String getAlgorithm() {
        return algorithmField.getText();
    }

    public String getFireStart() {
        return fireStartField.getText();
    }

    public String getFireEnd() {
        return fireEndField.getText();
    }

    public String getDataSizeStart() {
        return dataSizeStartField.getText();
    }

    public String getDataSizeEnd() {
        return dataSizeEndField.getText();
    }
}

