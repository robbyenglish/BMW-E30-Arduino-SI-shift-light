# BMW-E30-Arduino-SI-shift-light
Arduino code for turning BMW E30 SI into shift lights

Arduino will need a power supply, rpm signal and wiring to new leds which you will need to install in the gauge cluster where si lights used to be.

I have an M54 engine with 6500rpm redline. Change the rpms variable to adjust when you want the lights to turn on for your engine. 

Designed for 7 segment si but could work with other number of leds with some changes.

RPM signal taken form black wire on the blue connector on the back of the gauge cluster. I beleve this wire gives a pulse for every ignition event, so 3 times per revolution for 6 cylinder 4 stroke. For use with 4 of 8 cylinder engine you will need to made some changes in the code. 

I used an arduino nano every but code should work with others. I used a nano every as the voltage regulated is rated up to 24v so more suitable for 14v supply in the car, other arduinos are only rated to 12v so might need an external volatge regulator. 

I am not very good at programing, this code is definitly not as good as it could be but it does work:). I have borrowed some sections from speeduino code. The flashing function should probably be on a timed interupt but I couldn't get this to work so its just called form the main loop. 
