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
            260
        ],
        "boxes": [
            {
                "box": {
                    "id": "title",
                    "maxclass": "comment",
                    "text": "bbb.zmq.peek",
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
                    "text": "Debug inspector for packet views.\nOutputs frame info, text preview, hex dumps.",
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
                        120,
                        70,
                        22.0
                    ]
                }
            },
            {
                "box": {
                    "id": "peek",
                    "maxclass": "newobj",
                    "text": "bbb.zmq.peek",
                    "numinlets": 1,
                    "numoutlets": 1,
                    "patching_rect": [
                        30,
                        160,
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
                    "id": "pr",
                    "maxclass": "newobj",
                    "text": "print peek",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [
                        30,
                        200,
                        70,
                        22.0
                    ]
                }
            },
            {
                "box": {
                    "id": "c-v",
                    "maxclass": "comment",
                    "text": "@verbose 1  (0=summary, 1=+preview, 2=+hex)",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [
                        150,
                        160,
                        300,
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
                        "peek",
                        0
                    ],
                    "hidden": 0,
                    "midpoints": []
                }
            },
            {
                "patchline": {
                    "source": [
                        "peek",
                        0
                    ],
                    "destination": [
                        "pr",
                        0
                    ],
                    "hidden": 0,
                    "midpoints": []
                }
            }
        ]
    }
}