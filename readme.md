# Ws2812 Strip controller 
* Up to 8 strips with up to 1000 pixels per strip
* Controlled by uart input 
* Json schema data format
* 8 independ strips or tied to matrix display

![alt text](doc/overview.png "Overview")

## RGB2222 Color Format
The **RGB2222 format** is a method to represent colors in a compact manner using only 6 bits to define a color and 2 bits to define the brightness.   
Representatin of one byte   
BR[2] RED[2] GREEN[2] BLUE[2]  
Each segment of 2 bits can represent 4 intensity levels (from 0 to 3).
    - The possible values for each channel in **binär**:
        - 0: No intensity (off)
        - 1: Low intensity
        - 2: Medium intensity
        - 3: Full intensity

## JSON Serialized data format

### Setup strip 

{"pin": 2, "update":"triggered", "pixels":100, "autoclear":1, "pattern": [100, 240, 200],  "repeat": 1, "interval":100, "mode":"rotate-right"}

{"pin": 2, "update":"immediately", "pixels":100, "autoclear":1, "repeat": 0, "interval":100, "mode":"rotate-right"}

{"pin": 2, "pattern": [100, 240, 200]}

{"animation":0}

{"animation":1}

{"update-trigger":1}

### 





