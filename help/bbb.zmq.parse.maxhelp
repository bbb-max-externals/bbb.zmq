{
    "patcher": {
        "fileversion": 1,
        "appversion": {
            "major": 9, "minor": 1, "revision": 3,
            "architecture": "x64", "modernui": 1
        },
        "classnamespace": "box",
        "rect": [50, 80, 700, 480],
        "bglocked": 1,
        "boxes": [
            {"box": {
                "id": "title", "maxclass": "comment",
                "text": "bbb.zmq.parse",
                "numinlets": 1, "numoutlets": 0,
                "patching_rect": [30, 20, 200, 24],
                "fontsize": 18, "fontname": "Arial", "fontface": 1
            }},
            {"box": {
                "id": "desc", "maxclass": "comment",
                "text": "Parse a packet frame using a compiled DSL schema.\nOutputs Max messages (selector + atoms) defined by emit rules.\nLeft outlet: parsed data. Right outlet: errors.",
                "numinlets": 1, "numoutlets": 0,
                "patching_rect": [30, 50, 520, 54],
                "fontsize": 12, "fontname": "Arial", "linecount": 3
            }},

            {"box": {"id": "sec-ex", "maxclass": "comment", "text": "--- Example ---", "numinlets": 1, "numoutlets": 0, "patching_rect": [30, 115, 120, 20], "fontsize": 12, "fontname": "Arial", "fontface": 1}},
            {"box": {
                "id": "msg-pkt", "maxclass": "message", "text": "packet 1",
                "numinlets": 1, "numoutlets": 1, "outlettype": [""],
                "patching_rect": [30, 140, 70, 22]
            }},
            {"box": {
                "id": "parse", "maxclass": "newobj", "text": "bbb.zmq.parse imu",
                "numinlets": 1, "numoutlets": 2,
                "patching_rect": [30, 175, 150, 22],
                "outlettype": ["", ""]
            }},
            {"box": {
                "id": "pr-data", "maxclass": "newobj", "text": "print parsed",
                "numinlets": 1, "numoutlets": 0,
                "patching_rect": [30, 220, 80, 22]
            }},
            {"box": {
                "id": "pr-err", "maxclass": "newobj", "text": "print diag",
                "numinlets": 1, "numoutlets": 0,
                "patching_rect": [200, 220, 65, 22]
            }},
            {"box": {
                "id": "c-output", "maxclass": "comment",
                "text": "Output example:\ntimestamp 123456789\naccel 0.12 -0.03 9.81\ngyro 0.01 0.02 0.00",
                "numinlets": 1, "numoutlets": 0,
                "patching_rect": [300, 220, 220, 54], "fontsize": 11, "fontname": "Arial", "linecount": 4
            }},

            {"box": {"id": "sec-attr", "maxclass": "comment", "text": "--- Attributes ---", "numinlets": 1, "numoutlets": 0, "patching_rect": [300, 130, 130, 20], "fontsize": 12, "fontname": "Arial", "fontface": 1}},
            {"box": {
                "id": "c-schema", "maxclass": "comment",
                "text": "@schema <name>  Schema name (or use arg)",
                "numinlets": 1, "numoutlets": 0,
                "patching_rect": [300, 150, 280, 20], "fontsize": 11, "fontname": "Arial"
            }},
            {"box": {
                "id": "c-frame", "maxclass": "comment",
                "text": "@frame 0  Frame index within current view (0-based)",
                "numinlets": 1, "numoutlets": 0,
                "patching_rect": [300, 170, 370, 20], "fontsize": 11, "fontname": "Arial"
            }},

            {"box": {"id": "sec-limits", "maxclass": "comment", "text": "--- Limit Overrides ---", "numinlets": 1, "numoutlets": 0, "patching_rect": [300, 300, 170, 20], "fontsize": 12, "fontname": "Arial", "fontface": 1}},
            {"box": {
                "id": "c-limits", "maxclass": "comment",
                "text": "@maxbytes 0    (0 = use schema default)\n@maxitems 0    (max array elements)\n@maxatoms 0    (max atoms per emit)\n@maxstring 0   (max string search length)",
                "numinlets": 1, "numoutlets": 0,
                "patching_rect": [300, 320, 300, 74], "fontsize": 11, "fontname": "Arial", "linecount": 4
            }},

            {"box": {"id": "sec-io", "maxclass": "comment", "text": "--- Inlet / Outlets ---", "numinlets": 1, "numoutlets": 0, "patching_rect": [30, 260, 170, 20], "fontsize": 12, "fontname": "Arial", "fontface": 1}},
            {"box": {
                "id": "c-inlet", "maxclass": "comment",
                "text": "inlet 0: packet <view_id>",
                "numinlets": 1, "numoutlets": 0,
                "patching_rect": [30, 280, 200, 20], "fontsize": 11, "fontname": "Arial"
            }},
            {"box": {
                "id": "c-out-data", "maxclass": "comment",
                "text": "outlet 0 (left): <selector> <atom1> <atom2> ...  parsed messages",
                "numinlets": 1, "numoutlets": 0,
                "patching_rect": [30, 300, 250, 20], "fontsize": 11, "fontname": "Arial"
            }},
            {"box": {
                "id": "c-out-err", "maxclass": "comment",
                "text": "outlet 1 (right): error <view_id> <message>",
                "numinlets": 1, "numoutlets": 0,
                "patching_rect": [30, 320, 250, 20], "fontsize": 11, "fontname": "Arial"
            }},

            {"box": {"id": "sec-workflow", "maxclass": "comment", "text": "--- Usage with route ---", "numinlets": 1, "numoutlets": 0, "patching_rect": [30, 360, 200, 20], "fontsize": 12, "fontname": "Arial", "fontface": 1}},
            {"box": {
                "id": "c-wf-route", "maxclass": "comment",
                "text": "recv → route /imu → parse imu @frame 0\nAfter route consumes /imu, frame[0] = payload.",
                "numinlets": 1, "numoutlets": 0,
                "patching_rect": [30, 380, 350, 34], "fontsize": 11, "fontname": "Arial", "linecount": 2
            }},
            {"box": {
                "id": "c-wf-routepass", "maxclass": "comment",
                "text": "recv → routepass /imu → parse imu @frame 1\nWith routepass, frame[0] still = /imu, payload at frame[1].",
                "numinlets": 1, "numoutlets": 0,
                "patching_rect": [30, 420, 400, 34], "fontsize": 11, "fontname": "Arial", "linecount": 2
            }}
        ],
        "lines": [
            {"patchline": {"source": ["msg-pkt", 0], "destination": ["parse", 0]}},
            {"patchline": {"source": ["parse", 0], "destination": ["pr-data", 0]}},
            {"patchline": {"source": ["parse", 1], "destination": ["pr-err", 0]}}
        ]
    }
}
