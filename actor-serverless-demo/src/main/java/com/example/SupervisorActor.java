package com.example;

import akka.actor.AbstractActor;
import akka.actor.ActorRef;
import akka.actor.OneForOneStrategy;
import akka.actor.Props;
import akka.actor.SupervisorStrategy;
import akka.event.Logging;
import akka.event.LoggingAdapter;
import akka.japi.pf.DeciderBuilder;
import scala.concurrent.duration.Duration;

public class SupervisorActor extends AbstractActor {
    private final LoggingAdapter log = Logging.getLogger(getContext().getSystem(), this);
    private ActorRef worker;

    static Props props() {
        return Props.create(SupervisorActor.class, SupervisorActor::new);
    }

    @Override
    public void preStart() {
        log.info("SupervisorActor started");
        // Create a single worker for simplicity, although we could use a router for multiple workers
        worker = getContext().actorOf(WorkerActor.props(), "worker1");
    }

    // Supervision Strategy: Restart the worker if an ArithmeticException occurs
    @Override
    public SupervisorStrategy supervisorStrategy() {
        return new OneForOneStrategy(
            10,
            Duration.create("1 minute"),
            DeciderBuilder.match(ArithmeticException.class, e -> {
                log.info("Supervisor decided to RESTART worker due to: " + e.getMessage());
                return SupervisorStrategy.restart();
            })
            .matchAny(e -> SupervisorStrategy.escalate())
            .build()
        );
    }

    @Override
    public Receive createReceive() {
        return receiveBuilder()
            .match(Messages.ProcessNumbers.class, processNumbers -> {
                log.info("Supervisor forwarding task to worker.");
                // Forward the message to the worker so the worker's reply goes directly to the original sender
                worker.forward(new Messages.ComputeSum(processNumbers.numbers), getContext());
            })
            .build();
    }
}
