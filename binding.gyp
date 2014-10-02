{
    "targets": [{
        "target_name": "gpio",
        "sources": ["src/node-gpio.cpp", "src/GPIO.cpp", "src/PWM.cpp"],
        'cflags': ['-fexceptions'],
        'cflags_cc': ['-fexceptions']
    }]
}
