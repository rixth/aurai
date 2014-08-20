# aurai

Helping traditional air conditioners join the Internet of Things -  Aurai allows any AC that has a remote control to be controlled via a modern web interface.

This is an entry in to the Hackaday Prize. Details build logs are available on the [project page](https://hackaday.io/project/2288).

## The hardware

Aurai comprises of two hardware components. The first is the base  station (or "hub") which connects to a computer over a USB-UART. The second component, referred to as the "spoke", contains the infrared transmitter to talk to the AC and environmental monitoring sensors. The two units communicate with each other over a wireless connection.

## The software

I have found that many home automation software packages have atrocious interfaces. I tried to ensure that Aurai does not.

<img src="http://i.imgur.com/0uaebi3.png" alt="screenshot of the interface" width="400">

## The firmware

The hub & spoke communicate using a series of simple commands over the radio link. The hub should be able to ask the spoke for current environmental data, or to turn on the AC, set target temperature, etc. The spoke firmware contains a state machine to keep track of the air conditioner's state, as some commands are not always valid (ie. you can't set the target temperature when the AC mode is set to "fan only").

## License

Aurai's hardware, firmware & software are licensed under the GPL. The project uses avr-libc extensively, which is under a modified BSD license.
