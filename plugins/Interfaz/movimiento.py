import time
class mov:
    def __init__(self):
        import serial
        self.freedom=serial.Serial('/dev/ttyACM0', 9600)
        self.freedom.open()
        self.command="p" #Stop

    def startSerialSend_W(self):
        self.command="w"
        print "Sending %s" % self.command
        self.freedom.write(self.command+' \r\n')

    def stopSerialSend(self):
        self.command="p"
        print "Sending %s" % self.command
        self.freedom.write(self.command+' \r\n')
        
    def close(self):
        self.freedom.close()

a = mov()
a.startSerialSend_W()
time.sleep(1)
a.stopSerialSend()
a.close()