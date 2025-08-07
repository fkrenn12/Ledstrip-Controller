# WS2812 Strip controller 
* Up to 8 strips with up to 1000 pixels per strip
* Controlled by uart input 
* Json schema data format
* 8 independ strips or tied to matrix display

![alt text](doc/overview.png "Overview")

## RGB2222 Color Format
The **RGB2222 format** is a method to represent colors in a compact manner using only 6 bits to define a color and 2 bits to define the brightness.   
### Representatin of one byte   
BRIGHTNESS[2] RED[2] GREEN[2] BLUE[2]  
Each segment of 2 bits can represent 4 intensity levels (from 0 to 3).

The possible values for each **color channel (RED, GREEN, BLUE)** in **binär**:  
00: No intensity (off)  
01: Low intensity  
10: Medium intensity  
11: Full intensity  

The possible values for the **brighness channel** in **binär**:  
00: Very Low intensity
01: Low intensity  
10: Medium intensity  
11: Full intensity  

## JSON Schema data format

### Setup strip 

{"strip": 1, "update-mode":"triggered", "pixels":100, "autoclear":1, "pattern": [100, 240, 200],  "repeat": 1, "interval":100, "animation-mode":"rotate-right"}

{"strip": 2, "update":"immediately", "pixels":100, "autoclear":1, "repeat": 0, "interval":100, "mode":"rotate-right"}

{"strip": 1, "pattern": [100, 240, 200]}

{"animation":0}

{"animation":1}

{"update-trigger":1}

### 





