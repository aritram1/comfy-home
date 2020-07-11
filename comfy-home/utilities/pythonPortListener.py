from datetime import datetime

# initialize port and listen
import serial
serialport = serial.Serial(port='/dev/ttyUSB0', baudrate=9600)

#Record data in loop
def logIt():
    with open('../data/joystick.csv', 'a') as dataFile:
        sensorData = serialport.readline()
        now = datetime.now().strftime("%H:%M:%S")
        dataFile.write(now + ' ' + sensorData.decode())
        dataFile.close()

while True:
    logIt()

