package com.example;

import akka.actor.AbstractActor;
import akka.actor.Props;
import akka.event.Logging;
import akka.event.LoggingAdapter;

public class WorkerActor extends AbstractActor {
    private final LoggingAdapter log = Logging.getLogger(getContext().getSystem(), this);

    static Props props() {
        return Props.create(WorkerActor.class, WorkerActor::new);
    }

    @Override
    public void preStart() {
        log.info("WorkerActor started");
    }

    @Override
    public void postStop() {
        log.info("WorkerActor stopped");
    }

    @Override
    public Receive createReceive() {
        return receiveBuilder()
            .match(Messages.ComputeSum.class, computeSum -> {
                log.info("Worker received task to sum: " + computeSum.numbers);
                
                int sum = 0;
                for (int num : computeSum.numbers) {
                    if (num < 0) {
                        log.error("Worker encountered a negative number! Simulating failure.");
                        throw new ArithmeticException("Negative numbers are not allowed: " + num);
                    }
                    sum += num;
                }
                
                log.info("Worker computed sum: " + sum);
                getSender().tell(new Messages.SumResult(sum), getSelf());
            })
            .build();
    }
}
