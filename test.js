var gpio = require('./build/Release/gpio.node');
var GPIO = gpio.GPIO;

gpio.setDebug(true);

var pin = new GPIO();
pin.open()
pin.setMode(gpio.OUT);
pin.write(gpio.LOW);
pin.setMode(gpio.IN);
console.log(pin.read());
pin.close();
