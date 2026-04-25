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
            520,
            280
        ],
        "boxes": [
            {
                "box": {
                    "id": "title",
                    "maxclass": "comment",
                    "text": "bbb.zmq.parse",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [
                        30,
                        30,
                        150,
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
                    "text": "Parse a packet frame using a compiled schema.\nLeft outlet: parsed messages. Right outlet: diagnostics.",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [
                        30,
                        55,
                        350,
                        40.0
                    ],
                    "fontsize": 12,
                    "fontname": "Arial",
                    "linecount": 2
                }
            },
            {
                "box": {
                    "id": "msg-pkt",
                    "maxclass": "message",
                    "text": "packet 1",
                    "numinlets": 1,
                    "numoutlets": 1,
                    "outlettype": [
                        ""
                    ],
                    "patching_rect": [
                        30,
                        130,
                        70,
                        22.0
                    ]
                }
            },
            {
                "box": {
                    "id": "parse",
                    "maxclass": "newobj",
                    "text": "bbb.zmq.parse my_schema",
                    "numinlets": 1,
                    "numoutlets": 2,
                    "patching_rect": [
                        30,
                        170,
                        170,
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
                    "id": "pr-data",
                    "maxclass": "newobj",
                    "text": "print parsed",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [
                        30,
                        210,
                        80,
                        22.0
                    ]
                }
            },
            {
                "box": {
                    "id": "pr-err",
                    "maxclass": "newobj",
                    "text": "print diag",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [
                        220,
                        210,
                        60,
                        22.0
                    ]
                }
            },
            {
                "box": {
                    "id": "c-schema",
                    "maxclass": "comment",
                    "text": "@schema my_schema  (or as arg)",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [
                        220,
                        170,
                        230,
                        20.0
                    ],
                    "fontsize": 12.0,
                    "fontname": "Arial"
                }
            },
            {
                "box": {
                    "id": "c-frame",
                    "maxclass": "comment",
                    "text": "@frame 0  (frame index in view)",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [
                        220,
                        190,
                        220,
                        20.0
                    ],
                    "fontsize": 12.0,
                    "fontname": "Arial"
                }
            },
            {
                "box": {
                    "id": "c-max",
                    "maxclass": "comment",
                    "text": "@maxbytes 0 @maxatoms 0",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [
                        220,
                        210,
                        200,
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
                        "msg-pkt",
                        0
                    ],
                    "destination": [
                        "parse",
                        0
                    ],
                    "hidden": 0,
                    "midpoints": []
                }
            },
            {
                "patchline": {
                    "source": [
                        "parse",
                        0
                    ],
                    "destination": [
                        "pr-data",
                        0
                    ],
                    "hidden": 0,
                    "midpoints": []
                }
            },
            {
                "patchline": {
                    "source": [
                        "parse",
                        1
                    ],
                    "destination": [
                        "pr-err",
                        0
                    ],
                    "hidden": 0,
                    "midpoints": []
                }
            }
        ]
    }
}