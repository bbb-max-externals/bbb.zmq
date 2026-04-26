{
    "patcher": {
        "fileversion": 1,
        "appversion": {
            "major": 9, "minor": 1, "revision": 3,
            "architecture": "x64", "modernui": 1
        },
        "classnamespace": "box",
        "rect": [50, 80, 700, 530],
        "bglocked": 1,
        "boxes": [
            {"box": {
                "id": "title", "maxclass": "comment",
                "text": "bbb.zmq.schema",
                "numinlets": 1, "numoutlets": 0,
                "patching_rect": [30, 20, 200, 24],
                "fontsize": 18, "fontname": "Arial", "fontface": 1
            }},
            {"box": {
                "id": "desc", "maxclass": "comment",
                "text": "Load, compile, and register DSL schemas for bbb.zmq.parse.\nSchemas describe binary frame layouts: fields, types, emit rules.\nLeft outlet: status. Right outlet: errors.",
                "numinlets": 1, "numoutlets": 0,
                "patching_rect": [30, 50, 520, 54],
                "fontsize": 12, "fontname": "Arial", "linecount": 3
            }},

            {"box": {"id": "sec-file", "maxclass": "comment", "text": "--- Load from File ---", "numinlets": 1, "numoutlets": 0, "patching_rect": [30, 115, 170, 20], "fontsize": 12, "fontname": "Arial", "fontface": 1}},
            {"box": {
                "id": "msg-read", "maxclass": "message", "text": "read schemas/imu.zmqdsl",
                "numinlets": 1, "numoutlets": 1, "outlettype": [""],
                "patching_rect": [30, 140, 200, 22]
            }},
            {"box": {
                "id": "c-read", "maxclass": "comment",
                "text": "Read + compile a .zmqdsl file (relative to patcher path)",
                "numinlets": 1, "numoutlets": 0,
                "patching_rect": [240, 140, 350, 20], "fontsize": 11, "fontname": "Arial"
            }},
            {"box": {
                "id": "msg-reload", "maxclass": "message", "text": "reload",
                "numinlets": 1, "numoutlets": 1, "outlettype": [""],
                "patching_rect": [30, 168, 55, 22]
            }},
            {"box": {
                "id": "c-reload", "maxclass": "comment",
                "text": "Re-read the last loaded file",
                "numinlets": 1, "numoutlets": 0,
                "patching_rect": [95, 168, 200, 20], "fontsize": 11, "fontname": "Arial"
            }},

            {"box": {"id": "sec-text", "maxclass": "comment", "text": "--- Inline DSL (textedit) ---", "numinlets": 1, "numoutlets": 0, "patching_rect": [30, 200, 210, 20], "fontsize": 12, "fontname": "Arial", "fontface": 1}},
            {"box": {
                "id": "textedit", "maxclass": "textedit",
                "numinlets": 1, "numoutlets": 1, "outlettype": [""],
                "patching_rect": [30, 225, 280, 90],
                "wordwrap": 1,
                "text": "schema imu {\nendian little;\nu16 magic = 0xCAFE;\nu8 version;\nu64 timestamp;\nf32 accel[3];\nemit timestamp timestamp;\nemit accel accel[0] accel[1] accel[2];\n}"
            }},
            {"box": {
                "id": "prepend-set", "maxclass": "newobj", "text": "prepend set",
                "numinlets": 1, "numoutlets": 1,
                "patching_rect": [30, 325, 75, 22],
                "outlettype": [""]
            }},
            {"box": {
                "id": "c-auto", "maxclass": "comment",
                "text": "With @autocompile 1, schema compiles on every set.\nWithout it, send compile message manually.",
                "numinlets": 1, "numoutlets": 0,
                "patching_rect": [320, 225, 330, 34], "fontsize": 11, "fontname": "Arial", "linecount": 2
            }},

            {"box": {"id": "sec-msg", "maxclass": "comment", "text": "--- Messages ---", "numinlets": 1, "numoutlets": 0, "patching_rect": [320, 270, 130, 20], "fontsize": 12, "fontname": "Arial", "fontface": 1}},
            {"box": {
                "id": "c-msgs", "maxclass": "comment",
                "text": "set <dsl text>  — replace buffer\nappend <text>    — add line to buffer\ncompile          — compile current buffer\nclear            — clear buffer\ndump             — list registered schemas\nwrite <path>     — save buffer to file",
                "numinlets": 1, "numoutlets": 0,
                "patching_rect": [320, 290, 280, 110], "fontsize": 11, "fontname": "Arial", "linecount": 6
            }},

            {"box": {
                "id": "schema", "maxclass": "newobj", "text": "bbb.zmq.schema @name imu @autocompile 1",
                "numinlets": 1, "numoutlets": 2,
                "patching_rect": [30, 370, 270, 22],
                "outlettype": ["", ""]
            }},
            {"box": {
                "id": "pr-status", "maxclass": "newobj", "text": "print status",
                "numinlets": 1, "numoutlets": 0,
                "patching_rect": [30, 410, 90, 22]
            }},
            {"box": {
                "id": "pr-error", "maxclass": "newobj", "text": "print error",
                "numinlets": 1, "numoutlets": 0,
                "patching_rect": [150, 410, 80, 22]
            }},

            {"box": {"id": "sec-attr", "maxclass": "comment", "text": "--- Attributes ---", "numinlets": 1, "numoutlets": 0, "patching_rect": [320, 370, 130, 20], "fontsize": 12, "fontname": "Arial", "fontface": 1}},
            {"box": {
                "id": "c-name", "maxclass": "comment",
                "text": "@name <symbol>  Registration name override",
                "numinlets": 1, "numoutlets": 0,
                "patching_rect": [320, 390, 310, 20], "fontsize": 11, "fontname": "Arial"
            }},
            {"box": {
                "id": "c-file", "maxclass": "comment",
                "text": "@file <path>  Auto-load .zmqdsl on creation",
                "numinlets": 1, "numoutlets": 0,
                "patching_rect": [320, 410, 310, 20], "fontsize": 11, "fontname": "Arial"
            }},
            {"box": {
                "id": "c-autoc", "maxclass": "comment",
                "text": "@autocompile 0|1  Auto-compile on set",
                "numinlets": 1, "numoutlets": 0,
                "patching_rect": [320, 430, 250, 20], "fontsize": 11, "fontname": "Arial"
            }},

            {"box": {"id": "sec-out", "maxclass": "comment", "text": "--- Outlets ---", "numinlets": 1, "numoutlets": 0, "patching_rect": [30, 445, 120, 20], "fontsize": 12, "fontname": "Arial", "fontface": 1}},
            {"box": {
                "id": "c-ostatus", "maxclass": "comment",
                "text": "outlet 0 (left): compiled <name>, schema <name> fields N emits M",
                "numinlets": 1, "numoutlets": 0,
                "patching_rect": [30, 465, 420, 20], "fontsize": 11, "fontname": "Arial"
            }},
            {"box": {
                "id": "c-oerr", "maxclass": "comment",
                "text": "outlet 1 (right): error messages (lexer, parser, compile)",
                "numinlets": 1, "numoutlets": 0,
                "patching_rect": [30, 485, 350, 20], "fontsize": 11, "fontname": "Arial"
            }}
        ],
        "lines": [
            {"patchline": {"source": ["msg-read", 0], "destination": ["schema", 0]}},
            {"patchline": {"source": ["msg-reload", 0], "destination": ["schema", 0]}},
            {"patchline": {"source": ["textedit", 0], "destination": ["prepend-set", 0]}},
            {"patchline": {"source": ["prepend-set", 0], "destination": ["schema", 0]}},
            {"patchline": {"source": ["schema", 0], "destination": ["pr-status", 0]}},
            {"patchline": {"source": ["schema", 1], "destination": ["pr-error", 0]}}
        ]
    }
}
