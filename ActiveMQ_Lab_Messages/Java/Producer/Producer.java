import org.apache.activemq.ActiveMQConnectionFactory;
import javax.jms.ExceptionListener;
import javax.jms.MessageProducer;
import javax.jms.MessageListener;
import javax.jms.JMSException;
import javax.jms.Destination;
import javax.jms.TextMessage;
import javax.jms.Connection;
import javax.jms.Session;

import java.io.BufferedReader;
import java.io.InputStreamReader;

public class Producer implements ExceptionListener {

    void processProducer() {
        try {
            ActiveMQConnectionFactory connectionFactory;
            // Create a ConnectionFactory Object
            String host = "tcp://localhost:61616";
            // Create a host string detailing the location of the host and port
            connectionFactory = new ActiveMQConnectionFactory(host);
            // Initialize the connection factory object using the host string

            Connection connection = connectionFactory.createConnection();
            // Connection is created from the connectionFactory object
            connection.start();
            // Start the connection

            // Create a Session
            Session session = connection.createSession(false,
                    Session.AUTO_ACKNOWLEDGE);

            // Create the destination (Topic or Queue)
            Destination destination = session.createQueue("MyQUEUE");
            // Destination destination = session.createTopic("MyTOPIC");

            // Create a MessageProducer from the Session to the Topic or Queue
            MessageProducer producer = session.createProducer(destination);

            BufferedReader br;
            br = new BufferedReader(new InputStreamReader(System.in));
            // In case of reading from a text file: new FileReader(filename).

            // To output to a file:
            // BufferedWriter out;
            // out = new BufferedWriter(new OutputStreamWriter(System.out));

            while (true) {
                // This will read lines from STDIN. If a line contains spaces or
                // commas, it will split it there into a array of strings
                String line = br.readLine();
                String arr[] = line.split("[, ]");
                // Split using a regular expression. In this case at ',' or ' '
                for (int i = 0; i < arr.length; i++) {
                    TextMessage message = session.createTextMessage(arr[i]);
                    System.out.printf("Sent Message: %s\n", arr[i]);
                    producer.send(message);
                }
            }
            // session.close();
            // connection.close();
        }
        catch (Exception e) {
            System.out.println("Caught: " + e);
            e.printStackTrace();
        }
    }

    public synchronized void onException(JMSException ex) {
        System.out.println("JMS Exception occured. Shutting down producer.");
    }

    public Producer() {
        // Empty constructor!
    }

    public static void main(String[] args) throws Exception {
        Producer p = new Producer();
        System.out.println("Running ActiveMQ Producer...");
        p.processProducer();
    }
}
