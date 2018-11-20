import cgi 
import RPi.GPIO as gpio 
from BaseHTTPServer import HTTPServer
from SimpleHTTPServer import SimpleHTTPRequestHandler

#Actuator define BCM GPIO Pin - is not wiringPi Pin number

FAN = 6
LED = 12

#FANON FANOFF LEDON LEDOFF SMODEON SMODEOFF
class Handler(SimpleHTTPRequestHandler):
    def do_POST(self):
        if self.path == '/rccar':
            form = cgi.FieldStorage(fp=self.rfile, 
                                    headers=self.headers,
                                    environ={'REQUEST_METHOD':'POST'})
            cmd = form['cmd'].value
            print cmd

            if cmd == "FANON":
				gpio.output(FAN, gpio.HIGH)
                                f = open("data/fanmode.txt", 'w')
                                f.write("1")
                                f.close()
            elif cmd == "FANOFF":
				gpio.output(FAN, gpio.LOW)
                                f = open("data/fanmode.txt", 'w')
                                f.write("0")
                                f.close()
            elif cmd == "LEDON":
				gpio.output(LED, gpio.HIGH)
                                f = open("data/ledmode.txt", 'w')
                                f.write("1")
                                f.close()
            elif cmd == "LEDOFF":
				gpio.output(LED, gpio.LOW)
                                f = open("data/ledmode.txt", 'w')
                                f.write("0")
                                f.close()
            elif cmd == "SMODEON":
                                f = open("data/smode.txt", 'w')
                                f.write("3")
                                f.close()
            elif cmd == "SLEEPMODEON":
                                f = open("data/smode.txt", 'w')
                                f.write("2")
                                f.close()
            elif cmd == "SMODEOFF":
                                f = open("data/smode.txt", 'w')
                                f.write("0")
                                f.close()
            elif cmd >= 24:
                                f = open("data/tempcut.txt", 'w')
                                f.write(cmd)
                                f.close()

            self.send_response(100)
            self.send_header('Content-type', 'text/html')

            return

        return self.do_GET() 

gpio.setwarnings(False)
gpio.setmode( gpio.BCM ) 

#Pin Output Setup
gpio.setup(FAN, gpio.OUT)
gpio.setup(LED, gpio.OUT)

#Pin Initialization
gpio.output(FAN, gpio.LOW)
gpio.output(LED, gpio.LOW)

server = HTTPServer(('', 8002), Handler).serve_forever()
