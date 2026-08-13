package com.example;

import com.fasterxml.jackson.databind.ObjectMapper;
import com.microsoft.azure.functions.ExecutionContext;
import com.microsoft.azure.functions.HttpMethod;
import com.microsoft.azure.functions.HttpRequestMessage;
import com.microsoft.azure.functions.HttpResponseMessage;
import com.microsoft.azure.functions.HttpStatus;
import com.microsoft.azure.functions.annotation.AuthorizationLevel;
import com.microsoft.azure.functions.annotation.FunctionName;
import com.microsoft.azure.functions.annotation.HttpTrigger;

import akka.actor.ActorRef;
import akka.actor.ActorSystem;
import akka.pattern.Patterns;

import java.util.List;
import java.util.Map;
import java.util.Optional;
import java.util.concurrent.CompletionStage;
import java.util.concurrent.TimeUnit;

public class Function {
    // We instantiate the ActorSystem and Supervisor once per function app instance
    private static final ActorSystem system;
    private static final ActorRef supervisor;
    private static final ObjectMapper mapper = new ObjectMapper();

    static {
        System.out.println("Initializing Akka ActorSystem...");
        system = ActorSystem.create("ServerlessActorSystem");
        supervisor = system.actorOf(SupervisorActor.props(), "supervisor");
    }

    @FunctionName("processNumbers")
    public HttpResponseMessage run(
            @HttpTrigger(
                name = "req",
                methods = {HttpMethod.POST},
                authLevel = AuthorizationLevel.ANONYMOUS)
                HttpRequestMessage<Optional<String>> request,
            final ExecutionContext context) {
        
        context.getLogger().info("Java HTTP trigger processed a request.");

        String bodyString = request.getBody().orElse(null);
        if (bodyString == null || bodyString.isEmpty()) {
            return request.createResponseBuilder(HttpStatus.BAD_REQUEST)
                    .body("Please pass a JSON body with a 'numbers' array (e.g. {\"numbers\": [1,2,3]}).")
                    .build();
        }

        try {
            // Parse JSON body
            Map<String, Object> bodyMap = mapper.readValue(bodyString, Map.class);
            if (!bodyMap.containsKey("numbers")) {
                return request.createResponseBuilder(HttpStatus.BAD_REQUEST)
                        .body("Missing 'numbers' array in JSON.")
                        .build();
            }

            List<Integer> numbers = (List<Integer>) bodyMap.get("numbers");
            context.getLogger().info("Extracted numbers: " + numbers);

            // Send message to the Supervisor Actor and wait for response (Ask pattern)
            Messages.ProcessNumbers msg = new Messages.ProcessNumbers(numbers);
            java.time.Duration timeout = java.time.Duration.ofSeconds(5);
            
            context.getLogger().info("Asking Supervisor Actor...");
            CompletionStage<Object> futureResult = Patterns.ask(supervisor, msg, timeout);
            
            // Await the result synchronously (Serverless functions typically block until they finish)
            Messages.SumResult result = (Messages.SumResult) futureResult.toCompletableFuture().get(5, TimeUnit.SECONDS);
            
            context.getLogger().info("Got result from Actor: " + result.result);
            
            return request.createResponseBuilder(HttpStatus.OK)
                    .body(mapper.writeValueAsString(result))
                    .header("Content-Type", "application/json")
                    .build();

        } catch (Exception e) {
            context.getLogger().severe("Error processing request: " + e.getMessage());
            return request.createResponseBuilder(HttpStatus.INTERNAL_SERVER_ERROR)
                    .body("Error processing request: " + e.getMessage())
                    .build();
        }
    }
}
