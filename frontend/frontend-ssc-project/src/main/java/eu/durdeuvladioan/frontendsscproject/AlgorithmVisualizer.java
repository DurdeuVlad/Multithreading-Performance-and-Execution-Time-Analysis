package eu.durdeuvladioan.frontendsscproject;

import javafx.application.Application;
import javafx.application.Platform;
import javafx.geometry.Pos;
import javafx.geometry.Rectangle2D;
import javafx.scene.Scene;
import javafx.scene.chart.LineChart;
import javafx.scene.chart.NumberAxis;
import javafx.scene.chart.XYChart;
import javafx.scene.control.*;
import javafx.scene.input.ScrollEvent;
import javafx.scene.layout.*;
import javafx.stage.DirectoryChooser;
import javafx.stage.FileChooser;
import javafx.stage.Screen;
import javafx.stage.Stage;
import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.ObjectMapper;
import java.io.*;
import java.util.ArrayList;
import java.util.Arrays;
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
//        Button executeAutomatedTestsButton = new Button("Execute Automated Tests");
//        executeAutomatedTestsButton.setOnAction(e -> executeAutomatedTests());

        HBox inputBox = new HBox(10, inputPane.getInputFields(), executeButton);


        VBox topSection = new VBox(10, inputBox, outputPane.getOutputArea());

        LineChart<Number, Number> chart = chartPane.getChart();
        chart.setMinHeight(400); // Minimum height for the chart
        VBox.setVgrow(chart, Priority.ALWAYS);

        VBox mainLayout = new VBox(10, topSection, chart);

        Button saveImageButton = chartPane.getSaveImageButton();
        // add the button to the buttom middle of the layout
        mainLayout.getChildren().add(saveImageButton);
        mainLayout.setAlignment(Pos.CENTER);

        Scene scene = new Scene(mainLayout);

        primaryStage.setScene(scene);

        Rectangle2D screenBounds = Screen.getPrimary().getVisualBounds();
        primaryStage.setX(screenBounds.getMinX());
        primaryStage.setY(screenBounds.getMinY());
        primaryStage.setWidth(screenBounds.getWidth());
        primaryStage.setHeight(screenBounds.getHeight());

        topSection.setPrefHeight(screenBounds.getHeight() / 3);

        addZooming(chart);

        primaryStage.show();
    }

    private void addZooming(LineChart<Number, Number> chart) {
        chart.setOnScroll((ScrollEvent event) -> {
            double zoomFactor = 0.1;
            NumberAxis xAxis = (NumberAxis) chart.getXAxis();
            NumberAxis yAxis = (NumberAxis) chart.getYAxis();

            if (event.getDeltaY() > 0) {
                xAxis.setLowerBound(xAxis.getLowerBound() + zoomFactor * (xAxis.getUpperBound() - xAxis.getLowerBound()));
                xAxis.setUpperBound(xAxis.getUpperBound() - zoomFactor * (xAxis.getUpperBound() - xAxis.getLowerBound()));
                yAxis.setLowerBound(yAxis.getLowerBound() + zoomFactor * (yAxis.getUpperBound() - yAxis.getLowerBound()));
                yAxis.setUpperBound(yAxis.getUpperBound() - zoomFactor * (yAxis.getUpperBound() - yAxis.getLowerBound()));
            } else {
                xAxis.setLowerBound(xAxis.getLowerBound() - zoomFactor * (xAxis.getUpperBound() - xAxis.getLowerBound()));
                xAxis.setUpperBound(xAxis.getUpperBound() + zoomFactor * (xAxis.getUpperBound() - xAxis.getLowerBound()));
                yAxis.setLowerBound(yAxis.getLowerBound() - zoomFactor * (yAxis.getUpperBound() - yAxis.getLowerBound()));
                yAxis.setUpperBound(yAxis.getUpperBound() + zoomFactor * (yAxis.getUpperBound() - yAxis.getLowerBound()));
            }

            event.consume();
        });
    }

