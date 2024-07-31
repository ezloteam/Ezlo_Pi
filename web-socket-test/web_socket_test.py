import io
import ssl
import json
import uuid
import asyncio
import logging
import websockets
from color_logs import *

_logger = logging.getLogger(__name__)
_logger.setLevel(logging.DEBUG)

ch = logging.StreamHandler()
ch.setLevel(logging.DEBUG)
ch.setFormatter(CustomFormatter())
_logger.addHandler(ch)

# # Save certificates to files
# with open("cert.pem", "w") as cert_file:
#     cert_file.write(SSL_CERT)

# with open("key.pem", "w") as key_file:
#     key_file.write(SSL_KEY)

# with open("ca.pem", "w") as ca_file:
#     ca_file.write(CA_CERT)

# Verify the contents of the files
# with open("cert.pem", "r") as cert_file:
#     # print(cert_file.read())
#     pass

# with open("key.pem", "r") as key_file:
#     # print(key_file.read())
#     pass

# with open("ca.pem", "r") as ca_file:
#     # print(ca_file.read())
#     pass

# Device details
DEVICE_INFO = {
    "serial": "105200134",
    "firmware": "4.1.3",
    "timeOffset": 20700,
    "media": "radio",
    "hubType": "32.1",
    "controller_uuid": "623d3d65-0795-47df-b954-6e5f3baf13fc",
    "mac_address": "C8:F0:9E:AC:A1:18",
    "maxFrameSize": 20480,
}

