var gpio = require('./build/Release/gpio.node');
var GPIO = gpio.GPIO;
var PWM = gpio.PWM;
var joystick = new(require('joystick'))(0);

//gpio.setDebug(true);

var pin = new PWM("25");
pin.open()
pin.setMode(gpio.OUT);
pin.write(gpio.HIGH);
/*pin.frequency = 100;
pin.dutyCycle = 70;
pin.start();*/


process.on('SIGINT', function () {
    pin.stop();
    pin.close();
    process.exit(0);
});


/*joystick.on('axis', function (data) {
    if (data.number === 5) {
        var val = Math.floor((data.value + 32767) / 65534 * 30);
        pin.dutyCycle = 70 + val;
    }
});*/
