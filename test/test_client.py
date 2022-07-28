import socket

client_sock = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
client_sock.connect(("192.168.238.128",7788));

msg = b" this is message from client."
client_sock.send(msg)

while True:
    data = client_sock.recv(1024)
    print(str(data))

