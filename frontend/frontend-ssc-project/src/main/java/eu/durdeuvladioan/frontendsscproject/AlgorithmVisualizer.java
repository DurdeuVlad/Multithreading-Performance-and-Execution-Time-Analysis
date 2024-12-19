package eu.durdeuvladioan.frontendsscproject;

import javafx.application.Application;
import javafx.application.Platform;
import javafx.scene.Scene;
import javafx.scene.chart.LineChart;
import javafx.scene.chart.NumberAxis;
import javafx.scene.chart.XYChart;
import javafx.scene.control.*;
import javafx.scene.layout.*;
import javafx.stage.Stage;
import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.ObjectMapper;
import java.io.*;
import java.util.ArrayList;
import java.util.List;
import java.util.logging.Level;
import java.util.logging.Logger;

public class AlgorithmVisualizer extends Application {
    private static final Logger logger = Logger.getLogger(AlgorithmVisualizer.class.getName());
    private AlgorithmInputPanel inputPane;
    private AlgorithmOutputPane outputPane;
    private AlgorithmChartPane chartPane;

    public static void main(String[] args) {
        launch(args);
    }

    @Override
    public void start(Stage primaryStage) {
        primaryStage.setTitle("Algorithm Visualizer");

        inputPane = new AlgorithmInputPanel();
        outputPane = new AlgorithmOutputPane();
        chartPane = new AlgorithmChartPane();

        Button executeButton = new Button("Execute");
        executeButton.setOnAction(e -> executeCommand());

        HBox inputBox = new HBox(10, inputPane.getInputFields(), executeButton);
        VBox mainLayout = new VBox(10, inputBox, outputPane.getOutputArea(), chartPane.getChart());
        Scene scene = new Scene(mainLayout, 1000, 600);

        primaryStage.setScene(scene);
        primaryStage.show();
    }

    private void executeCommand() {
        outputPane.clearOutput();

        String algorithm = inputPane.getAlgorithm();
        String fireStart = inputPane.getFireStart();
        String fireEnd = inputPane.getFireEnd();
        String dataSizeStart = inputPane.getDataSizeStart();
        String dataSizeEnd = inputPane.getDataSizeEnd();

        logger.log(Level.INFO, "Executing command with parameters: Algorithm={0}, FireStart={1}, FireEnd={2}, DataSizeStart={3}, DataSizeEnd={4}",
                new Object[]{algorithm, fireStart, fireEnd, dataSizeStart, dataSizeEnd});

        if (algorithm.isEmpty()) {
            outputPane.appendOutput("Algorithm field must be filled.");
            logger.log(Level.WARNING, "Input validation failed: Algorithm field is empty.");
            return;
        }

        String[] commandArray = constructCommandArray(algorithm, fireStart, fireEnd, dataSizeStart, dataSizeEnd);

        try {
            Process process = Runtime.getRuntime().exec(commandArray);

            new Thread(() -> handleProcessOutput(process.getInputStream())).start();
            new Thread(() -> handleProcessError(process.getErrorStream())).start();

            int exitCode = process.waitFor();
            Platform.runLater(() -> outputPane.appendOutput("Process exited with code: " + exitCode + "\n"));
            logger.log(Level.INFO, "Process exited with code: {0}", exitCode);

        } catch (IOException | InterruptedException e) {
            Platform.runLater(() -> outputPane.appendOutput("Error executing the program: " + e.getMessage() + "\n"));
            logger.log(Level.SEVERE, "Error executing the program.", e);
        }
    }

    private String[] constructCommandArray(String algorithm, String fireStart, String fireEnd, String dataSizeStart, String dataSizeEnd) {
        List<String> commandList = new ArrayList<>();
        commandList.add("C:\\Users\\Public\\Documents\\GitHub\\ssc-labs\\proiect-nou\\main.exe");

        if (fireStart != null && fireEnd != null && !fireStart.isEmpty() && !fireEnd.isEmpty()) {
            if (dataSizeStart != null && dataSizeEnd != null && !dataSizeStart.isEmpty() && !dataSizeEnd.isEmpty()) {
                commandList.add("run");
                commandList.add(algorithm);
                commandList.add(fireStart);
                commandList.add(fireEnd);
                commandList.add(dataSizeStart);
                commandList.add(dataSizeEnd);
            } else {
                commandList.add("test");
                commandList.add(algorithm);
                commandList.add(fireStart);
                commandList.add(fireEnd);
            }
        } else {
            commandList.add("analyze");
            commandList.add(algorithm);
        }

        return commandList.toArray(new String[0]);
    }


    private void handleProcessOutput(InputStream inputStream) {
        try (BufferedReader processOutput = new BufferedReader(new InputStreamReader(inputStream))) {
            StringBuilder jsonOutput = new StringBuilder();
            String line;
            while ((line = processOutput.readLine()) != null) {
                String finalLine = line;
                Platform.runLater(() -> outputPane.appendOutput(finalLine + "\n"));
                jsonOutput.append(line).append("\n");
            }
            parseAndDisplayResults(jsonOutput.toString());
            chartPane.populateChart(jsonOutput.toString());
        } catch (IOException e) {
            Platform.runLater(() -> outputPane.appendOutput("Error reading process output: " + e.getMessage() + "\n"));
            logger.log(Level.SEVERE, "Error reading process output.", e);
        }
    }

    private void handleProcessError(InputStream errorStream) {
        try (BufferedReader processError = new BufferedReader(new InputStreamReader(errorStream))) {
            String line;
            while ((line = processError.readLine()) != null) {
                String finalLine = line;
                Platform.runLater(() -> outputPane.appendOutput("ERROR: " + finalLine + "\n"));
            }
        } catch (IOException e) {
            Platform.runLater(() -> outputPane.appendOutput("Error reading process error output: " + e.getMessage() + "\n"));
            logger.log(Level.SEVERE, "Error reading process error output.", e);
        }
    }

    private void parseAndDisplayResults(String jsonOutput) {
        Platform.runLater(() -> {
            try {
                ObjectMapper objectMapper = new ObjectMapper();
                JsonNode rootNode = objectMapper.readTree(jsonOutput);
                chartPane.clearChart();
                XYChart.Series<Number, Number> series = new XYChart.Series<>();

                for (JsonNode node : rootNode) {
                    double x = node.get("x").asDouble();
                    double y = node.get("y").asDouble();
                    series.getData().add(new XYChart.Data<>(x, y));
                }

                chartPane.addSeries(series);
            } catch (IOException e) {
                outputPane.appendOutput("Error parsing JSON: " + e.getMessage() + "\n");
                logger.log(Level.SEVERE, "Error parsing JSON.", e);
            }
        });
    }
}