//    public void executeAutomatedTests() {
//        // Set the default directory to the "images" folder in the user's home directory
//        String userHome = System.getProperty("user.home");
//        File defaultDirectory = new File(userHome, "images");
//
//        if (!defaultDirectory.exists() && !defaultDirectory.mkdirs()) {
//            Logger.getLogger(AlgorithmVisualizer.class.getName())
//                    .log(Level.SEVERE, "Failed to create default directory: {0}", defaultDirectory.getAbsolutePath());
//            outputPane.appendOutput("Failed to create the default directory.");
//            return;
//        }
//
//        DirectoryChooser directoryChooser = new DirectoryChooser();
//        directoryChooser.setTitle("Select the folder where the images will be saved");
//        directoryChooser.setInitialDirectory(defaultDirectory);
//
//        File selectedDirectory = directoryChooser.showDialog(null);
//
//        if (selectedDirectory == null) {
//            Logger.getLogger(AlgorithmVisualizer.class.getName())
//                    .log(Level.INFO, "No directory selected. Using default directory: {0}", defaultDirectory.getAbsolutePath());
//            selectedDirectory = defaultDirectory;
//        }
//
//        Logger.getLogger(AlgorithmVisualizer.class.getName())
//                .log(Level.INFO, "Images will be saved in: {0}", selectedDirectory.getAbsolutePath());
//
//        List<String> sortingAlgorithms = Arrays.asList("quick_sort", "bubble_sort", "insertion_sort", "selection_sort", "merge_sort", "heap_sort");
//
//        for (String algorithm : sortingAlgorithms) {
//            String jsonData = executeCommandWithParameters(algorithm, "0", "5", "10", "15", false);
//            if (jsonData == null) {
//                continue;
//            }
//            File outputFile = new File(selectedDirectory, algorithm + ".png");
//
//            chartPane.populateChart(jsonData, inputPane.getRepeat(), outputFile.getAbsolutePath());
//
//
//        }
//
//
//
//    }

    private String executeCommandWithParameters(String algorithm, String fireStart, String fireEnd, String dataSizeStart, String dataSizeEnd, boolean useIterative) {
        logger.log(Level.INFO, "Executing command with parameters: Algorithm={0}, FireStart={1}, FireEnd={2}, DataSizeStart={3}, DataSizeEnd={4}, UseIterative={5}",
                new Object[]{algorithm, fireStart, fireEnd, dataSizeStart, dataSizeEnd, useIterative});

        if (algorithm.isEmpty()) {
            outputPane.appendOutput("Algorithm field must be filled.");
            logger.log(Level.WARNING, "Input validation failed: Algorithm field is empty.");
            return null;
        }

        String[] commandArray = constructCommandArray(algorithm, fireStart, fireEnd, dataSizeStart, dataSizeEnd, useIterative);
        System.out.println(Arrays.toString(commandArray));
        try {
            Process process = Runtime.getRuntime().exec(commandArray);

            String jsonString = handleProcessOutput(process.getInputStream());
            handleProcessError(process.getErrorStream());
            if (jsonString == null) {
                return null;
            }
            int exitCode = process.waitFor();
            //Platform.runLater(() -> outputPane.appendOutput("Process exited with code: " + exitCode + "\n"));
            logger.log(Level.INFO, "Process exited with code: {0}", exitCode);
            return jsonString;

        } catch (IOException | InterruptedException e) {
            Platform.runLater(() -> outputPane.appendOutput("Error executing the program: " + e.getMessage() + "\n"));
            logger.log(Level.SEVERE, "Error executing the program.", e);
        }
        return null;

    }
    private void executeCommand() {
        outputPane.clearOutput();

        String algorithm = inputPane.getAlgorithm();
        String fireStart = inputPane.getFireStart();
        String fireEnd = inputPane.getFireEnd();
        String dataSizeStart = inputPane.getDataSizeStart();
        String dataSizeEnd = inputPane.getDataSizeEnd();
        boolean useIterative = inputPane.getUseIterative();

        executeCommandWithParameters(algorithm, fireStart, fireEnd, dataSizeStart, dataSizeEnd, useIterative);

    }

    private String[] constructCommandArray(String algorithm, String fireStart, String fireEnd, String dataSizeStart, String dataSizeEnd, boolean useIterative) {
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
        if (useIterative) {
            commandList.add("--use-iterative");
        }
        if (inputPane.getRepeat() > 0) {
            commandList.add("--repeat=" + inputPane.getRepeat());
        }

        return commandList.toArray(new String[0]);
    }


    private String handleProcessOutput(InputStream inputStream) {
        try (BufferedReader processOutput = new BufferedReader(new InputStreamReader(inputStream))) {
            StringBuilder jsonOutput = new StringBuilder();
            String line;
            while ((line = processOutput.readLine()) != null) {
                String finalLine = line;
                Platform.runLater(() -> outputPane.appendOutput(finalLine + "\n"));
                jsonOutput.append(line).append("\n");
            }
            parseAndDisplayResults(jsonOutput.toString());
            chartPane.populateChart(jsonOutput.toString(), inputPane.getRepeat(), inputPane.getUseIterative());
            return jsonOutput.toString();
        } catch (IOException e) {
            Platform.runLater(() -> outputPane.appendOutput("Error reading process output: " + e.getMessage() + "\n"));
            logger.log(Level.SEVERE, "Error reading process output.", e);
        }
        return null;
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