# Cloud Reporting Thermostat
# By Frank Columbus
# December 3, 2016
# This program implements a simple cloud reporting thermostat that sends real data from a
# Grove temperature module and displays the reading on a Grove LCD display, as well as,
# sending it to the Intel IoT Analytics Dashboard.
# The temperature setting is done using the Grove Rotary Potentiometer.
# A Grove Relay is used to activate a device that can heat or cool.

import subprocess
import time
import pyupm_grove as grove
import pyupm_i2clcd as lcd

# Initialize Jhd1313m1 at 0x3E (LCD_ADDRESS) and 0x62 (RGB_ADDRESS)
myLcd = lcd.Jhd1313m1(0, 0x3E, 0x62)

# Create the relay switch object using GPIO pin 8
relay = grove.GroveRelay(8)

def main():
  color_control = 0
  # Voltage to drive the display causes a shift in the thermistor reference.
  calibration_constant = 2
  print

  while True:
    
    # Create the temperature sensor object using AIO pin 0
    temp = grove.GroveTemp(0)
    temp = temp.value()    
    temp += calibration_constant 
    
    # Use Intel's iotkit-admin tool to send the temperature
    print "Sending temperature", temp, "degree Celsius..."
    
    # Change the color to indicate a new sample interval.
    if color_control % 2 == 0:
    	# RGB Cyan
        myLcd.setColor(0, 255, 255)
    else:
        # RGB Yellow
        myLcd.setColor(255, 255, 0)

    color_control += 1
    myLcd.setCursor(0,0)
    myLcd.write('Sending temp   ')
    myLcd.setCursor(1,1)
    myLcd.write('Sampled: '+  str(temp) + ' C')

    output = subprocess.check_output (["/usr/bin/iotkit-admin", "observation", "temperature", str(temp)])
    
    print output
 
    # Wait for one minute
    print "Waiting for one minute. Press CTRL+C to stop..."
    time.sleep(5)
    # Sample temperature setting every 1 seconds
    for count in range(54):
         setting = rotary()
         print "Temperature set for: ", setting, " Degrees Celsius"
         if setting < temp:
            relay.off()
            print relay.name(), 'is off'
	 else:
	    relay.on()
       	    print relay.name(), 'is on'

    # Delete the temperature sensor object
    del temp
  return 0;

def rotary():

    "This function reads the rotary potentiometer to set thermostat temperature."

    temp_set = 0
    abs = 0
    # New knob on AIO pin 1
    knob = grove.GroveRotary(1)

    # Read values
    abs = knob.abs_value()

    # Scaling 0 => 10 degrees C
    # Scaling 1023 => 50 degrees C   
    temp_set = (1023 -  abs) / 1023 * 40 + 10
    temp_set = int(temp_set)
#    print 'int(abs)= ', int(abs)
#    print 'temp_set= ', temp_set

    myLcd.setCursor(0,0)
    myLcd.write('Set temp: ' + str(temp_set) + ' C  ' )

    # Sleep for 1 sec
    time.sleep(1)
    return temp_set; 

main()
