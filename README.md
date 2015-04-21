##Santiago Zubieta
####EAFIT University, 2014-2

##Distributed Systems Course Labs And Challenges
Labs and Challenges for the Distributed Systems Course, involving a lot of neat stuff such as system design, software design, implementation, integration, etc. This repository focus on the work done using **MPI**, which allows to design parallel algorithms to distribute the execution of computationally intensive applications in order to gain performance. Algorithms have their serial and their parallel implementation, for performing benchmarks and comparing between the single core version and the version that can use arbitrary number of cores.

###MPI Exam, Challenge and Course Project

####Course Project
This deals with calculating the [Motion Vectors](http://en.wikipedia.org/wiki/Motion_vector) between two consecutive frames of a video / images. This is done by checking some 'image blocks' on the first frame, and seeking in the next frame where did that block most likely 'moved' *(some are completely destroyed, some are completely new, some move and remain intact, and some move while having part of their info changed)*. The corresponding place where the block moved is the block in the next frame with the least 'difference' with the previous one. Then we store a **Motion Vector** for each block detailing the distance and angle of movement. These vectors are very useful for predicting and/or reconstructing images, so less frames have to be actually stored, and the missing frames are reconstructed by very light-weight **Motion Vectors** previously calculated *(thats why encoding video takes **so long** and its so **computationally intensive** but playing back is not)*. This program will deal only with finding these vectors *(and painting them over their frames)*, but it does not deal with the computing that is done afterwards using these vectors and very fancy error correction algorithms to actually encode a video and predict images using only these vectors. This is done in **C/C++**, using **OpenCV**, done for educational purposes, and is licensed under the **BSD License** *(same as OpenCV)*.

####Challenge
It consists on making a **recommendation system**, reading a *large* table of ratings that **X** users registered to a movie database site have given to the **Y** movies available to rate in such site *(0: not yet seen, 1-5: rating)* and then using **MPI** for finding the most correlated users, in order to make suggestions of movies to see to each user *(as in, if we want to make suggestions to user A, lets find the user B most correlated to A, and suggest to A some movies that B has seen that A has not seen).*

####Exam
It consists on solving **arithmetic expressions**, reading a *large* file with such detailed as:
``
1+2=
7*6=
27/22=
21-14=
`` *(one per line)*
And then using **MPI** for finding the result of these simple arithmetic expressions and writing them into a single file with the complete expression.
