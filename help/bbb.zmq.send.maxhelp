{
    "patcher": {
        "fileversion": 1,
        "appversion": {
            "major": 9, "minor": 1, "revision": 3,
            "architecture": "x64", "modernui": 1
        },
        "classnamespace": "box",
        "rect": [50, 80, 750, 530],
        "bglocked": 1,
        "boxes": [
            {"box": {
                "id": "title", "maxclass": "comment",
                "text": "bbb.zmq.send",
                "numinlets": 1, "numoutlets": 0,
                "patching_rect": [30, 20, 200, 24],
                "fontsize": 18, "fontname": "Arial", "fontface": 1
            }},
            {"box": {
                "id": "desc", "maxclass": "comment",
                "text": "Send ZMQ messages from Max. Background thread ensures zero blocking.\nAtoms are type-encoded: int→int64, float→double, symbol→UTF-8 bytes.",
                "numinlets": 1, "numoutlets": 0,
                "patching_rect": [30, 50, 550, 40],
                "fontsize": 12, "fontname": "Arial", "linecount": 2
            }},

            {"box": {"id": "sec-single", "maxclass": "comment", "text": "--- Single Frame (send) ---", "numinlets": 1, "numoutlets": 0, "patching_rect": [30, 100, 200, 20], "fontsize": 12, "fontname": "Arial", "fontface": 1}},
            {"box": {
                "id": "msg-start", "maxclass": "message", "text": "start",
                "numinlets": 1, "numoutlets": 1, "outlettype": [""],
                "patching_rect": [30, 125, 50, 22]
            }},
            {"box": {
                "id": "msg-stop", "maxclass": "message", "text": "stop",
                "numinlets": 1, "numoutlets": 1, "outlettype": [""],
                "patching_rect": [90, 125, 45, 22]
            }},
            {"box": {
                "id": "msg-send", "maxclass": "message", "text": "send sensor 42 3.14",
                "numinlets": 1, "numoutlets": 1, "outlettype": [""],
                "patching_rect": [30, 155, 140, 22]
            }},
            {"box": {
                "id": "c-send", "maxclass": "comment",
                "text": "Encodes all atoms into one frame and sends immediately",
                "numinlets": 1, "numoutlets": 0,
                "patching_rect": [180, 155, 350, 20], "fontsize": 11, "fontname": "Arial"
            }},

            {"box": {"id": "sec-multi", "maxclass": "comment", "text": "--- Multipart (frame + flush) ---", "numinlets": 1, "numoutlets": 0, "patching_rect": [30, 195, 250, 20], "fontsize": 12, "fontname": "Arial", "fontface": 1}},
            {"box": {
                "id": "msg-frame1", "maxclass": "message", "text": "frame /sensor",
                "numinlets": 1, "numoutlets": 1, "outlettype": [""],
                "patching_rect": [30, 220, 90, 22]
            }},
            {"box": {
                "id": "msg-frame2", "maxclass": "message", "text": "frame_bytes 1 0 0 0 42",
                "numinlets": 1, "numoutlets": 1, "outlettype": [""],
                "patching_rect": [30, 250, 160, 22]
            }},
            {"box": {
                "id": "c-fb", "maxclass": "comment",
                "text": "frame_bytes: raw bytes (each int 0–255)",
                "numinlets": 1, "numoutlets": 0,
                "patching_rect": [200, 250, 280, 20], "fontsize": 11, "fontname": "Arial"
            }},
            {"box": {
                "id": "msg-flush", "maxclass": "message", "text": "flush",
                "numinlets": 1, "numoutlets": 1, "outlettype": [""],
                "patching_rect": [30, 280, 50, 22]
            }},
            {"box": {
                "id": "c-flush", "maxclass": "comment",
                "text": "Sends all buffered frames as one multipart message",
                "numinlets": 1, "numoutlets": 0,
                "patching_rect": [90, 280, 320, 20], "fontsize": 11, "fontname": "Arial"
            }},

            {"box": {
                "id": "send", "maxclass": "newobj", "text": "bbb.zmq.send",
                "numinlets": 1, "numoutlets": 0,
                "patching_rect": [30, 320, 100, 22]
            }},

            {"box": {"id": "sec-attr", "maxclass": "comment", "text": "--- Attributes ---", "numinlets": 1, "numoutlets": 0, "patching_rect": [200, 305, 130, 20], "fontsize": 12, "fontname": "Arial", "fontface": 1}},
            {"box": {
                "id": "c-endpoint", "maxclass": "comment",
                "text": "@endpoint tcp://*:5556  ZMQ endpoint",
                "numinlets": 1, "numoutlets": 0,
                "patching_rect": [200, 325, 280, 20], "fontsize": 11, "fontname": "Arial"
            }},
            {"box": {
                "id": "c-type", "maxclass": "comment",
                "text": "@type pub  Socket type: pub, push, pair",
                "numinlets": 1, "numoutlets": 0,
                "patching_rect": [200, 345, 280, 20], "fontsize": 11, "fontname": "Arial"
            }},
            {"box": {
                "id": "c-bind", "maxclass": "comment",
                "text": "@bind 1  1 = bind (server), 0 = connect (client)",
                "numinlets": 1, "numoutlets": 0,
                "patching_rect": [200, 365, 310, 20], "fontsize": 11, "fontname": "Arial"
            }},
            {"box": {
                "id": "c-hwm", "maxclass": "comment",
                "text": "@hwm 1000  Send high water mark",
                "numinlets": 1, "numoutlets": 0,
                "patching_rect": [200, 385, 250, 20], "fontsize": 11, "fontname": "Arial"
            }},
            {"box": {
                "id": "c-endian", "maxclass": "comment",
                "text": "@endian big  Byte order: big (default) or little",
                "numinlets": 1, "numoutlets": 0,
                "patching_rect": [200, 405, 300, 20], "fontsize": 11, "fontname": "Arial"
            }},

            {"box": {"id": "sec-io", "maxclass": "comment", "text": "--- Inlet ---", "numinlets": 1, "numoutlets": 0, "patching_rect": [30, 360, 100, 20], "fontsize": 12, "fontname": "Arial", "fontface": 1}},
            {"box": {
                "id": "c-inlet", "maxclass": "comment",
                "text": "inlet 0: start, stop, send, frame, frame_bytes, flush\nAlso accepts jit_matrix <name> to send matrix data.",
                "numinlets": 1, "numoutlets": 0,
                "patching_rect": [30, 380, 160, 34], "fontsize": 11, "fontname": "Arial", "linecount": 2
            }},

            {"box": {"id": "sec-encoding", "maxclass": "comment", "text": "--- Type Encoding ---", "numinlets": 1, "numoutlets": 0, "patching_rect": [30, 435, 170, 20], "fontsize": 12, "fontname": "Arial", "fontface": 1}},
            {"box": {
                "id": "c-enc", "maxclass": "comment",
                "text": "int/long → int64 (8 bytes)  float → double IEEE754 (8 bytes)\nsymbol → raw UTF-8 bytes (variable length)",
                "numinlets": 1, "numoutlets": 0,
                "patching_rect": [30, 455, 400, 34], "fontsize": 11, "fontname": "Arial", "linecount": 2
            }}
        ],
        "lines": [
            {"patchline": {"source": ["msg-start", 0], "destination": ["send", 0]}},
            {"patchline": {"source": ["msg-stop", 0], "destination": ["send", 0]}},
            {"patchline": {"source": ["msg-send", 0], "destination": ["send", 0]}},
            {"patchline": {"source": ["msg-frame1", 0], "destination": ["send", 0]}},
            {"patchline": {"source": ["msg-frame2", 0], "destination": ["send", 0]}},
            {"patchline": {"source": ["msg-flush", 0], "destination": ["send", 0]}}
        ]
    }
}
