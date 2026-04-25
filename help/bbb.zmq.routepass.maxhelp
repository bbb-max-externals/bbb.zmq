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
            580,
            320
        ],
        "boxes": [
            {
                "box": {
                    "id": "title",
                    "maxclass": "comment",
                    "text": "bbb.zmq.routepass",
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
                    "text": "Like bbb.zmq.route but the matched frame\nis NOT consumed. View passes through unchanged.",
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
                    "id": "rp",
                    "maxclass": "newobj",
                    "text": "bbb.zmq.routepass alpha beta",
                    "numinlets": 1,
                    "numoutlets": 3,
                    "patching_rect": [
                        30,
                        170,
                        200,
                        22.0
                    ],
                    "outlettype": [
                        "",
                        "",
                        ""
                    ]
                }
            },
            {
                "box": {
                    "id": "pr-a",
                    "maxclass": "newobj",
                    "text": "print alpha",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [
                        30,
                        220,
                        80,
                        22.0
                    ]
                }
            },
            {
                "box": {
                    "id": "pr-b",
                    "maxclass": "newobj",
                    "text": "print beta",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [
                        260,
                        220,
                        70,
                        22.0
                    ]
                }
            },
            {
                "box": {
                    "id": "pr-u",
                    "maxclass": "newobj",
                    "text": "print unmatched",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [
                        360,
                        220,
                        110,
                        22.0
                    ]
                }
            },
            {
                "box": {
                    "id": "c1",
                    "maxclass": "comment",
                    "text": "outlet 0: alpha (frame kept)",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [
                        30,
                        250,
                        180,
                        20.0
                    ],
                    "fontsize": 12.0,
                    "fontname": "Arial"
                }
            },
            {
                "box": {
                    "id": "c2",
                    "maxclass": "comment",
                    "text": "outlet 1: beta (frame kept)",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [
                        260,
                        250,
                        170,
                        20.0
                    ],
                    "fontsize": 12.0,
                    "fontname": "Arial"
                }
            },
            {
                "box": {
                    "id": "c3",
                    "maxclass": "comment",
                    "text": "outlet 2: unmatched",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [
                        360,
                        250,
                        150,
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
                        "rp",
                        0
                    ],
                    "hidden": 0,
                    "midpoints": []
                }
            },
            {
                "patchline": {
                    "source": [
                        "rp",
                        0
                    ],
                    "destination": [
                        "pr-a",
                        0
                    ],
                    "hidden": 0,
                    "midpoints": []
                }
            },
            {
                "patchline": {
                    "source": [
                        "rp",
                        1
                    ],
                    "destination": [
                        "pr-b",
                        0
                    ],
                    "hidden": 0,
                    "midpoints": []
                }
            },
            {
                "patchline": {
                    "source": [
                        "rp",
                        2
                    ],
                    "destination": [
                        "pr-u",
                        0
                    ],
                    "hidden": 0,
                    "midpoints": []
                }
            }
        ]
    }
}