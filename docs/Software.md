Aight, so this is not decided at all and serves as a "throw it all in and figure it as you go" document, so here it goes. 

The goal is to: 

- detect pupil position send that data though websockets / OSC back to Neos/Vrchat. 
- detect eye lids position and send that data though websockets / OSC back to Neos/Vrchat

Now, having that out of the way, how should we process the data? It's being sent to us as an uncompressed stream from the ESP. 

We could use the OpenCV for image processing but what out detection? 

DLib has a very good eye / face detection pre-trained model 
There are some ready-to-go solutions for openCV but won't they be too heavy? 

How about training out own CNN model based on yey data sets? 
They would require labeling but they were used with great success by others. 

Datasets - https://datagen.tech/blog/eye-datasets/
TowardsDataScience showcasing DLib used for eye tracking (no eye lids) - https://towardsdatascience.com/real-time-eye-tracking-using-opencv-and-dlib-b504ca724ac6
some more research using dlib - https://pyimagesearch.com/2017/04/24/eye-blink-detection-opencv-python-dlib/
eyeloop, looks intereg - https://github.com/simonarvin/eyeloop 

Now for some papers:
http://stanford.edu/class/ee267/Spring2018/report_griffin_ramirez.pdf - eye tracing using CNN

For a stupid version, we could blink the eyes if there are no pupils detected? But that means no stupid faces :c 