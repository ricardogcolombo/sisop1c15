#!/usr/bin/python
#-*- coding: utf8 -*-

PORT = 5481
HOST = 'localhost'

import BaseHTTPServer
import threading
import socket
import random
import urllib
import sys
import os.path

mutex = threading.Lock()
clientes = {}

folder = os.path.dirname(sys.argv[0])

class HandlerSO(BaseHTTPServer.BaseHTTPRequestHandler):
    def do_GET(self):
        if self.path == '/':
            self.do_HTML()
        elif self.path.startswith('/static/'):
            self.do_static()
        elif self.path == '/comandos/new/':
            self.do_comandos_setup()
        elif self.path.startswith('/comandos/delete/'):
            self.do_comandos_teardown()
        elif self.path.startswith('/comandos/'):
            self.do_comandos_execute()
        else:
            self.give_error(404)

    def give_error(self, code, message='¡Despreciable equívoco!'):
        self.send_response(code)
        self.send_header('Content-type','text/html')
        self.send_header('Content-encoding', 'utf8')
        self.end_headers()
        print "Error: %s" % message
        self.wfile.write(message)

    def do_HTML(self):
        self.send_response(200)
        self.send_header('Content-type','text/html')
        self.send_header('Content-encoding', 'utf8')
        self.end_headers()
        self.wfile.write(open(folder + '/static/cliente.html').read())

    def do_static(self):
        try:
            ext = self.path.split('.')[1]
            fname = self.path.split('/')[2]
            f = open(folder + '/static/' + fname)
        except Exception, e:
            give_error(404, 'No se pudo abrir %s' % self.path)

        self.send_response(200)
        if ext in ('jpg', 'png', 'gif'):
            self.send_header('Content-type','image/%s' % ext)
        elif ext in ('js'):
            self.send_header('Content-type','text/javascript')
        else:
            self.send_header('Content-type','text/%s' % ext)
        self.end_headers()
        self.wfile.write(f.read())

    def do_comandos_setup(self):
        sock = socket.socket()
        try:
            sock.connect((HOST, PORT))
        except Exception, e:
            self.send_response(200)
            self.send_header('Content-type', 'text/html')
            self.end_headers()
            self.wfile.write('ERROR_SOCKET')
            return
        mutex.acquire()
        i = int(str(random.random())[2:])
        if len(clientes) != 0:
            while i in clientes:
                i = int(str(random.random())[2:])
        clientes[i] = sock
        mutex.release()

        self.send_response(200)
        self.send_header('Content-type','text/html')
        self.end_headers()
        self.wfile.write(i)

    def do_comandos_teardown(self):
        try:
            i = int(self.path.split('/')[3])
        except Exception, e:
            self.give_error(500, str(e))

        mutex.acquire()
        try:
            sock = clientes[i]
        except Exception, e:
            self.give_error(500, str(e))
        sock.shutdown(socket.SHUT_RDWR)
        sock.close()
        del clientes[i]
        mutex.release()

        self.send_response(200)
        self.send_header('Content-type','text/html')
        self.end_headers()
        self.wfile.write("OK")

    def do_comandos_execute(self):
        try:
            i = int(self.path.split('/')[2])
        except Exception, e:
            self.give_error(500, str(e))

        mutex.acquire()
        try:
            sock = clientes[i]
        except Exception, e:
            self.give_error(500, str(e))
        mutex.release()


        comando = self.path.split('/')[3]
        try:
            sock.sendall(urllib.unquote(comando))
            r = sock.recv(1000000)
        except:
            r = "LOST_CONNECTION"
            mutex.acquire()
            del(clientes[i])
            mutex.release()

        self.send_response(200)
        self.send_header('Content-type','text/html')
        self.end_headers()
        self.wfile.write(r)


def run(server_class=BaseHTTPServer.HTTPServer,
    handler_class=HandlerSO, server_address = ('',5482)):
    httpd = server_class(server_address, handler_class)
    print "Escuchando en http://localhost:5482/"
    httpd.serve_forever()

if __name__ == '__main__':
    run()
