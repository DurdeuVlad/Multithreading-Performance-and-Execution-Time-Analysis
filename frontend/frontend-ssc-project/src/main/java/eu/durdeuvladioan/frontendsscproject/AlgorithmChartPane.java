package eu.durdeuvladioan.frontendsscproject;

import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.ObjectMapper;
import javafx.application.Platform;
import javafx.scene.SnapshotParameters;
import javafx.scene.chart.LineChart;
import javafx.scene.chart.NumberAxis;
import javafx.scene.chart.XYChart;

import java.io.IOException;
import java.util.HashMap;
import java.util.Map;

import javafx.embed.swing.SwingFXUtils;
import javafx.scene.control.Button;
import javafx.scene.image.WritableImage;
import javafx.stage.FileChooser;
import javax.imageio.ImageIO;
import java.io.File;
import java.io.IOException;


public class AlgorithmChartPane {
    private LineChart<Number, Number> lineChart;
    int startThreadCount = 1, endThreadCount = 1, startDataSize = 1, endDataSize = 1;
    private Button saveImageButton;
    public AlgorithmChartPane() {
        NumberAxis xAxis = new NumberAxis();
        NumberAxis yAxis = new NumberAxis();
        lineChart = new LineChart<>(xAxis, yAxis);
        lineChart.setTitle("Algorithm Performance");
        // name the axes
        xAxis.setLabel("Data Size");
        yAxis.setLabel("Duration (ms)");
        saveImageButton = new Button("Save as PNG");
        saveImageButton.setOnAction(e -> saveChartAsPng("chart", false));
        saveImageButton.setDisable(true);
    }

    public LineChart<Number, Number> getChart() {
        return lineChart;
    }

    public Button getSaveImageButton() {
        return saveImageButton;
    }

    public void clearChart() {
        lineChart.getData().clear();
    }

    public void addSeries(XYChart.Series<Number, Number> series) {
        lineChart.getData().add(series);
    }

    public void populateChart(String jsonData, int testSize, boolean iterative) {
        Platform.runLater(() -> {
            try {
                ObjectMapper objectMapper = new ObjectMapper();

                if (jsonData == null || jsonData.trim().isEmpty()) {
                    System.err.println("Error: Empty or null JSON data provided.");
                    return;
                }

                JsonNode rootNode = objectMapper.readTree(jsonData);

                if (!rootNode.isArray()) {
                    System.err.println("Error: JSON data must be an array.");
                    return;
                }

                this.clearChart();
                lineChart.setTitle("Algorithm Performance");
                Map<Integer, XYChart.Series<Number, Number>> threadSeriesMap = new HashMap<>();
                String algorithm = "";
                for (JsonNode node : rootNode) {
                    if (!node.has("data_size") || !node.has("threads") || !node.has("result") || !node.get("result").has("duration")) {
                        System.err.println("Error: Missing required fields in JSON node.");
                        continue;
                    }

                    try {
                        lineChart.setTitle("Algorithm " + node.get("algorithm").asText() + " Performance - Test Size: " + testSize);
                        saveImageButton.setOnAction(e -> saveChartAsPng(node.get("algorithm").asText(), iterative));
                        algorithm = node.get("algorithm").asText();
                        if (startThreadCount > node.get("threads").asInt()) {
                            startThreadCount = node.get("threads").asInt();
                        }
                        if (endThreadCount < node.get("threads").asInt()) {
                            endThreadCount = node.get("threads").asInt();
                        }
                        if (startDataSize > node.get("data_size").asInt()) {
                            startDataSize = node.get("data_size").asInt();
                        }
                        if (endDataSize < node.get("data_size").asInt()) {
                            endDataSize = node.get("data_size").asInt();
                        }
                        int dataSize = node.get("data_size").asInt();
                        int threads = node.get("threads").asInt();
                        double duration = node.get("result").get("duration").asDouble();

                        XYChart.Series<Number, Number> series = threadSeriesMap.computeIfAbsent(threads, t -> {
                            XYChart.Series<Number, Number> newSeries = new XYChart.Series<>();
                            newSeries.setName("Threads: " + t);
                            return newSeries;
                        });

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
//                if (path != null) {
//                    saveChartAsPng(algorithm, path);
//                }
                saveImageButton.setDisable(false);
            } catch (JsonProcessingException e) {
                System.err.println("Error: Malformed JSON data.");
                e.printStackTrace();
            }
        });
    }

    public void saveChartAsPng(String functionName, boolean iterative) {
        Platform.runLater(() -> {
            String data = functionName+ " threads_"+ startThreadCount+ "_"+ endThreadCount+
                    " data_size_"+ startDataSize+ "_"+ endDataSize + (iterative ? "_iterative" : "");
            String filename = String.format("chart-%s.png", data);
            File file = null;
            FileChooser fileChooser = new FileChooser();
            fileChooser.setTitle("Save Chart as PNG");
            fileChooser.setInitialFileName(filename);
            fileChooser.getExtensionFilters().add(new FileChooser.ExtensionFilter("PNG Files", "*.png"));
            // select image directory
            fileChooser.setInitialDirectory(new File(System.getProperty("user.home")));
            file = fileChooser.showSaveDialog(null);

            if (file != null) {
                WritableImage image = lineChart.snapshot(new SnapshotParameters(), null);

                try {
                    ImageIO.write(SwingFXUtils.fromFXImage(image, null), "png", file);
                    System.out.println("Chart saved to: " + file.getAbsolutePath());
                } catch (IOException e) {
                    System.err.println("Error saving chart as PNG: " + e.getMessage());
                    e.printStackTrace();
                }
            }
        });
    }

}

