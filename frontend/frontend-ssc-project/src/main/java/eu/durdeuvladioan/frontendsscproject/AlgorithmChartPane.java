package eu.durdeuvladioan.frontendsscproject;

import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.ObjectMapper;
import javafx.application.Platform;
import javafx.scene.chart.LineChart;
import javafx.scene.chart.NumberAxis;
import javafx.scene.chart.XYChart;

import java.io.IOException;
import java.util.HashMap;
import java.util.Map;

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

    public void populateChart(String jsonData) {
        Platform.runLater(() -> {
            try {
                ObjectMapper objectMapper = new ObjectMapper();

                // Check if the input JSON is valid
                if (jsonData == null || jsonData.trim().isEmpty()) {
                    System.err.println("Error: Empty or null JSON data provided.");
                    return;
                }

                // Parse the JSON input
                JsonNode rootNode = objectMapper.readTree(jsonData);

                if (!rootNode.isArray()) {
                    System.err.println("Error: JSON data must be an array.");
                    return;
                }

                this.clearChart();

                // Map to hold thread-based series
                Map<Integer, XYChart.Series<Number, Number>> threadSeriesMap = new HashMap<>();

                for (JsonNode node : rootNode) {
                    // Safeguard against missing fields
                    if (!node.has("data_size") || !node.has("threads") || !node.has("result") || !node.get("result").has("duration")) {
                        System.err.println("Error: Missing required fields in JSON node.");
                        continue;
                    }

                    try {
                        int dataSize = node.get("data_size").asInt();
                        int threads = node.get("threads").asInt();
                        double duration = node.get("result").get("duration").asDouble();

                        // Get or create the series for this thread count
                        XYChart.Series<Number, Number> series = threadSeriesMap.computeIfAbsent(threads, t -> {
                            XYChart.Series<Number, Number> newSeries = new XYChart.Series<>();
                            newSeries.setName("Threads: " + t);
                            return newSeries;
                        });

                        // Add data size to X-axis and duration to Y-axis
                        series.getData().add(new XYChart.Data<>(dataSize, duration));
                    } catch (Exception e) {
                        System.err.println("Error processing node: " + node);
                        e.printStackTrace();
                    }
                }

                // Add all series to the chart
                for (XYChart.Series<Number, Number> series : threadSeriesMap.values()) {
                    this.addSeries(series);
                }
            } catch (JsonProcessingException e) {
                System.err.println("Error: Malformed JSON data.");
                e.printStackTrace();
            } catch (IOException e) {
                System.err.println("Error: IO exception during JSON processing.");
                e.printStackTrace();
            }
        });
    }



}

