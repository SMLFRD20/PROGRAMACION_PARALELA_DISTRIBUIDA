package com.example;

import java.util.List;

public class Messages {
    
    // Command sent to the Supervisor
    public static class ProcessNumbers {
        public final List<Integer> numbers;

        public ProcessNumbers(List<Integer> numbers) {
            this.numbers = numbers;
        }
    }

    // Command sent to the Worker
    public static class ComputeSum {
        public final List<Integer> numbers;

        public ComputeSum(List<Integer> numbers) {
            this.numbers = numbers;
        }
    }

    // Response from the Worker
    public static class SumResult {
        public final int result;

        public SumResult(int result) {
            this.result = result;
        }
    }
}
