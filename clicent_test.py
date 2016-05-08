import socket

address = ('127.0.0.1', 8001)
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect(address)

data = s.recv(512);
print(data)

s.send("hihi")