# Responses for known RPC methods
RESPONSES = {
    "hub.data.list": {
        "error": {
            "code": -32602,
            "data": "rpc.method.notfound",
            "message": "Unknown method",
        },
        "result": {},
        "method": "hub.data.list",
        "msg_id": 1,
    },
    "hub.network.get": {
        "result": {
            "interfaces": [
                {
                    "_id": "wlan0",
                    "enabled": "auto",
                    "hwaddr": "c8:f0:9e:ac:a1:18",
                    "internetAvailable": True,
                    "ipv4": {
                        "dns": ["192.168.1.254"],
                        "ip": "192.168.1.101",
                        "mask": "255.255.255.0",
                        "gateway": "192.168.1.254",
                        "mode": "dhcp",
                    },
                    "priority": 0,
                    "network": "wan",
                    "status": "up",
                    "type": "wifi",
                    "wifi": {
                        "mode": "sta",
                        "network": {
                            "bssid": "04:75:f9:1d:15:35",
                            "encryption": "psk2",
                            "key": "NDS_0ffice",
                            "ssid": "nepaldigisys_2",
                        },
                        "channel": "10",
                        "region": "00",
                    },
                }
            ]
        },
        "error": None,
        "method": "hub.network.get",
        "msg_id": 2,
    },
    "hub.devices.list": {
        "result": {
            "devices": [
                {
                    "_id": "10676000",
                    "deviceTypeId": "ezlopi",
                    "parentDeviceId": "",
                    "category": "switch",
                    "subcategory": "relay",
                    "gatewayId": "457a5069",
                    "batteryPowered": False,
                    "name": "switch_temp",
                    "type": "switch.inwall",
                    "reachable": True,
                    "persistent": True,
                    "serviceNotification": False,
                    "armed": False,
                    "roomId": "",
                    "security": "no",
                    "ready": True,
                    "status": "idle",
                },
                {
                    "_id": "10676001",
                    "deviceTypeId": "ezlopi",
                    "parentDeviceId": "",
                    "category": "dimmable_light",
                    "subcategory": "dimmable_bulb",
                    "gatewayId": "457a5069",
                    "batteryPowered": False,
                    "name": "Dimmer",
                    "type": "dimmer.outlet",
                    "reachable": True,
                    "persistent": True,
                    "serviceNotification": False,
                    "armed": False,
                    "roomId": "",
                    "security": "no",
                    "ready": True,
                    "status": "idle",
                },
                {
                    "_id": "10676002",
                    "deviceTypeId": "ezlopi",
                    "parentDeviceId": "",
                    "category": "temperature",
                    "subcategory": "",
                    "gatewayId": "457a5069",
                    "batteryPowered": False,
                    "name": "DHT22_temp_humi_temp",
                    "type": "sensor",
                    "reachable": True,
                    "persistent": True,
                    "serviceNotification": False,
                    "armed": False,
                    "roomId": "",
                    "security": "no",
                    "ready": True,
                    "status": "idle",
                },
                {
                    "_id": "10676003",
                    "deviceTypeId": "ezlopi",
                    "parentDeviceId": "10676002",
                    "category": "humidity",
                    "subcategory": "",
                    "gatewayId": "457a5069",
                    "batteryPowered": False,
                    "name": "DHT22_temp_humi_humi",
                    "type": "sensor",
                    "reachable": True,
                    "persistent": True,
                    "serviceNotification": False,
                    "armed": False,
                    "roomId": "",
                    "security": "no",
                    "ready": True,
                    "status": "idle",
                },
            ]
        },
        "error": None,
        "method": "hub.devices.list",
        "msg_id": 5,
    },
    "hub.items.list": {
        "result": {
            "items": [
                {
                    "_id": "20676000",
                    "deviceId": "10676000",
                    "hasGetter": True,
                    "hasSetter": True,
                    "name": "switch",
                    "show": True,
                    "valueType": "bool",
                    "value": False,
                    "valueFormatted": "false",
                    "status": "idle",
                },
                {
                    "_id": "20676001",
                    "deviceId": "10676001",
                    "hasGetter": True,
                    "hasSetter": True,
                    "name": "dimmer",
                    "show": True,
                    "valueType": "int",
                    "minValue": 0,
                    "maxValue": 100,
                    "value": 0,
                    "valueFormatted": "0",
                    "status": "idle",
                },
                {
                    "_id": "20676002",
                    "deviceId": "10676001",
                    "hasGetter": True,
                    "hasSetter": True,
                    "name": "dimmer_up",
                    "show": True,
                    "valueType": "int",
                    "value": 0,
                    "valueFormatted": "0",
                    "status": "idle",
                },
                {
                    "_id": "20676003",
                    "deviceId": "10676001",
                    "hasGetter": True,
                    "hasSetter": True,
                    "name": "dimmer_down",
                    "show": True,
                    "valueType": "int",
                    "value": 0,
                    "valueFormatted": "0",
                    "status": "idle",
                },
                {
                    "_id": "20676004",
                    "deviceId": "10676001",
                    "hasGetter": True,
                    "hasSetter": True,
                    "name": "dimmer_stop",
                    "show": True,
                    "valueType": "int",
                    "value": 0,
                    "valueFormatted": "0",
                    "status": "idle",
                },
                {
                    "_id": "20676005",
                    "deviceId": "10676001",
                    "hasGetter": True,
                    "hasSetter": True,
                    "name": "switch",
                    "show": True,
                    "valueType": "bool",
                    "value": False,
                    "valueFormatted": "false",
                    "status": "idle",
                },
                {
                    "_id": "20676006",
                    "deviceId": "10676002",
                    "hasGetter": True,
                    "hasSetter": False,
                    "name": "temp",
                    "show": True,
                    "valueType": "temperature",
                    "scale": "celsius",
                    "value": 0,
                    "valueFormatted": "0.00",
                    "status": "idle",
                },
                {
                    "_id": "20676007",
                    "deviceId": "10676003",
                    "hasGetter": True,
                    "hasSetter": False,
                    "name": "humidity",
                    "show": True,
                    "valueType": "humidity",
                    "scale": "percent",
                    "value": 0,
                    "valueFormatted": "0.00",
                    "status": "idle",
                },
            ]
        },
        "error": None,
        "method": "hub.items.list",
        "msg_id": 6,
    },
    "hub.scenes.list": {
        "error": {
            "code": -32602,
            "data": "rpc.method.notfound",
            "message": "Unknown method",
        },
        "result": {},
        "method": "hub.scenes.list",
        "msg_id": 7,
    },
    "hub.room.list": {
        "result": {},
        "error": None,
        "method": "hub.room.list",
        "msg_id": 8,
    },
    "hub.info.get": {
        "result": {
            "model": "ezlopi_generic",
            "architecture": "xtensa-esp32s2",
            "chipId": "esp32",
            "serial": 105200113,
            "uuid": "623d3d65-0795-47df-b954-6e5f3baf13fc",
            "offlineAnonymousAccess": True,
            "offlineInsecureAccess": True,
            "location": {
                "latitude": 0,
                "longitude": 0,
                "timezone": "GMT0",
                "state": "default",
            },
            "build": {
                "time": "2024-07-10T14:42:27+0000",
                "builder": "lomas.subedi@ezlo.com",
                "branch": "development",
                "commit": "c2fbb27a518900e391a060655358b9f06429b0c0",
            },
            "battery": {
                "stateOfCharge": 0,
                "remainingTime": 0,
                "health": 0,
                "status": "",
            },
            "localtime": "2024-07-18T09:10:03+0000",
            "uptime": "0d 0h 0m 53s",
        },
        "error": None,
        "method": "hub.info.get",
        "msg_id": 9,
    },
    "hub.features.list": {
        "error": {
            "code": -32602,
            "data": "rpc.method.notfound",
            "message": "Unknown method",
        },
        "result": {},
        "method": "hub.features.list",
        "msg_id": 10,
    },
    "hub.custom.nodes.list": {
        "error": {
            "code": -32602,
            "data": "rpc.method.notfound",
            "message": "Unknown method",
        },
        "result": {},
        "method": "hub.custom.nodes.list",
        "msg_id": 11,
    },
    "hub.modes.get": {
        "error": {
            "code": -32602,
            "data": "rpc.method.notfound",
            "message": "Unknown method",
        },
        "result": {},
        "method": "hub.modes.get",
        "msg_id": 16,
    },
}


