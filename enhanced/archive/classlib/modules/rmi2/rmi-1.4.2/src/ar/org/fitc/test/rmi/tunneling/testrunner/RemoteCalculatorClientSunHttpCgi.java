package ar.org.fitc.test.rmi.tunneling.testrunner;

import ar.org.fitc.test.rmi.tunneling.integration.RemoteCalculatorClient;

public class RemoteCalculatorClientSunHttpCgi {

    public static void main(String[] args) {
        String[] data = {"cgi","90.0.0.1","3128","10.100.2.246","sun"};
        RemoteCalculatorClient.main(data);
    }

}
