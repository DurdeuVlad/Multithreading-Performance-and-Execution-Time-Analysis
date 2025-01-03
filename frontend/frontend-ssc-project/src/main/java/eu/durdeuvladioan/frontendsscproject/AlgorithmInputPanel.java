package eu.durdeuvladioan.frontendsscproject;

import javafx.geometry.VerticalDirection;
import javafx.scene.chart.LineChart;
import javafx.scene.chart.NumberAxis;
import javafx.scene.chart.XYChart;
import javafx.scene.control.Button;
import javafx.scene.control.CheckBox;
import javafx.scene.control.TextArea;
import javafx.scene.control.TextField;
import javafx.scene.layout.HBox;

import javax.sound.sampled.Line;

public class AlgorithmInputPanel {
    private TextField algorithmField;
    private TextField fireStartField;
    private TextField fireEndField;
    private TextField dataSizeStartField;
    private TextField dataSizeEndField;
    private CheckBox useIterativeCheckBox;
    private TextField repeatField;
    private Button executeAutomatedTestsButton;

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

        useIterativeCheckBox = new CheckBox("Use Iterative");
        useIterativeCheckBox.setSelected(false);

        repeatField = new TextField();
        repeatField.setPromptText("Repeat tests count");


    }

    public HBox getInputFields() {
        return new HBox(10, algorithmField, fireStartField, fireEndField, dataSizeStartField, dataSizeEndField, useIterativeCheckBox, repeatField);
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

    public boolean getUseIterative() {
        return useIterativeCheckBox.isSelected();
    }

    public int getRepeat() {
        try {
            return Integer.parseInt(repeatField.getText());
        } catch (NumberFormatException e) {
            return 1;
        }
    }
}

