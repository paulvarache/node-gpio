/**
 * Just binds a button to a led
 */
var gpio = require('../node-gpio');
var GPIO = gpio.GPIO;

var led = new GPIO("28");
var button = new GPIO("18");

led.open();
led.setMode(gpio.OUT);

button.open();
button.setMode(gpio.IN);
button.write(gpio.LOW);

button.on("changed", function (value) {
    console.log(value);
    if (value) {
        led.write(gpio.HIGH);
    } else {
        led.write(gpio.LOW);
    }
});
button.listen();

process.on('SIGINT', function () {
    button.close();
    led.close();
    process.exit();
})
