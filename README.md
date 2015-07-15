# sismo-mixed-bag
A couple of short programs using a piezo &amp; a MPU-6050


<h1>log.c</h1>
log.c (log.mk) accepts data from the Arduino sketch piezo.ino;
it runs on a Raspberry Pi.

usage is 

$ ./log < /dev/ttyUSB0 > log.`now` &
('now' is 'date "+%g%m%d.%H%M%S"')

The idea is to buffer the data coming from the serial line
and write it in chunks.  Moreover, delta-t are printed on STDERR every 1000 sample
to give an idea of the # of sample per seconds.

<h1>piezo.ino</h1>
A simple Arduino sketch which ouputs analogread(0) @ 38400 bps with a sample rate
of about 400 sample per second.

<h1>mpu6050.c</h1>
A short program dumping accelarations coming from a MPU-6050 via I2C.
(Only jerks do that but it is the shortest path from the datasheet to the code
and from the code to the datasheet... ; defines in an included file just obscurs
the matter...)

The idea was to compare the reading of a MPU-6050 with a petanque boule on a piezo.
See : http://chipotons.blogspot.be/2015/06/sismo-piezo.html
