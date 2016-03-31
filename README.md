#Sound synthesis on parallella board
##Introduction
This is a final year project. Implement the modal synthesis algorithm on the parallella board. Using each core to represent one string. I use the microserver version of the parallella, so the data transmits through the ethernet cable to the PC. In the PC side, recive the data and play it. The string parameters can be changed in real-time.
##Status
Mostly, it works. I use 16 cores to represent a guitar. However, the guitar usually has 6 strings. So there are some strings are the same. 
###Already finished:
* Modal synthesis
* Real-time parameters change
* Socket transmit
* Pick the velocity or force as output
###Need to be done:
* Input force signal
* Better UI for control the string
* General method to scale the output
* ...
