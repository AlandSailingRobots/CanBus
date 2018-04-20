from Canbus import Canbus

canbus = Canbus()
canbus.Init(1000000)

while (True):
    pass
    if canbus.CheckForMessages():
        print("recived message")
        message = canbus.GetMessages()
        