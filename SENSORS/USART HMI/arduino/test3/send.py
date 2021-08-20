import serial

com = serial.Serial()
com.port = "COM9"
com.baudrate = 9600
com.parity = 'N'
com.bytesize=8
#com.timeout = 1
com.setDTR(False)
com.open()
arr = ""
com.write(b"t0.txt=\"Hello\"")
packet = bytearray()
packet.append(0xff)
packet.append(0xff)
packet.append(0xff)
com.write(packet)

while True:
    line = com.readline()
    print(str(line))