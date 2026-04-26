{
    "patcher": {
        "fileversion": 1,
        "appversion": {
            "major": 9, "minor": 1, "revision": 3,
            "architecture": "x64", "modernui": 1
        },
        "classnamespace": "box",
        "rect": [50, 80, 620, 480],
        "bglocked": 1,
        "boxes": [
            {"box": {
                "id": "title", "maxclass": "comment",
                "text": "bbb.zmq.recv",
                "numinlets": 1, "numoutlets": 0,
                "patching_rect": [30, 20, 200, 24],
                "fontsize": 18, "fontname": "Arial", "fontface": 1
            }},
            {"box": {
                "id": "desc", "maxclass": "comment",
                "text": "Receive ZMQ multipart messages in a background thread.\nOutputs packet <view_id> for each received message.\nFrames are stored internally; only a lightweight handle travels through Max.",
                "numinlets": 1, "numoutlets": 0,
                "patching_rect": [30, 50, 550, 40],
                "fontsize": 12, "fontname": "Arial", "linecount": 3
            }},

            {"box": {"id": "sec-msg", "maxclass": "comment", "text": "--- Messages ---", "numinlets": 1, "numoutlets": 0, "patching_rect": [30, 100, 120, 20], "fontsize": 12, "fontname": "Arial", "fontface": 1}},
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
                "id": "msg-bang", "maxclass": "message", "text": "bang",
                "numinlets": 1, "numoutlets": 1, "outlettype": [""],
                "patching_rect": [145, 125, 45, 22]
            }},
            {"box": {
                "id": "c-bang", "maxclass": "comment", "text": "bang = alias for start",
                "numinlets": 1, "numoutlets": 0,
                "patching_rect": [200, 125, 180, 20], "fontsize": 11, "fontname": "Arial"
            }},

            {"box": {
                "id": "recv", "maxclass": "newobj", "text": "bbb.zmq.recv",
                "numinlets": 1, "numoutlets": 1,
                "patching_rect": [30, 160, 100, 22],
                "outlettype": [""]
            }},
            {"box": {
                "id": "pr-out", "maxclass": "newobj", "text": "print recv",
                "numinlets": 1, "numoutlets": 0,
                "patching_rect": [30, 195, 80, 22]
            }},

            {"box": {"id": "sec-attr", "maxclass": "comment", "text": "--- Attributes ---", "numinlets": 1, "numoutlets": 0, "patching_rect": [250, 145, 130, 20], "fontsize": 12, "fontname": "Arial", "fontface": 1}},
            {"box": {
                "id": "c-endpoint", "maxclass": "comment",
                "text": "@endpoint tcp://*:5555  ZMQ endpoint",
                "numinlets": 1, "numoutlets": 0,
                "patching_rect": [250, 165, 280, 20], "fontsize": 11, "fontname": "Arial"
            }},
            {"box": {
                "id": "c-type", "maxclass": "comment",
                "text": "@type sub  Socket type: sub, pull, rep, router, pair",
                "numinlets": 1, "numoutlets": 0,
                "patching_rect": [250, 185, 340, 20], "fontsize": 11, "fontname": "Arial"
            }},
            {"box": {
                "id": "c-bind", "maxclass": "comment",
                "text": "@bind 1  1 = bind (server), 0 = connect (client)",
                "numinlets": 1, "numoutlets": 0,
                "patching_rect": [250, 205, 310, 20], "fontsize": 11, "fontname": "Arial"
            }},
            {"box": {
                "id": "c-sub", "maxclass": "comment",
                "text": "@subscribe \"\"  Topic filter (sub sockets only)",
                "numinlets": 1, "numoutlets": 0,
                "patching_rect": [250, 225, 300, 20], "fontsize": 11, "fontname": "Arial"
            }},
            {"box": {
                "id": "c-hwm", "maxclass": "comment",
                "text": "@hwm 1000  Receive high water mark",
                "numinlets": 1, "numoutlets": 0,
                "patching_rect": [250, 245, 250, 20], "fontsize": 11, "fontname": "Arial"
            }},

            {"box": {"id": "sec-io", "maxclass": "comment", "text": "--- Inlet / Outlet ---", "numinlets": 1, "numoutlets": 0, "patching_rect": [30, 230, 160, 20], "fontsize": 12, "fontname": "Arial", "fontface": 1}},
            {"box": {
                "id": "c-inlet", "maxclass": "comment",
                "text": "inlet 0: messages (start, stop, bang)",
                "numinlets": 1, "numoutlets": 0,
                "patching_rect": [30, 250, 250, 20], "fontsize": 11, "fontname": "Arial"
            }},
            {"box": {
                "id": "c-outlet", "maxclass": "comment",
                "text": "outlet 0: packet <view_id> — handle to received frames",
                "numinlets": 1, "numoutlets": 0,
                "patching_rect": [30, 270, 350, 20], "fontsize": 11, "fontname": "Arial"
            }},

            {"box": {"id": "sec-workflow", "maxclass": "comment", "text": "--- Typical Workflow ---", "numinlets": 1, "numoutlets": 0, "patching_rect": [30, 310, 180, 20], "fontsize": 12, "fontname": "Arial", "fontface": 1}},
            {"box": {
                "id": "c-wf", "maxclass": "comment",
                "text": "recv → route → parse → Max messages\nrecv → peek (debug)",
                "numinlets": 1, "numoutlets": 0,
                "patching_rect": [30, 330, 300, 34], "fontsize": 11, "fontname": "Arial", "linecount": 2
            }},
            {"box": {
                "id": "c-self", "maxclass": "comment",
                "text": "Loopback test: one side binds (bind 1), other connects (bind 0) to same port.",
                "numinlets": 1, "numoutlets": 0,
                "patching_rect": [30, 370, 500, 20], "fontsize": 11, "fontname": "Arial"
            }}
        ],
        "lines": [
            {"patchline": {"source": ["msg-start", 0], "destination": ["recv", 0]}},
            {"patchline": {"source": ["msg-stop", 0], "destination": ["recv", 0]}},
            {"patchline": {"source": ["msg-bang", 0], "destination": ["recv", 0]}},
            {"patchline": {"source": ["recv", 0], "destination": ["pr-out", 0]}}
        ]
    }
}
