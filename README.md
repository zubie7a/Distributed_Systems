##Santiago Zubieta
####EAFIT University, 2014-2

##Distributed Systems Course Labs And Challenges
Labs and Challenges for the Distributed Systems Course, involving a lot of neat stuff such as system design, software design, implementation, integration, etc.

###MPI Exam/Lab
This deals with calculating the [Motion Vectors](http://en.wikipedia.org/wiki/Motion_vector) between two consecutive frames of a video / images. Doing this sequentially is **VERY** intensive in calculations, so the greater idea is to handle it in several processors to fasten the time of such calculations.
This is done by checking some 'blocks' on the first frame and seeing in the next frame where did that block most likely 'moved' (some are completely destroyed, some are completely new, some move and remain intact, and some move while having part of their info changed). The corresponding place where the block moved is the block in the next frame with the least 'difference' with the previous one. Then we store a **Motion Vector** for each block detailing the distance and angle of movement. These vectors are very useful for predicting and/or reconstructing images, so less frames have to be actually stored, and the missing frames are reconstructed by very light-weight **Motion Vectors** previously calculated (thats why encoding video takes **SO LONG** and its so **COMPUTATIONALLY INTENSIVE** but playing back is not). This program will deal only with finding these vectors (and painting them over their frames), but not with the computing that is done afterwards using these vectors and very fancy error correction algorithms to actually encode a video and predict images using only these vectors. This is done in **C/C++**, using **OpenCV** to load images into memory (and comparing pixels), and in the future, using **MPI** to handle parallelism. This is done for educational purposes, and is licensed under the **BSD License** (same as **OpenCV**).

###ActiveMQ Exam/Lab
This deals with a **MOM** *(Message Oriented Middleware)* , in which we use **ActiveMQ** with **Java** for handling messages between a consumer and a producer (using a message queue). This is done for educational purposes, and is licensed under the [Apache 2.0](http://www.apache.org/licenses/LICENSE-2.0.html) license (same as **ActiveMQ**). For running, install and run **ActiveMQ**, which in my case was simply `$ brew install activemq`and then run it. While its running, do the following:

####Building the class files
Go to the folders of the source codes (Consumer and Producer), and do this at each:
  
`javac -cp ../activemq-all-5.10.0.jar:. Producer.java` ... for the producer  
`javac -cp ../activemq-all-5.10.0.jar:. Consumer.java` ... for the consumer  
  
This will create the .class files for both
####Running the class files
At the respective folders (where the class files are), do this at each:
  
`java -cp ../activemq-all-5.10.0.jar:. Producer` ... for the producer  
`java -cp ../activemq-all-5.10.0.jar:. Consumer` ... for the consumer  
  
This will run both the producer and the consumer applications
  
While running the producer, it will indefinitely read from **STDIN** lines, and these will be broadcaster to the consumer. If you happen to open more than one consumer (Which you definetively should not do, unless you modified the code so that it worked with *topics* instead of *queues*), there will be some *race conditions* where one of the consumers grabs first a broadcasted message, then the other grabs another, but not both at the same time.

There's an upcoming implementation which is *much* easier and shorter to implement in **NodeJS**, using a queue in **STOMP** *Simple/Streaming Text Orientated Message Protocol*, to send and receive messages between a consumer and a producer (*and taking NodeJS advantages to easily send JSON objects encoded in strings)*.