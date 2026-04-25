{
    "patcher": {
        "fileversion": 1,
        "appversion": {
            "major": 9, "minor": 1, "revision": 3,
            "architecture": "x64", "modernui": 1
        },
        "classnamespace": "box",
        "rect": [ 50.0, 50.0, 1100.0, 580.0 ],
        "boxes": [
            { "box": { "fontface": 1, "fontname": "Arial", "fontsize": 16.0, "id": "title", "maxclass": "comment", "numinlets": 1, "numoutlets": 0, "patching_rect": [ 20.0, 15.0, 250.0, 24.0 ], "text": "bbb.zmq.* Integration Test" } },

            { "box": { "fontname": "Arial", "fontsize": 12.0, "id": "sec-recv", "maxclass": "comment", "numinlets": 1, "numoutlets": 0, "patching_rect": [ 20.0, 50.0, 120.0, 20.0 ], "text": "=== Receiver ===" } },
            { "box": { "id": "msg-start", "maxclass": "message", "numinlets": 2, "numoutlets": 1, "outlettype": [ "" ], "patching_rect": [ 20.0, 75.0, 50.0, 22.0 ], "text": "start" } },
            { "box": { "id": "msg-stop", "maxclass": "message", "numinlets": 2, "numoutlets": 1, "outlettype": [ "" ], "patching_rect": [ 80.0, 75.0, 45.0, 22.0 ], "text": "stop" } },
            { "box": { "id": "recv", "maxclass": "newobj", "numinlets": 1, "numoutlets": 1, "outlettype": [ "" ], "patching_rect": [ 20.0, 105.0, 260.0, 22.0 ], "text": "bbb.zmq.recv @type sub @endpoint tcp://localhost:5556 @bind 0" } },
            { "box": { "fontname": "Arial", "fontsize": 10.0, "id": "c-recv", "maxclass": "comment", "numinlets": 1, "numoutlets": 0, "patching_rect": [ 290.0, 105.0, 200.0, 20.0 ], "text": "connect to sender on 5556" } },

            { "box": { "fontname": "Arial", "fontsize": 12.0, "id": "sec-route", "maxclass": "comment", "numinlets": 1, "numoutlets": 0, "patching_rect": [ 20.0, 145.0, 200.0, 20.0 ], "text": "=== Router (consumes key) ===" } },
            { "box": { "id": "route", "maxclass": "newobj", "numinlets": 1, "numoutlets": 3, "outlettype": [ "", "", "" ], "patching_rect": [ 20.0, 170.0, 200.0, 22.0 ], "text": "bbb.zmq.route sensor actuator" } },
            { "box": { "id": "pr-sensor", "maxclass": "newobj", "numinlets": 1, "numoutlets": 0, "patching_rect": [ 20.0, 200.0, 80.0, 22.0 ], "text": "print sensor" } },
            { "box": { "id": "pr-act", "maxclass": "newobj", "numinlets": 1, "numoutlets": 0, "patching_rect": [ 230.0, 200.0, 85.0, 22.0 ], "text": "print actuator" } },
            { "box": { "id": "pr-unmatch", "maxclass": "newobj", "numinlets": 1, "numoutlets": 0, "patching_rect": [ 340.0, 200.0, 100.0, 22.0 ], "text": "print unmatched" } },

            { "box": { "fontname": "Arial", "fontsize": 12.0, "id": "sec-parse", "maxclass": "comment", "numinlets": 1, "numoutlets": 0, "patching_rect": [ 20.0, 240.0, 120.0, 20.0 ], "text": "=== Parse ===" } },
            { "box": { "id": "parse", "maxclass": "newobj", "numinlets": 1, "numoutlets": 2, "outlettype": [ "", "" ], "patching_rect": [ 20.0, 265.0, 170.0, 22.0 ], "text": "bbb.zmq.parse test_schema" } },
            { "box": { "id": "pr-parse", "maxclass": "newobj", "numinlets": 1, "numoutlets": 0, "patching_rect": [ 20.0, 295.0, 60.0, 22.0 ], "text": "print data" } },
            { "box": { "id": "pr-perr", "maxclass": "newobj", "numinlets": 1, "numoutlets": 0, "patching_rect": [ 100.0, 295.0, 65.0, 22.0 ], "text": "print perr" } },

            { "box": { "fontname": "Arial", "fontsize": 12.0, "id": "sec-peek", "maxclass": "comment", "numinlets": 1, "numoutlets": 0, "patching_rect": [ 20.0, 335.0, 100.0, 20.0 ], "text": "=== Peek ===" } },
            { "box": { "id": "peek", "maxclass": "newobj", "numinlets": 1, "numoutlets": 1, "outlettype": [ "" ], "patching_rect": [ 20.0, 360.0, 152.0, 22.0 ], "text": "bbb.zmq.peek @verbose 2" } },
            { "box": { "id": "pr-peek", "maxclass": "newobj", "numinlets": 1, "numoutlets": 0, "patching_rect": [ 20.0, 390.0, 70.0, 22.0 ], "text": "print peek" } },

            { "box": { "fontname": "Arial", "fontsize": 12.0, "id": "sec-schema", "maxclass": "comment", "numinlets": 1, "numoutlets": 0, "patching_rect": [ 470.0, 50.0, 120.0, 20.0 ], "text": "=== Schema ===" } },
            { "box": { "id": "msg-read", "maxclass": "message", "numinlets": 2, "numoutlets": 1, "outlettype": [ "" ], "patching_rect": [ 470.0, 75.0, 200.0, 22.0 ], "text": "read schemas/test_schema.zmqdsl" } },
            { "box": { "id": "schema", "maxclass": "newobj", "numinlets": 1, "numoutlets": 2, "outlettype": [ "", "" ], "patching_rect": [ 470.0, 110.0, 340.0, 22.0 ], "text": "bbb.zmq.schema @name test_schema @autocompile 1" } },
            { "box": { "id": "pr-s-status", "maxclass": "newobj", "numinlets": 1, "numoutlets": 0, "patching_rect": [ 470.0, 140.0, 60.0, 22.0 ], "text": "print s-ok" } },
            { "box": { "id": "pr-s-err", "maxclass": "newobj", "numinlets": 1, "numoutlets": 0, "patching_rect": [ 560.0, 140.0, 65.0, 22.0 ], "text": "print s-err" } },
            { "box": { "id": "textedit", "linecount": 7, "maxclass": "textedit", "numinlets": 1, "numoutlets": 4, "outlettype": [ "", "int", "", "" ], "parameter_enable": 0, "patching_rect": [ 470.0, 170.0, 380.0, 100.0 ], "text": "schema test_schema {\nendian big;\nu8 id;\ni32 value;\nf32 temp;\nemit id value temp;\n}" } },
            { "box": { "id": "prepend-set", "maxclass": "newobj", "numinlets": 1, "numoutlets": 1, "outlettype": [ "" ], "patching_rect": [ 470.0, 280.0, 75.0, 22.0 ], "text": "prepend set" } },

            { "box": { "fontname": "Arial", "fontsize": 12.0, "id": "sec-send", "maxclass": "comment", "numinlets": 1, "numoutlets": 0, "patching_rect": [ 470.0, 320.0, 120.0, 20.0 ], "text": "=== Sender ===" } },
            { "box": { "id": "msg-snd-start", "maxclass": "message", "numinlets": 2, "numoutlets": 1, "outlettype": [ "" ], "patching_rect": [ 470.0, 345.0, 50.0, 22.0 ], "text": "start" } },
            { "box": { "id": "msg-snd-stop", "maxclass": "message", "numinlets": 2, "numoutlets": 1, "outlettype": [ "" ], "patching_rect": [ 530.0, 345.0, 45.0, 22.0 ], "text": "stop" } },
            { "box": { "fontname": "Arial", "fontsize": 10.0, "id": "c-s1", "maxclass": "comment", "numinlets": 1, "numoutlets": 0, "patching_rect": [ 590.0, 345.0, 260.0, 20.0 ], "text": "single frame: symbol + int64 + double" } },
            { "box": { "id": "msg-snd-simple", "maxclass": "message", "numinlets": 2, "numoutlets": 1, "outlettype": [ "" ], "patching_rect": [ 470.0, 380.0, 140.0, 22.0 ], "text": "send sensor 42 3.14" } },
            { "box": { "fontname": "Arial", "fontsize": 10.0, "id": "c-s2", "maxclass": "comment", "numinlets": 1, "numoutlets": 0, "patching_rect": [ 620.0, 380.0, 260.0, 20.0 ], "text": "multipart: frame topic, frame payload, flush" } },
            { "box": { "id": "msg-snd-mp", "maxclass": "message", "numinlets": 2, "numoutlets": 1, "outlettype": [ "" ], "patching_rect": [ 470.0, 415.0, 100.0, 22.0 ], "text": "sensor 42 3.14" } },
            { "box": { "id": "t-mp", "maxclass": "newobj", "numinlets": 1, "numoutlets": 3, "outlettype": [ "", "", "" ], "patching_rect": [ 470.0, 445.0, 42.0, 22.0 ], "text": "t l l l" } },
            { "box": { "id": "prepend-frame", "maxclass": "newobj", "numinlets": 1, "numoutlets": 1, "outlettype": [ "" ], "patching_rect": [ 470.0, 475.0, 80.0, 22.0 ], "text": "prepend frame" } },
            { "box": { "id": "msg-snd-flush", "maxclass": "message", "numinlets": 2, "numoutlets": 1, "outlettype": [ "" ], "patching_rect": [ 620.0, 475.0, 50.0, 22.0 ], "text": "flush" } },
            { "box": { "id": "sender", "maxclass": "newobj", "numinlets": 1, "numoutlets": 0, "patching_rect": [ 470.0, 510.0, 260.0, 22.0 ], "text": "bbb.zmq.send @endpoint tcp://*:5556" } },

            { "box": { "fontname": "Arial", "fontsize": 10.0, "id": "c-loopback", "maxclass": "comment", "numinlets": 1, "numoutlets": 0, "patching_rect": [ 20.0, 430.0, 400.0, 32.0 ], "linecount": 2, "text": "Self-test: sender binds on 5556, recv connects to localhost:5556.\nStart sender first, then recv. Send messages and see them loop back." } }
        ],
        "lines": [
            { "patchline": { "destination": [ "recv", 0 ], "source": [ "msg-start", 0 ] } },
            { "patchline": { "destination": [ "recv", 0 ], "source": [ "msg-stop", 0 ] } },
            { "patchline": { "destination": [ "route", 0 ], "order": 1, "source": [ "recv", 0 ] } },

            { "patchline": { "destination": [ "pr-sensor", 0 ], "order": 2, "source": [ "route", 0 ] } },
            { "patchline": { "destination": [ "parse", 0 ], "order": 1, "source": [ "route", 0 ] } },
            { "patchline": { "destination": [ "peek", 0 ], "order": 0, "source": [ "route", 0 ] } },
            { "patchline": { "destination": [ "pr-act", 0 ], "source": [ "route", 1 ] } },
            { "patchline": { "destination": [ "pr-unmatch", 0 ], "source": [ "route", 2 ] } },

            { "patchline": { "destination": [ "pr-parse", 0 ], "source": [ "parse", 0 ] } },
            { "patchline": { "destination": [ "pr-perr", 0 ], "source": [ "parse", 1 ] } },

            { "patchline": { "destination": [ "pr-peek", 0 ], "source": [ "peek", 0 ] } },

            { "patchline": { "destination": [ "schema", 0 ], "source": [ "msg-read", 0 ] } },
            { "patchline": { "destination": [ "pr-s-status", 0 ], "source": [ "schema", 0 ] } },
            { "patchline": { "destination": [ "pr-s-err", 0 ], "source": [ "schema", 1 ] } },
            { "patchline": { "destination": [ "prepend-set", 0 ], "source": [ "textedit", 0 ] } },
            { "patchline": { "destination": [ "schema", 0 ], "source": [ "prepend-set", 0 ] } },

            { "patchline": { "destination": [ "sender", 0 ], "source": [ "msg-snd-start", 0 ] } },
            { "patchline": { "destination": [ "sender", 0 ], "source": [ "msg-snd-stop", 0 ] } },
            { "patchline": { "destination": [ "sender", 0 ], "source": [ "msg-snd-simple", 0 ] } },
            { "patchline": { "destination": [ "t-mp", 0 ], "source": [ "msg-snd-mp", 0 ] } },
            { "patchline": { "destination": [ "prepend-frame", 0 ], "source": [ "t-mp", 0 ] } },
            { "patchline": { "destination": [ "sender", 0 ], "source": [ "prepend-frame", 0 ] } },
            { "patchline": { "destination": [ "sender", 0 ], "source": [ "msg-snd-flush", 0 ] } }
        ],
        "autosave": 0
    }
}
