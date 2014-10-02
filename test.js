var gpio = require('./build/Release/gpio.node');
var GPIO = gpio.GPIO;
var PWM = gpio.PWM;

//gpio.setDebug(true);

var pin = new PWM("25");
pin.open()
pin.setMode(gpio.OUT);
pin.frequency = 1;
pin.dutyCycle = 50;
console.log(pin.frequency);
pin.start();
setInterval(function () {
    pin.dutyCycle = Math.min(100, pin.dutyCycle + 5);
}, 500);
setTimeout(function () {
    pin.stop();
    pin.close();
    process.exit();
}, 10000);
//pin.write(gpio.HIGH);

/*var button = new GPIO("17");
button.open();
button.setMode(gpio.IN);*/

/*process.on('SIGINT', function () {
    pin.close();
    //button.close();
    process.exit(0);
});

setInterval(function () {
    pin.toggle();
}, 500);*/