# Function to handle incoming messages
async def handle_message(message):
    data = json.loads(message)
    _logger.info("WS Request :" + str(data))
    # print("WS Request :" + str(data))
    response = RESPONSES.get(data.get("method"), None)
    sender = data.get("sender", None)
    if response:
        response["id"] = data["id"]
        response["msg_id"] += 1  # Incrementing msg_id
        if sender:
            response["sender"] = {"conn_id": sender["conn_id"], "type": sender["type"]}
        JSON_response = json.dumps(response)
        _logger.debug("WS Response : " + str(JSON_response))
        # print("WS Response : " + str(JSON_response))
        return JSON_response

    return json.dumps(
        {
            "error": {
                "code": -32602,
                "data": "rpc.method.notfound",
                "message": "Unknown method",
            },
            "result": {},
            "id": data["id"],
            "method": data.get("method"),
        }
    )


# Function to register the device
async def register_device(websocket):
    register_message = {
        "id": str(uuid.uuid4()),
        "method": "register",
        "params": DEVICE_INFO,
    }
    await websocket.send(json.dumps(register_message))


# Main function to connect and handle communication
async def main():
    ssl_context = ssl.create_default_context(ssl.Purpose.SERVER_AUTH)
    ssl_context.load_cert_chain(certfile="cert.pem", keyfile="key.pem")
    ssl_context.load_verify_locations(cafile="ca.pem")

    uri = "wss://nma-server7-cloud.ezlo.com:443"
    async with websockets.connect(
        uri, ssl=ssl_context, ping_interval=10, ping_timeout=5
    ) as websocket:
        # Send the registration message
        registration_message = {
            "id": str(uuid.uuid4()),
            "method": "register",
            "params": {
                "serial": "105200113",
                "firmware": "4.1.3",
                "timeOffset": 20700,
                "media": "radio",
                "hubType": "32.1",
                "controller_uuid": "623d3d65-0795-47df-b954-6e5f3baf13fc",
                "mac_address": "C8:F0:9E:AC:A1:18",
                "maxFrameSize": 20480,
            },
        }
        await websocket.send(json.dumps(registration_message))

        while True:
            message = await websocket.recv()
            response = await handle_message(message)
            await websocket.send(response)


asyncio.run(main())
