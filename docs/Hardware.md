### Hardware is supposed to be:
- Cheap
- Off the shelf and readly available 
- Easy to work with
- Fast enough for send a live stream


### So far all the boxes have been check by:
##### Base components:
- esp32-cam 
- ov2640 IR with 160 degree FOV
- esp-32 programmer 

#### Alternative possibility:
- raspberry pi zero 
- ov2640 IR with 160 FOV

#### Powering it on:
Just like a [slimevr trakcer](https://docs.slimevr.dev/diy/components-guide.html)

- 2x1N5817 SCHOTTKY diodes 
- TP4056 USB-C charging board
- a bipositional switch 
- A 800mAh Li-Po battery 

This will make the eye tracker last for about 9 - 10h of playtime. 

One could also power it though a USB port as the esp requires only 5V and 300-500ma 

 A usb battery or two lipos with a 5.1v power regulator should also do the job.

#### Radical alternatives for a camera

Shave off the IR filter off of any camera module:
https://marksbench.com/electronics/removing-ir-filter-from-esp32-cam/

#### Ommited because of safety 
I'm ommiting any IR emmiters here purposfully - I have no idea if any of them are safe to use for prolonged periods of time and thus I leave them out. 
