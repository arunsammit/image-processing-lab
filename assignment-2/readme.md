### For building the histogram_equalization.cpp file the following command on terminal can be used
``` shell
g++ -g ./histogram_equalization.cpp lib/EqualizeTransform.cpp lib/CVcommonTask.cpp -lstdc++fs -std=c++17 -o build/histogram_equalization `pkg-config --cflags --libs opencv4`
```
### For building the histogram_matching.cpp file the following command on terminal can be used
``` shell
g++ -g ./histogram_matching.cpp lib/EqualizeTransform.cpp CVcommonTask.cpp -lstdc++fs -std=c++17 -o ./histogram_matching `pkg-config --cflags --libs opencv`
```
### For Running the built file
```shell
./histogram_equalization input_images output_images/
./histogram_matching input_images/livingroom.jpg output_images/mandril_color_equalized.jpg output_images/matched/
```