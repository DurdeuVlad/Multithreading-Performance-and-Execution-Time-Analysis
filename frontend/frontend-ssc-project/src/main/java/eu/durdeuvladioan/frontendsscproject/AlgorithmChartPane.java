package eu.durdeuvladioan.frontendsscproject;

import javafx.scene.chart.LineChart;
import javafx.scene.chart.NumberAxis;
import javafx.scene.chart.XYChart;

public class AlgorithmChartPane {
    private LineChart<Number, Number> lineChart;

    public AlgorithmChartPane() {
        NumberAxis xAxis = new NumberAxis();
        NumberAxis yAxis = new NumberAxis();
        lineChart = new LineChart<>(xAxis, yAxis);
        lineChart.setTitle("Algorithm Performance");
    }

    public LineChart<Number, Number> getChart() {
        return lineChart;
    }

    public void clearChart() {
        lineChart.getData().clear();
    }

    public void addSeries(XYChart.Series<Number, Number> series) {
        lineChart.getData().add(series);
    }
}
