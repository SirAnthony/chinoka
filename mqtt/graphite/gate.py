#!/usr/bin/env python
from config import BaseConfig as Config
import logging
import paho.mqtt.client as mqtt
import os, sys
import time
import socket
import json
import signal

# init config
config = Config()

class Client:
    methods = 'message connect disconnect subscribe log'.split(' ')
    def __init__(self, mapping):
        self.mapping = mapping
        self.__get_socket()
        self.conn = mqtt.Client(config.cid, clean_session=True)
        for method in Client.methods:
            self.conn.__setattr__('on_'+method, self.__getattribute__('_'+method));
        self.conn.will_set('clients/'+config.cid, payload="Bye!", qos=0, retain=False)

    def __get_socket(self):
        try:
            self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        except:
            logging.error('Can not create UDP socket')
            sys.exit(1)
        return self.sock

    def connect(self, host, port, timeout=60):
        self.conn.connect(host, int(port), timeout)

    def disconnect(self):
        '''Disconnect'''
        self.conn.publish('/clients/'+config.cid, "Offline")
        self.conn.disconnect()
        logging.info("Disconnected from broker")

    def loop(self):
        self.conn.loop_forever()

    def _log(self, client, level, buf):
        logging[level](buf)

    def _payload_n(self, msg, key):
        '''Number: obtain a float from the payload'''
        number = float(msg.payload)
        return [(key, number,)]

    def _payload_j(self, msg, key):
        '''JSON: try and load the JSON string from payload and use
           subkeys to pass to Carbon'''
        lines = []
        st = json.loads(msg.payload)
        for k in st:
            try:
               lines.append((key+'.'+k, float(st[k]), ))
            except ValueError:
                pass
        return lines

    def _msg_process(self, topic, msg, ts):
        data = self.mapping[topic]
        logging.debug('%s matches MAP(%s) => %s', msg.topic, topic, data)
        # Must we rename the received msg topic into a different
        # name for Carbon? In any case, replace MQTT slashes (/)
        # by Carbon periods (.)
        type = data['type']
        topic = data['topic']
        if topic is not None and '#' in topic:
            topic = topic.replace('#', msg.topic.replace('/', '.'))
        carbonkey = topic if topic else msg.topic
        carbonkey = carbonkey.replace('/', '.')
        logging.debug("CARBONKEY is [%s]", carbonkey)
        method = getattr(self, '_payload_'+type, None)
        if not callable(method):
            logging.info("Unknown mapping key [%s]", type)
            return False
        try:
            lines = method(msg, carbonkey)
        except:
            logging.info("Type %s of topic %s payload is incorrect [%s]",
                type, msg.topic, msg.payload)
            return False
        return '\n'.join(["%s %s %s" % (k, v, ts) for (k,v) in lines])+'\n'

    def _connect(self, mosq, userdata, flags, rc):
        logging.info("Connected to broker at %s as %s", config.mqtt.host, config.cid)
        self.conn.publish("/clients/%s" % config.cid, "Online")
        for topic in self.mapping:
            logging.debug("Subscribing to topic %s", topic)
            self.conn.subscribe(topic, 0)

    def _message(self, mosq, userdata, msg):
        now = int(time.time())
        messages = []
        for t in self.mapping:
            if not mqtt.topic_matches_sub(t, msg.topic):
                continue
            data = self.mapping[t]
            message = self._msg_process(t, msg, now)
            if message:
                messages.append(message)
        opt = (config.graphite.host, config.graphite.port,)
        for m in messages:
            logging.debug("Send '%s'", message)
            self.sock.sendto(str.encode(m), opt)

    def _subscribe(self, mosq, userdata, mid, granted_qos):
        pass

    def _disconnect(self, mosq, userdata, rc):
        if rc == 0:
            return logging.info("Clean disconnection")
        timeout = config.reconnect
        logging.info("Unexpected disconnect (rc %s); reconnecting in %s seconds",
            rc, timeout)
        time.sleep(int(timeout))

def main():
    logging.info("Starting %s", config.cid)
    client = Client(config.mapping)
    client.connect(config.mqtt.host, config.mqtt.port)
    def cleanup(signum, frame):
        client.disconnect();
        self.conn.publish('/clients/'+config.cid, "Offline")
        self.conn.disconnect()
        logging.info("Exiting on signal %d", signum)
        sys.exit(signum)
    signal.signal(signal.SIGTERM, cleanup)
    signal.signal(signal.SIGINT, cleanup)

    # Loop forever
    client.loop()

if __name__ == '__main__':
    main()

