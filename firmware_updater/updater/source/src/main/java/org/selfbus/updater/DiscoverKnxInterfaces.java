package org.selfbus.updater;

import io.calimero.knxnetip.Discoverer;
import io.calimero.knxnetip.servicetype.SearchResponse;

import java.time.Duration;
import java.util.List;
import java.util.concurrent.CompletableFuture;
import java.util.concurrent.ExecutionException;

public class DiscoverKnxInterfaces {
    public static List<Discoverer.Result<SearchResponse>> getAllInterfaces(){

        List<Discoverer.Result<SearchResponse>> interfacesResult = null;
        try {
            // set true to be aware of Network Address Translation (NAT) during discovery
            final boolean useNAT = false;
            interfacesResult = Discoverer.udp(useNAT).timeout(Duration.ofSeconds(3)).search().get();
        }catch (InterruptedException | ExecutionException e) {
            System.err.println("Error during KNXnet/IP discovery: " + e);
        }

        return interfacesResult;
    }
}
