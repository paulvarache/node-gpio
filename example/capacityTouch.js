/**
 * Uses the CapacityTouch object to switch between two light when touched
 */
var gpio = require('../node-gpio');
var GPIO = gpio.GPIO;
var CapacitiveTouch = gpio.CapacitiveTouch;

// Creates a new CapacityTouch object using the pin 24 as input ans the 25 as output
var touch = new CapacitiveTouch("24", "25");
touch.open();

// Create two leds output
var green = new GPIO('17');
var red = new GPIO('28');

red.open();
green.open();

red.setMode(gpio.OUT);
green.setMode(gpio.OUT);

red.write(gpio.HIGH);
green.write(gpio.LOW);

var redVal = 1;
var greenVal = 0;

// When the CapcityTouch changes this event is emitted
touch.on("changed", function (data) {
    if (data.value) {
        red.write(redVal ? 0 : 1);
        green.write(greenVal ? 0 : 1);
        greenVal = !greenVal;
        redVal = !redVal;
    }
});
// Start listening for touch events
touch.listen();

process.on('SIGINT', function () {
    touch.stopListen();
    red.close();
    green.close();
    touch.close();
});
