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
            500,
            300
        ],
        "boxes": [
            {
                "box": {
                    "id": "title",
                    "maxclass": "comment",
                    "text": "bbb.zmq.recv",
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
                    "text": "Receive ZMQ multipart messages.\nOutputs packet <view_id> for each message.",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [
                        30,
                        55,
                        300,
                        40.0
                    ],
                    "fontsize": 12,
                    "fontname": "Arial",
                    "linecount": 2
                }
            },
            {
                "box": {
                    "id": "msg-start",
                    "maxclass": "message",
                    "text": "start",
                    "numinlets": 1,
                    "numoutlets": 1,
                    "outlettype": [
                        ""
                    ],
                    "patching_rect": [
                        30,
                        110,
                        55,
                        22.0
                    ]
                }
            },
            {
                "box": {
                    "id": "msg-stop",
                    "maxclass": "message",
                    "text": "stop",
                    "numinlets": 1,
                    "numoutlets": 1,
                    "outlettype": [
                        ""
                    ],
                    "patching_rect": [
                        100,
                        110,
                        45,
                        22.0
                    ]
                }
            },
            {
                "box": {
                    "id": "msg-bang",
                    "maxclass": "message",
                    "text": "bang",
                    "numinlets": 1,
                    "numoutlets": 1,
                    "outlettype": [
                        ""
                    ],
                    "patching_rect": [
                        160,
                        110,
                        45,
                        22.0
                    ]
                }
            },
            {
                "box": {
                    "id": "recv",
                    "maxclass": "newobj",
                    "text": "bbb.zmq.recv",
                    "numinlets": 1,
                    "numoutlets": 1,
                    "patching_rect": [
                        30,
                        150,
                        100,
                        22.0
                    ],
                    "outlettype": [
                        ""
                    ]
                }
            },
            {
                "box": {
                    "id": "pr-out",
                    "maxclass": "newobj",
                    "text": "print recv",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [
                        30,
                        190,
                        90,
                        22.0
                    ]
                }
            },
            {
                "box": {
                    "id": "c-endpoint",
                    "maxclass": "comment",
                    "text": "@endpoint tcp://*:5555",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [
                        150,
                        150,
                        200,
                        20.0
                    ],
                    "fontsize": 12.0,
                    "fontname": "Arial"
                }
            },
            {
                "box": {
                    "id": "c-type",
                    "maxclass": "comment",
                    "text": "@type sub  (sub, pull, rep, router, pair)",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [
                        150,
                        170,
                        280,
                        20.0
                    ],
                    "fontsize": 12.0,
                    "fontname": "Arial"
                }
            },
            {
                "box": {
                    "id": "c-bind",
                    "maxclass": "comment",
                    "text": "@bind 1  (1=bind, 0=connect)",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [
                        150,
                        190,
                        210,
                        20.0
                    ],
                    "fontsize": 12.0,
                    "fontname": "Arial"
                }
            },
            {
                "box": {
                    "id": "c-sub",
                    "maxclass": "comment",
                    "text": "@subscribe \"\"  (sub socket topic)",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [
                        150,
                        210,
                        240,
                        20.0
                    ],
                    "fontsize": 12.0,
                    "fontname": "Arial"
                }
            },
            {
                "box": {
                    "id": "c-hwm",
                    "maxclass": "comment",
                    "text": "@hwm 1000",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [
                        150,
                        230,
                        100,
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
                        "msg-start",
                        0
                    ],
                    "destination": [
                        "recv",
                        0
                    ],
                    "hidden": 0,
                    "midpoints": []
                }
            },
            {
                "patchline": {
                    "source": [
                        "msg-stop",
                        0
                    ],
                    "destination": [
                        "recv",
                        0
                    ],
                    "hidden": 0,
                    "midpoints": []
                }
            },
            {
                "patchline": {
                    "source": [
                        "msg-bang",
                        0
                    ],
                    "destination": [
                        "recv",
                        0
                    ],
                    "hidden": 0,
                    "midpoints": []
                }
            },
            {
                "patchline": {
                    "source": [
                        "recv",
                        0
                    ],
                    "destination": [
                        "pr-out",
                        0
                    ],
                    "hidden": 0,
                    "midpoints": []
                }
            }
        ]
    }
}