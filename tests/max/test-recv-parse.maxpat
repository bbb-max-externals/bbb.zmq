{
    "patcher": {
        "fileversion": 1,
        "appversion": { "major": 9, "minor": 1, "revision": 3, "architecture": "x64", "modernui": 1 },
        "classnamespace": "box",
        "rect": [50, 50, 1200, 550],
        "bglocked": 1,
        "boxes": [
            {"box": {"fontface": 1, "fontname": "Arial", "fontsize": 16, "id": "title", "maxclass": "comment", "numinlets": 1, "numoutlets": 0, "patching_rect": [20, 12, 400, 24], "text": "Test: Receive + Route + Parse (Node → Max)"}},
            {"box": {"fontname": "Arial", "fontsize": 11, "id": "desc", "maxclass": "comment", "numinlets": 1, "numoutlets": 0, "patching_rect": [20, 38, 500, 18], "text": "Max binds port 5555. Run node scripts (npm run send:all) to send test data."}},

            {"box": {"fontface": 1, "fontname": "Arial", "fontsize": 12, "id": "sec-recv", "maxclass": "comment", "numinlets": 1, "numoutlets": 0, "patching_rect": [20, 70, 130, 20], "text": "=== Receiver ==="}},
            {"box": {"id": "msg-start", "maxclass": "message", "numinlets": 1, "numoutlets": 1, "outlettype": [""], "patching_rect": [20, 93, 50, 22], "text": "start"}},
            {"box": {"id": "msg-stop", "maxclass": "message", "numinlets": 1, "numoutlets": 1, "outlettype": [""], "patching_rect": [80, 93, 45, 22], "text": "stop"}},
            {"box": {"id": "recv", "maxclass": "newobj", "numinlets": 1, "numoutlets": 1, "outlettype": [""], "patching_rect": [20, 120, 330, 22], "text": "bbb.zmq.recv @type sub @endpoint tcp://*:5555 @bind 1"}},

            {"box": {"fontface": 1, "fontname": "Arial", "fontsize": 12, "id": "sec-route", "maxclass": "comment", "numinlets": 1, "numoutlets": 0, "patching_rect": [20, 155, 270, 20], "text": "=== Route (consumes key) ==="}},
            {"box": {"id": "route", "maxclass": "newobj", "numinlets": 1, "numoutlets": 4, "outlettype": ["", "", "", ""], "patching_rect": [20, 178, 260, 22], "text": "bbb.zmq.route /imu /sensor plain-text"}},

            {"box": {"fontface": 1, "fontname": "Arial", "fontsize": 12, "id": "sec-peek", "maxclass": "comment", "numinlets": 1, "numoutlets": 0, "patching_rect": [320, 155, 120, 20], "text": "=== Debug ==="}},
            {"box": {"id": "peek", "maxclass": "newobj", "numinlets": 1, "numoutlets": 1, "outlettype": [""], "patching_rect": [320, 178, 152, 22], "text": "bbb.zmq.peek @verbose 2"}},
            {"box": {"id": "pr-debug", "maxclass": "newobj", "numinlets": 1, "numoutlets": 0, "patching_rect": [320, 208, 80, 22], "text": "print debug"}},

            {"box": {"fontface": 1, "fontname": "Arial", "fontsize": 12, "id": "sec-imu", "maxclass": "comment", "numinlets": 1, "numoutlets": 0, "patching_rect": [20, 220, 120, 20], "text": "=== Parse IMU ==="}},
            {"box": {"id": "parse-imu", "maxclass": "newobj", "numinlets": 1, "numoutlets": 2, "outlettype": ["", ""], "patching_rect": [20, 243, 155, 22], "text": "bbb.zmq.parse imu"}},
            {"box": {"id": "pr-imu", "maxclass": "newobj", "numinlets": 1, "numoutlets": 0, "patching_rect": [20, 273, 55, 22], "text": "print imu"}},
            {"box": {"id": "pr-imu-err", "maxclass": "newobj", "numinlets": 1, "numoutlets": 0, "patching_rect": [90, 273, 80, 22], "text": "print imu-err"}},

            {"box": {"fontface": 1, "fontname": "Arial", "fontsize": 12, "id": "sec-sensor", "maxclass": "comment", "numinlets": 1, "numoutlets": 0, "patching_rect": [20, 305, 140, 20], "text": "=== Parse Simple ==="}},
            {"box": {"id": "parse-simple", "maxclass": "newobj", "numinlets": 1, "numoutlets": 2, "outlettype": ["", ""], "patching_rect": [20, 328, 170, 22], "text": "bbb.zmq.parse simple"}},
            {"box": {"id": "pr-simple", "maxclass": "newobj", "numinlets": 1, "numoutlets": 0, "patching_rect": [20, 358, 75, 22], "text": "print simple"}},
            {"box": {"id": "pr-simple-err", "maxclass": "newobj", "numinlets": 1, "numoutlets": 0, "patching_rect": [110, 358, 95, 22], "text": "print simple-err"}},

            {"box": {"fontname": "Arial", "fontsize": 11, "id": "pr-text", "maxclass": "newobj", "numinlets": 1, "numoutlets": 0, "patching_rect": [20, 395, 55, 22], "text": "print text"}},
            {"box": {"fontname": "Arial", "fontsize": 11, "id": "pr-unmatch", "maxclass": "newobj", "numinlets": 1, "numoutlets": 0, "patching_rect": [90, 395, 95, 22], "text": "print unmatch"}},

            {"box": {"fontface": 1, "fontname": "Arial", "fontsize": 12, "id": "sec-schemai", "maxclass": "comment", "numinlets": 1, "numoutlets": 0, "patching_rect": [600, 70, 130, 20], "text": "=== Schema IMU ==="}},
            {"box": {"id": "te-imu", "linecount": 10, "maxclass": "textedit", "numinlets": 1, "numoutlets": 4, "outlettype": ["", "int", "", ""], "parameter_enable": 0, "patching_rect": [600, 93, 380, 120], "text": "schema imu {\nendian little;\nu16 magic = 0xCAFE;\nu8 version;\nu8 type;\nu64 timestamp;\nf32 accel[3];\nf32 gyro[3];\nemit timestamp timestamp;\nemit version version;\nemit type type;\nemit accel accel[0] accel[1] accel[2];\nemit gyro gyro[0] gyro[1] gyro[2];\n}"}},
            {"box": {"id": "ps-imu", "maxclass": "newobj", "numinlets": 1, "numoutlets": 1, "outlettype": [""], "patching_rect": [600, 220, 75, 22], "text": "prepend set"}},
            {"box": {"id": "sc-imu", "maxclass": "newobj", "numinlets": 1, "numoutlets": 2, "outlettype": ["", ""], "patching_rect": [600, 248, 280, 22], "text": "bbb.zmq.schema @name imu @autocompile 1"}},
            {"box": {"id": "pr-si-ok", "maxclass": "newobj", "numinlets": 1, "numoutlets": 0, "patching_rect": [600, 278, 95, 22], "text": "print si-ok"}},
            {"box": {"id": "pr-si-err", "maxclass": "newobj", "numinlets": 1, "numoutlets": 0, "patching_rect": [710, 278, 80, 22], "text": "print si-err"}},

            {"box": {"fontface": 1, "fontname": "Arial", "fontsize": 12, "id": "sec-schemas", "maxclass": "comment", "numinlets": 1, "numoutlets": 0, "patching_rect": [600, 310, 160, 20], "text": "=== Schema Simple ==="}},
            {"box": {"id": "te-simple", "linecount": 6, "maxclass": "textedit", "numinlets": 1, "numoutlets": 4, "outlettype": ["", "int", "", ""], "parameter_enable": 0, "patching_rect": [600, 333, 380, 75], "text": "schema simple {\nendian big;\nu8 id;\ni32 value;\nf32 temp;\nemit id value temp;\n}"}},
            {"box": {"id": "ps-simple", "maxclass": "newobj", "numinlets": 1, "numoutlets": 1, "outlettype": [""], "patching_rect": [600, 415, 75, 22], "text": "prepend set"}},
            {"box": {"id": "sc-simple", "maxclass": "newobj", "numinlets": 1, "numoutlets": 2, "outlettype": ["", ""], "patching_rect": [600, 443, 290, 22], "text": "bbb.zmq.schema @name simple @autocompile 1"}},
            {"box": {"id": "pr-ss-ok", "maxclass": "newobj", "numinlets": 1, "numoutlets": 0, "patching_rect": [600, 473, 95, 22], "text": "print ss-ok"}},
            {"box": {"id": "pr-ss-err", "maxclass": "newobj", "numinlets": 1, "numoutlets": 0, "patching_rect": [710, 473, 80, 22], "text": "print ss-err"}}
        ],
        "lines": [
            {"patchline": {"destination": ["recv", 0], "source": ["msg-start", 0]}},
            {"patchline": {"destination": ["recv", 0], "source": ["msg-stop", 0]}},
            {"patchline": {"destination": ["route", 0], "order": 1, "source": ["recv", 0]}},
            {"patchline": {"destination": ["peek", 0], "order": 0, "source": ["recv", 0]}},
            {"patchline": {"destination": ["pr-debug", 0], "source": ["peek", 0]}},

            {"patchline": {"destination": ["parse-imu", 0], "source": ["route", 0]}},
            {"patchline": {"destination": ["pr-imu", 0], "source": ["parse-imu", 0]}},
            {"patchline": {"destination": ["pr-imu-err", 0], "source": ["parse-imu", 1]}},

            {"patchline": {"destination": ["parse-simple", 0], "source": ["route", 1]}},
            {"patchline": {"destination": ["pr-simple", 0], "source": ["parse-simple", 0]}},
            {"patchline": {"destination": ["pr-simple-err", 0], "source": ["parse-simple", 1]}},

            {"patchline": {"destination": ["pr-text", 0], "source": ["route", 2]}},
            {"patchline": {"destination": ["pr-unmatch", 0], "source": ["route", 3]}},

            {"patchline": {"destination": ["ps-imu", 0], "source": ["te-imu", 0]}},
            {"patchline": {"destination": ["sc-imu", 0], "source": ["ps-imu", 0]}},
            {"patchline": {"destination": ["pr-si-ok", 0], "source": ["sc-imu", 0]}},
            {"patchline": {"destination": ["pr-si-err", 0], "source": ["sc-imu", 1]}},

            {"patchline": {"destination": ["ps-simple", 0], "source": ["te-simple", 0]}},
            {"patchline": {"destination": ["sc-simple", 0], "source": ["ps-simple", 0]}},
            {"patchline": {"destination": ["pr-ss-ok", 0], "source": ["sc-simple", 0]}},
            {"patchline": {"destination": ["pr-ss-err", 0], "source": ["sc-simple", 1]}}
        ]
    }
}
