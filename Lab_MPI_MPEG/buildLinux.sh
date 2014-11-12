LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib/
mpicxx -I '/usr/local/include' -L '/usr/local/lib/' -lopencv_core -lopencv_highgui -lopencv_imgproc mpienc.cpp -o mpienc
mpicxx -I '/usr/local/include' -L '/usr/local/lib/' -lopencv_core -lopencv_highgui -lopencv_imgproc serialenc.cpp -o serialenc
