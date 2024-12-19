package eu.durdeuvladioan.frontendsscproject;

import javafx.scene.control.TextArea;

public class AlgorithmOutputPane {
    private TextArea outputArea;

    public AlgorithmOutputPane() {
        outputArea = new TextArea();
        outputArea.setEditable(false);
    }

    public TextArea getOutputArea() {
        return outputArea;
    }

    public void appendOutput(String text) {
        outputArea.appendText(text);
    }

    public void clearOutput() {
        outputArea.clear();
    }
}
