# Ws2812 Strip controler 
* 8 strips
* controlled by uart
* json 
![alt text](doc/overview.png "Overview")

## RGB2222 Format for color 
bit 6,7 brightness b00 -> b11 
bit 4,5 red b00 -> b11
bit 2,3 green b00 -> b11
bit 0,1 blue b00 -> b11

## JSON Serialized data format

### Setup strip 

{"pin": 2, "update":"triggered", "pixels":100, "autoclear":1, "pattern": [100, 240, 200],  "repeat": 1, "interval":100, "mode":"rotate-right"}

{"pin": 2, "update":"immediately", "pixels":100, "autoclear":1, "repeat": 0, "interval":100, "mode":"rotate-right"}

{"pin": 2, "pattern": [100, 240, 200]}

{"animation":0}

{"animation":1}

{"update-trigger":1}

### 





