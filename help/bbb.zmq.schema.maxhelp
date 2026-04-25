{
    "patcher": {
        "fileversion": 1,
        "appversion": {
            "major": 9,
            "minor": 1,
            "revision": 3,
            "architecture": "x64",
            "modernui": 1
        },
        "classnamespace": "box",
        "rect": [
            50,
            80,
            550,
            420
        ],
        "boxes": [
            {
                "box": {
                    "id": "title",
                    "maxclass": "comment",
                    "text": "bbb.zmq.schema",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [
                        30,
                        30,
                        180,
                        20.0
                    ],
                    "fontsize": 18,
                    "fontname": "Arial",
                    "fontface": 1
                }
            },
            {
                "box": {
                    "id": "desc",
                    "maxclass": "comment",
                    "text": "Compile & register DSL schemas for bbb.zmq.parse.\nLeft outlet: status. Right outlet: errors.",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [
                        30,
                        55,
                        330,
                        40.0
                    ],
                    "fontsize": 12,
                    "fontname": "Arial",
                    "linecount": 2
                }
            },
            {
                "box": {
                    "id": "msg-read",
                    "maxclass": "message",
                    "text": "read /path/to/schema.zmqdsl",
                    "numinlets": 1,
                    "numoutlets": 1,
                    "outlettype": [
                        ""
                    ],
                    "patching_rect": [
                        30,
                        130,
                        220,
                        22.0
                    ]
                }
            },
            {
                "box": {
                    "id": "msg-set",
                    "maxclass": "comment",
                    "text": "Or use textedit + prepend set for multiline DSL:",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 280, 190, 260, 20.0 ],
                    "fontsize": 12.0,
                    "fontname": "Arial"
                }
            },
            {
                "box": {
                    "id": "textedit",
                    "maxclass": "textedit",
                    "numinlets": 1,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 280, 220, 240, 80.0 ],
                    "wordwrap": 1,
                    "text": "schema test_schema {\nendian big;\nu8 id;\ni32 value;\nf32 temp;\nemit id value temp;\n}"
                }
            },
            {
                "box": {
                    "id": "prepend-set",
                    "maxclass": "newobj",
                    "text": "prepend set",
                    "numinlets": 1,
                    "numoutlets": 1,
                    "patching_rect": [ 280, 310, 75, 22.0 ],
                    "outlettype": [ "" ]
                }
            },
            {
                "box": {
                    "id": "msg-append",
                    "maxclass": "message",
                    "text": "append emit field1 field2",
                    "numinlets": 1,
                    "numoutlets": 1,
                    "outlettype": [
                        ""
                    ],
                    "patching_rect": [
                        30,
                        190,
                        190,
                        22.0
                    ]
                }
            },
            {
                "box": {
                    "id": "msg-compile",
                    "maxclass": "message",
                    "text": "compile",
                    "numinlets": 1,
                    "numoutlets": 1,
                    "outlettype": [
                        ""
                    ],
                    "patching_rect": [
                        30,
                        220,
                        65,
                        22.0
                    ]
                }
            },
            {
                "box": {
                    "id": "msg-clear",
                    "maxclass": "message",
                    "text": "clear",
                    "numinlets": 1,
                    "numoutlets": 1,
                    "outlettype": [
                        ""
                    ],
                    "patching_rect": [
                        30,
                        250,
                        50,
                        22.0
                    ]
                }
            },
            {
                "box": {
                    "id": "msg-dump",
                    "maxclass": "message",
                    "text": "dump",
                    "numinlets": 1,
                    "numoutlets": 1,
                    "outlettype": [
                        ""
                    ],
                    "patching_rect": [
                        30,
                        280,
                        50,
                        22.0
                    ]
                }
            },
            {
                "box": {
                    "id": "msg-write",
                    "maxclass": "message",
                    "text": "write /tmp/out.zmqdsl",
                    "numinlets": 1,
                    "numoutlets": 1,
                    "outlettype": [
                        ""
                    ],
                    "patching_rect": [
                        280,
                        130,
                        180,
                        22.0
                    ]
                }
            },
            {
                "box": {
                    "id": "msg-reload",
                    "maxclass": "message",
                    "text": "reload",
                    "numinlets": 1,
                    "numoutlets": 1,
                    "outlettype": [
                        ""
                    ],
                    "patching_rect": [
                        280,
                        160,
                        55,
                        22.0
                    ]
                }
            },
            {
                "box": {
                    "id": "schema",
                    "maxclass": "newobj",
                    "text": "bbb.zmq.schema",
                    "numinlets": 1,
                    "numoutlets": 2,
                    "patching_rect": [
                        30,
                        320,
                        120,
                        22.0
                    ],
                    "outlettype": [
                        "",
                        ""
                    ]
                }
            },
            {
                "box": {
                    "id": "pr-status",
                    "maxclass": "newobj",
                    "text": "print status",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [
                        30,
                        360,
                        90,
                        22.0
                    ]
                }
            },
            {
                "box": {
                    "id": "pr-error",
                    "maxclass": "newobj",
                    "text": "print error",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [
                        160,
                        360,
                        80,
                        22.0
                    ]
                }
            },
            {
                "box": {
                    "id": "c-name",
                    "maxclass": "comment",
                    "text": "@name my_schema",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [
                        170,
                        320,
                        140,
                        20.0
                    ],
                    "fontsize": 12.0,
                    "fontname": "Arial"
                }
            },
            {
                "box": {
                    "id": "c-auto",
                    "maxclass": "comment",
                    "text": "@autocompile 1",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [
                        170,
                        340,
                        140,
                        20.0
                    ],
                    "fontsize": 12.0,
                    "fontname": "Arial"
                }
            }
        ],
        "lines": [
            {
                "patchline": {
                    "source": [
                        "msg-read",
                        0
                    ],
                    "destination": [
                        "schema",
                        0
                    ],
                    "hidden": 0,
                    "midpoints": []
                }
            },
            {
                "patchline": {
                    "source": [
                        "msg-set",
                        0
                    ],
                    "destination": [
                        "schema",
                        0
                    ],
                    "hidden": 0,
                    "midpoints": []
                }
            },
            {
                "patchline": {
                    "source": [ "msg-append", 0 ],
                    "destination": [ "schema", 0 ],
                    "hidden": 0,
                    "midpoints": []
                }
            },
            {
                "patchline": {
                    "source": [ "textedit", 0 ],
                    "destination": [ "prepend-set", 0 ],
                    "hidden": 0,
                    "midpoints": []
                }
            },
            {
                "patchline": {
                    "source": [ "prepend-set", 0 ],
                    "destination": [ "schema", 0 ],
                    "hidden": 0,
                    "midpoints": []
                }
            },
            {
                "patchline": {
                    "source": [
                        "msg-compile",
                        0
                    ],
                    "destination": [
                        "schema",
                        0
                    ],
                    "hidden": 0,
                    "midpoints": []
                }
            },
            {
                "patchline": {
                    "source": [
                        "msg-clear",
                        0
                    ],
                    "destination": [
                        "schema",
                        0
                    ],
                    "hidden": 0,
                    "midpoints": []
                }
            },
            {
                "patchline": {
                    "source": [
                        "msg-dump",
                        0
                    ],
                    "destination": [
                        "schema",
                        0
                    ],
                    "hidden": 0,
                    "midpoints": []
                }
            },
            {
                "patchline": {
                    "source": [
                        "msg-write",
                        0
                    ],
                    "destination": [
                        "schema",
                        0
                    ],
                    "hidden": 0,
                    "midpoints": []
                }
            },
            {
                "patchline": {
                    "source": [
                        "msg-reload",
                        0
                    ],
                    "destination": [
                        "schema",
                        0
                    ],
                    "hidden": 0,
                    "midpoints": []
                }
            },
            {
                "patchline": {
                    "source": [
                        "schema",
                        0
                    ],
                    "destination": [
                        "pr-status",
                        0
                    ],
                    "hidden": 0,
                    "midpoints": []
                }
            },
            {
                "patchline": {
                    "source": [
                        "schema",
                        1
                    ],
                    "destination": [
                        "pr-error",
                        0
                    ],
                    "hidden": 0,
                    "midpoints": []
                }
            }
        ]
    }
}