# WS2812 Strip controller 
* Up to 8 strips with up to 1000 pixels per strip
* Controlled by uart input 
* Json schema data format
* 8 independ strips or tied to matrix display

![alt text](doc/overview.png "Overview")
---

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

---

## JSON Schema data format

###Strip Selector  
{"strip":<NUMEBR_OF_STRIP>} NUMBER_OF_STRIP = Integer number from 1 to 8  


### Setup independent strips 

examples:

{"strip": 1, "update-mode":"instant"}  // defaults pixels=10, animation mode=off, interval=100ms,  
{"strip": 2, "update-mode":"instant", "pixels":144}  
{"strip": 3, "update-mode":"instant", "pixels":100}  
{"strip": 3, "update-mode":"instant", "pixels":55}  // redefine pixels on strip 3  

{"strip": 1, "pattern": [100, 240, 200],  "repeat": 1}  // default first=0  
{"strip": 1, "pattern-add": [100, 240, 200],  "first":20, "repeat": 1}  
{"strip": 1, "pattern-add": [100, 240, 200],  "repeat": 2, "first": 40}  

{"strip": 1, "update-mode":"triggered", "pixels":100, "pattern-add": [100, 240, 200],  "repeat": 1, "interval":100, "animation-mode":"rotate-right"}  

{"strip": 2, "update-mode":"instant", "pixels":100, "repeat": 0, "interval":100, "animation-mode":"rotate-right"}  

{"strip": 1, "pattern": [100, 240, 200]}  
{"strip": 1, "pattern-add": [100, 240, 200], "first":20}  

### Setup matrix display strips
{"strip": 1, "update-mode":"triggered","pixels":144}   
{"strip": 2, "update-mode":"triggered","pixels":144}   
{"strip": 3, "update-mode":"triggered","pixels":144}   
{"strip": 4, "update-mode":"triggered","pixels":144}   
{"strip": 5, "update-mode":"triggered","pixels":144}   
{"strip": 6, "update-mode":"triggered","pixels":144}   
{"strip": 7, "update-mode":"triggered","pixels":144}   
{"strip": 8, "update-mode":"triggered","pixels":144}   

Display with 8 rows and 144 coloumns, so we have 8 * 144 = 1152 Pixels managed  



{"animation":0}

{"animation":1}

{"update-trigger":1}

### 





