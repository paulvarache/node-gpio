Node GPIO
==============

A GPIO module for single board computers like the Raspberry Pi or the Banana Pi written in C++.
--------------

Based on the sysfs, this module provides:

- Simple GPIO
- PWM
- Capacitive touch

The capacitive touch object uses two pins. One for charging the capacitor and the second to get the value.

Check out the example, they are quite self explanatory.

GPIO
*Light a led*

```javaScript
    var gpio = require('node-gpio');
    var GPIO = gpio.GPIO;

    var led = new GPIO("28");
    led.open();
    led.setMode(gpio.OUT);
    led.write(gpio.HIGH);
```
*Wait for a button push*
```javaScript
    var gpio = require('node-gpio');
    var GPIO = gpio.GPIO;

    var button = new GPIO("28");
    button.open();
    button.setMode(gpio.IN);
    button.on("changed", function (value) {
        console.log(value);
    });
    button.listen();
```

PWM
*Lightly light a led*

```javaScript
    var gpio = require('node-gpio');
    var PWM = gpio.PWM;

    var led = new PWM("28");
    led.open();
    led.frequency = 100;
    led.dutyCycle = 50;
    led.start();
    led.stop();
```

CapacityTouch
*Display all events on a capacitive touch device*

```javaScript
    var gpio = require('node-gpio');
    var CapacityTouch = gpio.CapacityTouch;

    var touch = new CapacityTouch("24", "28");
    touch.open();
    touch.on("changed", function (data) {
        console.log("Value: " + data.value);
        console.log("Charge time: " + data.charge);
    });
    touch.listen();
```

Every GPIO can be closed with:

```javaScript
    pin.close();
```

An event listening can be stoped with:
```javaScript
    pin.stopListening();
```

Do whatever you want with this module, I don't care.
