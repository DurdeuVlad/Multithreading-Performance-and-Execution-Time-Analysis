package eu.durdeuvladioan.frontendsscproject;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;

public class RunExternalProgram {
    public static void main(String[] args) {
        String exePath = "C:\\Users\\Public\\Documents\\GitHub\\ssc-labs\\proiect-nou\\main.exe";

        System.out.println("Running test with full parameters:");
        runTest("run quick_sort 0 10 100 200");

        System.out.println("Running test with fewer parameters:");
        runTest("test quick_sort 0 10");

        System.out.println("Running test with minimum parameters:");
        runTest("analyze quick_sort");
    }

    public static void runTest(String parameters) {
        try {
            // Construct the command
            String exePath = "C:\\Users\\Public\\Documents\\GitHub\\ssc-labs\\proiect-nou\\main.exe";
            String command = exePath + " " + parameters;

            // Execute the command
            Process process = Runtime.getRuntime().exec(command);

            // Read the output from the process
            try (BufferedReader reader = new BufferedReader(new InputStreamReader(process.getInputStream()))) {
                String line;
                while ((line = reader.readLine()) != null) {
                    System.out.println(line);
                }
            }

            // Wait for the process to complete
            int exitCode = process.waitFor();
            System.out.println("Process exited with code: " + exitCode);

        } catch (IOException | InterruptedException e) {
            e.printStackTrace();
        }
    }


}
