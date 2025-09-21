## minimal, simple to replicate addressable LED Strip controller  
currently uses ESP32, but can run on any fastled supported boards with minimal modification  
while using an esp32, data pin is 23  
the strip can be powered directly from the micro usb port using a power bank  
brightness is set to 50% for this use case. if powering the strip directly, feel free to bump it up  
the onboard boot button is a mode selector, with onboard led blinks on mode change  

![example](ledstrip-in-use.gif)