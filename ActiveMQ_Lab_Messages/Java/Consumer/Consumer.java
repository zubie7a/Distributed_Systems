import org.apache.activemq.ActiveMQConnectionFactory;
import javax.jms.ExceptionListener;
import javax.jms.MessageConsumer;
import javax.jms.MessageListener;
import javax.jms.JMSException;
import javax.jms.Destination;
import javax.jms.TextMessage;
import javax.jms.Connection;
import javax.jms.Message;
import javax.jms.Session;

public class Consumer implements ExceptionListener {
    private MessageListener listener;

    void processConsumer() {
        String clientID = "zubieta";
        // This is used for the TOPIC since every client needs an unique
        // ClientID so its better to specify it dynamically.
        try {
            ActiveMQConnectionFactory connectionFactory;
            // Create a ConnectionFactory Object
            String host = "tcp://localhost:61616";
            // Create a host string detailing the location of the host and port
            connectionFactory = new ActiveMQConnectionFactory(host);
            // Initialize the connection factory object using the host string

            Connection connection = connectionFactory.createConnection();
            // Connection is created from the connectionFactory object

            connection.setExceptionListener(this);
            // This class will serve as a listener to the connection's
            // exceptions, calling the 'onException' method whenever
            // they happen.

            // Create a Session object from the connection object
            Session session = connection.createSession(false,
                    Session.AUTO_ACKNOWLEDGE);

            // Create the destination (Topic or Queue)
            Destination destination = session.createQueue("MyQUEUE");
            // Destination destination = session.createTopic("MyTOPIC");
            // Lets make it a Topic so we can have several consumers listening

            // Create a MessageConsumer from the Session to the Topic or Queue
            MessageConsumer consumer = session.createConsumer(destination);
            // MessageConsumer consumer = session.createDurableSubscriber(
            //        (Topic) destination, "edwin");

            consumer.setMessageListener(listener);
            connection.start();
            // Start the connection

            /* ----- Synchronous receiving of messages: ------
             * --- can't use this if a listener is used ------
             *             
            Message message = consumer.receive(1000);
            // Wait a second for a message
            while (message != null) {
            // While no message has been received
                Thread.sleep(5000);
                // Wait 5 seconds
                if (message instanceof TextMessage) {
                    TextMessage textMessage = (TextMessage) message;
                    String text = textMessage.getText();
                    System.out.println("Received: " + text);
                } else {
                    System.out.println("Received: " + message);
                }
                message = consumer.receiveNoWait();
                // Attempt receiving the message again without waiting
            }
            consumer.close();
            session.close();
            connection.close();
            // We'd rather not use these since we want continuous listening.
            */
        } catch (Exception e) {
            System.out.println("Caught: " + e);
            e.printStackTrace();
        }
    }

    public Consumer() {
        // Initialize the MessageListener in the constructor
        listener = new MessageListener() {
            public void onMessage(Message msg) {
                // This is called once a message is received
                if (msg instanceof TextMessage) {
                    TextMessage textMessage = (TextMessage) msg;
                    String text = null;
                    try {
                        text = textMessage.getText();
                    } catch (JMSException e) {
                        e.printStackTrace();
                    }
                    System.out.println("Received: " + text);
                } else {
                    System.out.println("Received: " + msg);
                }
            }

        };
    }

    public synchronized void onException(JMSException ex) {
        // This is called in case that an exception happens
        System.out.println("JMS Exception occured. Shutting down consumer.");
    }

    public static void main(String[] args) throws Exception {
        // Main for the CONSUMER program
        Consumer c = new Consumer();
        System.out.println("Running Consumer...");
        c.processConsumer();
    }
}
