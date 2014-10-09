/**
 * Uses a joystick axis to show light variation using PWM
 */
var gpio = require('../node-gpio');
var PWM = gpio.PWM;
var joystick = new(require('joystick'))(0);


var led = new PWM('28');

led.open();
led.frequency = 100;
led.dutyCycle = 0;
led.start();

var val = 0;

joystick.on('axis', function (data) {
    if (data.number === 5) {
        // Getting a percent format of the value
        val = Math.floor((data.value + 32767) / 65534 * 100);
        led.dutyCycle = Math.min(100, val);
    }
});

process.on('SIGINT', function () {
    led.stop();
    led.close();
    process.exit();
})
