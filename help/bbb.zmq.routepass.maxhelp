{
    "patcher": {
        "fileversion": 1,
        "appversion": {
            "major": 9, "minor": 1, "revision": 3,
            "architecture": "x64", "modernui": 1
        },
        "classnamespace": "box",
        "rect": [50, 80, 640, 400],
        "bglocked": 1,
        "boxes": [
            {"box": {
                "id": "title", "maxclass": "comment",
                "text": "bbb.zmq.routepass",
                "numinlets": 1, "numoutlets": 0,
                "patching_rect": [30, 20, 200, 24],
                "fontsize": 18, "fontname": "Arial", "fontface": 1
            }},
            {"box": {
                "id": "desc", "maxclass": "comment",
                "text": "Route packets by matching the first frame text.\nMatched frame is NOT consumed — view passes through unchanged.\nUse when downstream objects need the routing key still present.",
                "numinlets": 1, "numoutlets": 0,
                "patching_rect": [30, 50, 530, 54],
                "fontsize": 12, "fontname": "Arial", "linecount": 3
            }},

            {"box": {"id": "sec-ex", "maxclass": "comment", "text": "--- Example ---", "numinlets": 1, "numoutlets": 0, "patching_rect": [30, 115, 120, 20], "fontsize": 12, "fontname": "Arial", "fontface": 1}},
            {"box": {
                "id": "msg-pkt", "maxclass": "message", "text": "packet 1",
                "numinlets": 1, "numoutlets": 1, "outlettype": [""],
                "patching_rect": [30, 140, 70, 22]
            }},
            {"box": {
                "id": "rp", "maxclass": "newobj", "text": "bbb.zmq.routepass /imu /camera",
                "numinlets": 1, "numoutlets": 3,
                "patching_rect": [30, 175, 220, 22],
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
                "text": "outlet 0: matched /imu — frame KEPT in view",
                "numinlets": 1, "numoutlets": 0,
                "patching_rect": [30, 280, 300, 20], "fontsize": 11, "fontname": "Arial"
            }},
            {"box": {
                "id": "c-o1", "maxclass": "comment",
                "text": "outlet 1: matched /camera — frame KEPT in view",
                "numinlets": 1, "numoutlets": 0,
                "patching_rect": [30, 300, 300, 20], "fontsize": 11, "fontname": "Arial"
            }},
            {"box": {
                "id": "c-o2", "maxclass": "comment",
                "text": "outlet 2 (rightmost): unmatched — view unchanged",
                "numinlets": 1, "numoutlets": 0,
                "patching_rect": [30, 320, 330, 20], "fontsize": 11, "fontname": "Arial"
            }},

            {"box": {"id": "sec-vs", "maxclass": "comment", "text": "--- route vs routepass ---", "numinlets": 1, "numoutlets": 0, "patching_rect": [380, 260, 210, 20], "fontsize": 12, "fontname": "Arial", "fontface": 1}},
            {"box": {
                "id": "c-vs", "maxclass": "comment",
                "text": "route     = consumes matched frame\nroutepass = keeps matched frame\n\nUse routepass when downstream also needs\nthe routing key (e.g. @frame 1 in parse).",
                "numinlets": 1, "numoutlets": 0,
                "patching_rect": [380, 280, 240, 88], "fontsize": 11, "fontname": "Arial", "linecount": 5
            }}
        ],
        "lines": [
            {"patchline": {"source": ["msg-pkt", 0], "destination": ["rp", 0]}},
            {"patchline": {"source": ["rp", 0], "destination": ["pr-imu", 0]}},
            {"patchline": {"source": ["rp", 1], "destination": ["pr-cam", 0]}},
            {"patchline": {"source": ["rp", 2], "destination": ["pr-u", 0]}}
        ]
    }
}
