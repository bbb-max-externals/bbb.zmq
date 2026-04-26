{
    "patcher": {
        "fileversion": 1,
        "appversion": {
            "major": 9, "minor": 1, "revision": 3,
            "architecture": "x64", "modernui": 1
        },
        "classnamespace": "box",
        "rect": [50, 50, 1200, 620],
        "autosave": 0,
        "boxes": [
            {"box": {"fontface": 1, "fontname": "Arial", "fontsize": 16, "id": "title", "maxclass": "comment", "numinlets": 1, "numoutlets": 0, "patching_rect": [20, 15, 300, 24], "text": "bbb.zmq.* Integration Test"}},

            {"box": {"fontface": 1, "fontname": "Arial", "fontsize": 12, "id": "sec-recv", "maxclass": "comment", "numinlets": 1, "numoutlets": 0, "patching_rect": [20, 50, 120, 20], "text": "=== Receiver ==="}},
            {"box": {"id": "msg-start", "maxclass": "message", "numinlets": 2, "numoutlets": 1, "outlettype": [""], "patching_rect": [20, 75, 50, 22], "text": "start"}},
            {"box": {"id": "msg-stop", "maxclass": "message", "numinlets": 2, "numoutlets": 1, "outlettype": [""], "patching_rect": [80, 75, 45, 22], "text": "stop"}},
            {"box": {"id": "recv", "maxclass": "newobj", "numinlets": 1, "numoutlets": 1, "outlettype": [""], "patching_rect": [20, 105, 300, 22], "text": "bbb.zmq.recv @type sub @endpoint tcp://localhost:5556 @bind 0"}},
            {"box": {"fontname": "Arial", "fontsize": 10, "id": "c-recv", "maxclass": "comment", "numinlets": 1, "numoutlets": 0, "patching_rect": [330, 105, 200, 20], "text": "connect to sender on 5556"}},

            {"box": {"fontface": 1, "fontname": "Arial", "fontsize": 12, "id": "sec-route", "maxclass": "comment", "numinlets": 1, "numoutlets": 0, "patching_rect": [20, 145, 250, 20], "text": "=== Route (consumes key) ==="}},
            {"box": {"id": "route", "maxclass": "newobj", "numinlets": 1, "numoutlets": 3, "outlettype": ["", "", ""], "patching_rect": [20, 170, 200, 22], "text": "bbb.zmq.route sensor actuator"}},
            {"box": {"id": "pr-sensor", "maxclass": "newobj", "numinlets": 1, "numoutlets": 0, "patching_rect": [20, 200, 80, 22], "text": "print sensor"}},
            {"box": {"id": "pr-act", "maxclass": "newobj", "numinlets": 1, "numoutlets": 0, "patching_rect": [230, 200, 85, 22], "text": "print actuator"}},
            {"box": {"id": "pr-unmatch", "maxclass": "newobj", "numinlets": 1, "numoutlets": 0, "patching_rect": [340, 200, 100, 22], "text": "print unmatched"}},

            {"box": {"fontface": 1, "fontname": "Arial", "fontsize": 12, "id": "sec-parse", "maxclass": "comment", "numinlets": 1, "numoutlets": 0, "patching_rect": [20, 240, 120, 20], "text": "=== Parse ==="}},
            {"box": {"id": "parse", "maxclass": "newobj", "numinlets": 1, "numoutlets": 2, "outlettype": ["", ""], "patching_rect": [20, 265, 170, 22], "text": "bbb.zmq.parse test_schema"}},
            {"box": {"id": "pr-parse", "maxclass": "newobj", "numinlets": 1, "numoutlets": 0, "patching_rect": [20, 295, 60, 22], "text": "print data"}},
            {"box": {"id": "pr-perr", "maxclass": "newobj", "numinlets": 1, "numoutlets": 0, "patching_rect": [100, 295, 65, 22], "text": "print perr"}},

            {"box": {"fontface": 1, "fontname": "Arial", "fontsize": 12, "id": "sec-peek", "maxclass": "comment", "numinlets": 1, "numoutlets": 0, "patching_rect": [20, 335, 100, 20], "text": "=== Peek ==="}},
            {"box": {"id": "peek", "maxclass": "newobj", "numinlets": 1, "numoutlets": 1, "outlettype": [""], "patching_rect": [20, 360, 152, 22], "text": "bbb.zmq.peek @verbose 2"}},
            {"box": {"id": "pr-peek", "maxclass": "newobj", "numinlets": 1, "numoutlets": 0, "patching_rect": [20, 390, 70, 22], "text": "print peek"}},

            {"box": {"fontface": 1, "fontname": "Arial", "fontsize": 12, "id": "sec-rp", "maxclass": "comment", "numinlets": 1, "numoutlets": 0, "patching_rect": [20, 430, 250, 20], "text": "=== Routepass (keeps key) ==="}},
            {"box": {"id": "routepass", "maxclass": "newobj", "numinlets": 1, "numoutlets": 2, "outlettype": ["", ""], "patching_rect": [20, 455, 190, 22], "text": "bbb.zmq.routepass sensor"}},
            {"box": {"id": "pr-rp-match", "maxclass": "newobj", "numinlets": 1, "numoutlets": 0, "patching_rect": [20, 485, 100, 22], "text": "print rp-sensor"}},
            {"box": {"id": "pr-rp-unmatch", "maxclass": "newobj", "numinlets": 1, "numoutlets": 0, "patching_rect": [140, 485, 105, 22], "text": "print rp-unmatch"}},
            {"box": {"fontname": "Arial", "fontsize": 10, "id": "c-rp", "maxclass": "comment", "numinlets": 1, "numoutlets": 0, "patching_rect": [260, 455, 300, 34], "linecount": 2, "text": "routepass keeps the matched frame.\nUse parse @frame 1 to skip the routing key."}},

            {"box": {"fontface": 1, "fontname": "Arial", "fontsize": 12, "id": "sec-schema", "maxclass": "comment", "numinlets": 1, "numoutlets": 0, "patching_rect": [520, 50, 120, 20], "text": "=== Schema ==="}},
            {"box": {"id": "msg-read", "maxclass": "message", "numinlets": 2, "numoutlets": 1, "outlettype": [""], "patching_rect": [520, 75, 200, 22], "text": "read schemas/test_schema.zmqdsl"}},
            {"box": {"id": "schema", "maxclass": "newobj", "numinlets": 1, "numoutlets": 2, "outlettype": ["", ""], "patching_rect": [520, 110, 340, 22], "text": "bbb.zmq.schema @name test_schema @autocompile 1"}},
            {"box": {"id": "pr-s-status", "maxclass": "newobj", "numinlets": 1, "numoutlets": 0, "patching_rect": [520, 140, 60, 22], "text": "print s-ok"}},
            {"box": {"id": "pr-s-err", "maxclass": "newobj", "numinlets": 1, "numoutlets": 0, "patching_rect": [610, 140, 65, 22], "text": "print s-err"}},
            {"box": {"id": "textedit", "linecount": 7, "maxclass": "textedit", "numinlets": 1, "numoutlets": 4, "outlettype": ["", "int", "", ""], "parameter_enable": 0, "patching_rect": [520, 170, 380, 100], "text": "schema test_schema {\nendian big;\nu8 id;\ni32 value;\nf32 temp;\nemit id value temp;\n}"}},
            {"box": {"id": "prepend-set", "maxclass": "newobj", "numinlets": 1, "numoutlets": 1, "outlettype": [""], "patching_rect": [520, 280, 75, 22], "text": "prepend set"}},

            {"box": {"fontface": 1, "fontname": "Arial", "fontsize": 12, "id": "sec-send", "maxclass": "comment", "numinlets": 1, "numoutlets": 0, "patching_rect": [520, 320, 120, 20], "text": "=== Sender ==="}},
            {"box": {"id": "msg-snd-start", "maxclass": "message", "numinlets": 2, "numoutlets": 1, "outlettype": [""], "patching_rect": [520, 345, 50, 22], "text": "start"}},
            {"box": {"id": "msg-snd-stop", "maxclass": "message", "numinlets": 2, "numoutlets": 1, "outlettype": [""], "patching_rect": [580, 345, 45, 22], "text": "stop"}},
            {"box": {"fontname": "Arial", "fontsize": 10, "id": "c-s1", "maxclass": "comment", "numinlets": 1, "numoutlets": 0, "patching_rect": [640, 345, 260, 20], "text": "single frame: symbol + int64 + double"}},
            {"box": {"id": "msg-snd-simple", "maxclass": "message", "numinlets": 2, "numoutlets": 1, "outlettype": [""], "patching_rect": [520, 380, 140, 22], "text": "send sensor 42 3.14"}},

            {"box": {"fontface": 1, "fontname": "Arial", "fontsize": 12, "id": "sec-mp", "maxclass": "comment", "numinlets": 1, "numoutlets": 0, "patching_rect": [520, 420, 250, 20], "text": "=== Multipart (click button) ==="}},
            {"box": {"fontname": "Arial", "fontsize": 10, "id": "c-mp", "maxclass": "comment", "numinlets": 1, "numoutlets": 0, "patching_rect": [620, 440, 300, 34], "linecount": 2, "text": "t b b b fires right-to-left:\nframe sensor → frame 42 3.14 → flush"}},
            {"box": {"id": "btn-mp", "maxclass": "button", "numinlets": 1, "numoutlets": 1, "outlettype": ["bang"], "patching_rect": [520, 440, 20, 20]}},
            {"box": {"id": "t-mp", "maxclass": "newobj", "numinlets": 1, "numoutlets": 3, "outlettype": ["bang", "bang", "bang"], "patching_rect": [520, 470, 42, 22], "text": "t b b b"}},
            {"box": {"id": "msg-frame1", "maxclass": "message", "numinlets": 2, "numoutlets": 1, "outlettype": [""], "patching_rect": [520, 500, 90, 22], "text": "frame sensor"}},
            {"box": {"id": "msg-frame2", "maxclass": "message", "numinlets": 2, "numoutlets": 1, "outlettype": [""], "patching_rect": [620, 500, 110, 22], "text": "frame 42 3.14"}},
            {"box": {"id": "msg-flush", "maxclass": "message", "numinlets": 2, "numoutlets": 1, "outlettype": [""], "patching_rect": [740, 500, 50, 22], "text": "flush"}},
            {"box": {"id": "sender", "maxclass": "newobj", "numinlets": 1, "numoutlets": 0, "patching_rect": [520, 540, 260, 22], "text": "bbb.zmq.send @endpoint tcp://*:5556"}},

            {"box": {"fontname": "Arial", "fontsize": 10, "id": "c-loopback", "maxclass": "comment", "numinlets": 1, "numoutlets": 0, "patching_rect": [20, 530, 450, 34], "linecount": 2, "text": "Self-test: sender binds on 5556, recv connects to localhost:5556.\nStart sender first, then receiver. Send messages and see them loop back."}}
        ],
        "lines": [
            {"patchline": {"destination": ["recv", 0], "source": ["msg-start", 0]}},
            {"patchline": {"destination": ["recv", 0], "source": ["msg-stop", 0]}},
            {"patchline": {"destination": ["route", 0], "order": 2, "source": ["recv", 0]}},
            {"patchline": {"destination": ["routepass", 0], "order": 0, "source": ["recv", 0]}},

            {"patchline": {"destination": ["pr-sensor", 0], "order": 2, "source": ["route", 0]}},
            {"patchline": {"destination": ["parse", 0], "order": 1, "source": ["route", 0]}},
            {"patchline": {"destination": ["peek", 0], "order": 0, "source": ["route", 0]}},
            {"patchline": {"destination": ["pr-act", 0], "source": ["route", 1]}},
            {"patchline": {"destination": ["pr-unmatch", 0], "source": ["route", 2]}},

            {"patchline": {"destination": ["pr-parse", 0], "source": ["parse", 0]}},
            {"patchline": {"destination": ["pr-perr", 0], "source": ["parse", 1]}},

            {"patchline": {"destination": ["pr-peek", 0], "source": ["peek", 0]}},

            {"patchline": {"destination": ["pr-rp-match", 0], "source": ["routepass", 0]}},
            {"patchline": {"destination": ["pr-rp-unmatch", 0], "source": ["routepass", 1]}},

            {"patchline": {"destination": ["schema", 0], "source": ["msg-read", 0]}},
            {"patchline": {"destination": ["pr-s-status", 0], "source": ["schema", 0]}},
            {"patchline": {"destination": ["pr-s-err", 0], "source": ["schema", 1]}},
            {"patchline": {"destination": ["prepend-set", 0], "source": ["textedit", 0]}},
            {"patchline": {"destination": ["schema", 0], "source": ["prepend-set", 0]}},

            {"patchline": {"destination": ["sender", 0], "source": ["msg-snd-start", 0]}},
            {"patchline": {"destination": ["sender", 0], "source": ["msg-snd-stop", 0]}},
            {"patchline": {"destination": ["sender", 0], "source": ["msg-snd-simple", 0]}},
            {"patchline": {"destination": ["t-mp", 0], "source": ["btn-mp", 0]}},
            {"patchline": {"destination": ["msg-frame1", 0], "source": ["t-mp", 2]}},
            {"patchline": {"destination": ["msg-frame2", 0], "source": ["t-mp", 1]}},
            {"patchline": {"destination": ["msg-flush", 0], "source": ["t-mp", 0]}},
            {"patchline": {"destination": ["sender", 0], "source": ["msg-frame1", 0]}},
            {"patchline": {"destination": ["sender", 0], "source": ["msg-frame2", 0]}},
            {"patchline": {"destination": ["sender", 0], "source": ["msg-flush", 0]}}
        ]
    }
}
