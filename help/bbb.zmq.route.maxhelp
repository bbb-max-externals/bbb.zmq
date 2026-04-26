{
    "patcher": {
        "fileversion": 1,
        "appversion": {
            "major": 9, "minor": 1, "revision": 3,
            "architecture": "x64", "modernui": 1
        },
        "classnamespace": "box",
        "rect": [50, 80, 620, 400],
        "bglocked": 1,
        "boxes": [
            {"box": {
                "id": "title", "maxclass": "comment",
                "text": "bbb.zmq.route",
                "numinlets": 1, "numoutlets": 0,
                "patching_rect": [30, 20, 200, 24],
                "fontsize": 18, "fontname": "Arial", "fontface": 1
            }},
            {"box": {
                "id": "desc", "maxclass": "comment",
                "text": "Route packets by matching the first frame text.\nMatched frame is CONSUMED (removed from the view).\nBinary or non-text first frames go to unmatched outlet.",
                "numinlets": 1, "numoutlets": 0,
                "patching_rect": [30, 50, 500, 54],
                "fontsize": 12, "fontname": "Arial", "linecount": 3
            }},

            {"box": {"id": "sec-ex", "maxclass": "comment", "text": "--- Example ---", "numinlets": 1, "numoutlets": 0, "patching_rect": [30, 115, 120, 20], "fontsize": 12, "fontname": "Arial", "fontface": 1}},
            {"box": {
                "id": "msg-pkt", "maxclass": "message", "text": "packet 1",
                "numinlets": 1, "numoutlets": 1, "outlettype": [""],
                "patching_rect": [30, 140, 70, 22]
            }},
            {"box": {
                "id": "c-pkt", "maxclass": "comment",
                "text": "Assumes view has frame[0] = \"alpha\"",
                "numinlets": 1, "numoutlets": 0,
                "patching_rect": [110, 140, 250, 20], "fontsize": 11, "fontname": "Arial"
            }},
            {"box": {
                "id": "route", "maxclass": "newobj", "text": "bbb.zmq.route /imu /camera",
                "numinlets": 1, "numoutlets": 3,
                "patching_rect": [30, 175, 190, 22],
                "outlettype": ["", "", ""]
            }},
            {"box": {
                "id": "pr-imu", "maxclass": "newobj", "text": "print /imu",
                "numinlets": 1, "numoutlets": 0,
                "patching_rect": [30, 220, 75, 22]
            }},
            {"box": {
                "id": "pr-cam", "maxclass": "newobj", "text": "print /camera",
                "numinlets": 1, "numoutlets": 0,
                "patching_rect": [130, 220, 90, 22]
            }},
            {"box": {
                "id": "pr-u", "maxclass": "newobj", "text": "print unmatched",
                "numinlets": 1, "numoutlets": 0,
                "patching_rect": [260, 220, 110, 22]
            }},

            {"box": {"id": "sec-out", "maxclass": "comment", "text": "--- Outlets ---", "numinlets": 1, "numoutlets": 0, "patching_rect": [30, 260, 120, 20], "fontsize": 12, "fontname": "Arial", "fontface": 1}},
            {"box": {
                "id": "c-o0", "maxclass": "comment",
                "text": "outlet 0: matched /imu — frame consumed, view shifted +1",
                "numinlets": 1, "numoutlets": 0,
                "patching_rect": [30, 280, 400, 20], "fontsize": 11, "fontname": "Arial"
            }},
            {"box": {
                "id": "c-o1", "maxclass": "comment",
                "text": "outlet 1: matched /camera — frame consumed, view shifted +1",
                "numinlets": 1, "numoutlets": 0,
                "patching_rect": [30, 300, 400, 20], "fontsize": 11, "fontname": "Arial"
            }},
            {"box": {
                "id": "c-o2", "maxclass": "comment",
                "text": "outlet 2 (rightmost): unmatched — view unchanged",
                "numinlets": 1, "numoutlets": 0,
                "patching_rect": [30, 320, 330, 20], "fontsize": 11, "fontname": "Arial"
            }},

            {"box": {"id": "sec-args", "maxclass": "comment", "text": "--- Arguments ---", "numinlets": 1, "numoutlets": 0, "patching_rect": [400, 260, 130, 20], "fontsize": 12, "fontname": "Arial", "fontface": 1}},
            {"box": {
                "id": "c-args", "maxclass": "comment",
                "text": "Route keys as creation arguments:\nbbb.zmq.route key1 key2 key3\nCreates N matched outlets + 1 unmatched.",
                "numinlets": 1, "numoutlets": 0,
                "patching_rect": [400, 280, 190, 54], "fontsize": 11, "fontname": "Arial", "linecount": 3
            }},

            {"box": {"id": "sec-cascade", "maxclass": "comment", "text": "--- Cascade Example ---", "numinlets": 1, "numoutlets": 0, "patching_rect": [30, 355, 180, 20], "fontsize": 12, "fontname": "Arial", "fontface": 1}},
            {"box": {
                "id": "c-cascade", "maxclass": "comment",
                "text": "recv → route /sensor → route /imu → parse imu\nEach match strips one routing frame.",
                "numinlets": 1, "numoutlets": 0,
                "patching_rect": [30, 375, 380, 34], "fontsize": 11, "fontname": "Arial", "linecount": 2
            }}
        ],
        "lines": [
            {"patchline": {"source": ["msg-pkt", 0], "destination": ["route", 0]}},
            {"patchline": {"source": ["route", 0], "destination": ["pr-imu", 0]}},
            {"patchline": {"source": ["route", 1], "destination": ["pr-cam", 0]}},
            {"patchline": {"source": ["route", 2], "destination": ["pr-u", 0]}}
        ]
    }
}
