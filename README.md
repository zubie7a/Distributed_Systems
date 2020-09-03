## Santiago Zubieta
#### Universidad EAFIT, 2014-2

## Distributed Systems
Labs and Challenges for the **Distributed Systems** course, involving a lot of neat stuff such as system design, software design, implementation, integration, concurrent programming, etc.

This repository focuses on the work done using [**MPI**](https://en.wikipedia.org/wiki/Message_Passing_Interface) _(Message Passing Interface)_, which allows to design parallel and concurrent algorithms to distribute the execution of computationally intensive applications in order to gain performance.

Algorithms have their **serial** and their **parallel** implementation, for performing benchmarks and comparing between the single core version and the version that can use arbitrary number of cores.

This was made for educational purposes, and is licensed under the **MIT License**. **OpenCV** and **OpenMPI** uses the **BSD License**.

### MPI Exam, Challenge and Course Project

#### Course Project
This deals with calculating the [Motion Vectors](http://en.wikipedia.org/wiki/Motion_vector) between two consecutive frames of a video. This is done by checking some 'image blocks' on the first frame, and seeking in the next frame where did that block most likely 'moved'. The corresponding place where the block moved is the block in the next frame with the least 'difference' _(count similar pixels)_ with the previous one.

Then we store a **Motion Vector** for each block detailing the distance and angle of movement. These vectors are very useful for predicting and/or reconstructing images, so less frames have to be actually stored, and the missing frames are reconstructed by very light-weight **Motion Vectors** previously calculated *(thats why encoding video takes __so long__ and its so __computationally intensive__, but playing back is not)*.

This program will deal only with finding these vectors *(and painting them over their frames)*, this drawing is done in **C/C++**, using **OpenCV**.

#### Challenge
It consists on making a **recommendation system**, reading a large table of ratings that **X** users registered to a movie database site have given to the **Y** movies available to rate in such site *(0: not yet seen, 1-5: rating)* and then using **MPI** for finding the most correlated users, in order to make suggestions of movies to see to each user *(as in, if we want to make suggestions to user A, lets find the user B most correlated to A, and suggest to A some movies that B has seen that A has not seen).*

#### Exam
It consists on solving **arithmetic expressions**, reading a large file with such operations, expressed as:

```
1+2=
7*6=
27/22=
21-14=
```

*(One per line)*
...and then using **MPI** for finding the result of these simple arithmetic expressions and writing them into a single file with the complete expression.
