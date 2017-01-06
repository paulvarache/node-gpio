var gpio = require('../node-gpio');

var led = new gpio.GPIO('14');

led.open();
led.setMode(gpio.OUT);
led.write(gpio.LOW);

setInterval(function () {
    led.toggle();
}, 100);

process.on('SIGINT', function () {
    led.write(gpio.LOW);
    led.close();
    process.exit();
});