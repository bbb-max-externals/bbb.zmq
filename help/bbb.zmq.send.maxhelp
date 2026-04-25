{
    "patcher": {
        "fileversion": 1,
        "appversion": {
            "major": 9, "minor": 1, "revision": 3,
            "architecture": "x64", "modernui": 1
        },
        "classnamespace": "box",
        "rect": [50, 80, 680, 440],
        "boxes": [
            {"box": {
                "id": "title", "maxclass": "comment", "text": "bbb.zmq.send",
                "numinlets": 1, "numoutlets": 0,
                "patching_rect": [30, 25, 150, 20.0],
                "fontsize": 18, "fontname": "Arial", "fontface": 1
            }},
            {"box": {
                "id": "desc", "maxclass": "comment",
                "text": "Send ZMQ messages.\nsend = single text frame. frame/flush = multipart.",
                "numinlets": 1, "numoutlets": 0,
                "patching_rect": [30, 50, 320, 40.0],
                "fontsize": 12, "fontname": "Arial", "linecount": 2
            }},
            {"box": {
                "id": "msg-start", "maxclass": "message", "text": "start",
                "numinlets": 1, "numoutlets": 1, "outlettype": [""],
                "patching_rect": [30, 100, 50, 22.0]
            }},
            {"box": {
                "id": "msg-stop", "maxclass": "message", "text": "stop",
                "numinlets": 1, "numoutlets": 1, "outlettype": [""],
                "patching_rect": [90, 100, 45, 22.0]
            }},
            {"box": {
                "id": "msg-send", "maxclass": "message", "text": "send hello world",
                "numinlets": 1, "numoutlets": 1, "outlettype": [""],
                "patching_rect": [30, 140, 120, 22.0]
            }},
            {"box": {
                "id": "msg-frame1", "maxclass": "message", "text": "frame sensor",
                "numinlets": 1, "numoutlets": 1, "outlettype": [""],
                "patching_rect": [30, 180, 90, 22.0]
            }},
            {"box": {
                "id": "msg-frame2", "maxclass": "message", "text": "frame_bytes 1 0 0 0 42",
                "numinlets": 1, "numoutlets": 1, "outlettype": [""],
                "patching_rect": [30, 210, 160, 22.0]
            }},
            {"box": {
                "id": "msg-flush", "maxclass": "message", "text": "flush",
                "numinlets": 1, "numoutlets": 1, "outlettype": [""],
                "patching_rect": [200, 210, 50, 22.0]
            }},
            {"box": {
                "id": "t-bf", "maxclass": "newobj", "text": "t b b",
                "numinlets": 1, "numoutlets": 2, "outlettype": ["bang", "bang"],
                "patching_rect": [30, 250, 40, 22.0]
            }},
            {"box": {
                "id": "send", "maxclass": "newobj", "text": "bbb.zmq.send",
                "numinlets": 1, "numoutlets": 0,
                "patching_rect": [30, 290, 100, 22.0]
            }},
            {"box": {
                "id": "c-attr", "maxclass": "comment",
                "text": "@endpoint tcp://*:5556\n@type pub (pub, push, pair)\n@bind 1  @hwm 1000",
                "numinlets": 1, "numoutlets": 0,
                "patching_rect": [150, 290, 200, 50.0],
                "fontsize": 12, "fontname": "Arial", "linecount": 3
            }},
            {"box": {
                "id": "c-mp", "maxclass": "comment",
                "text": "Multipart: frame + frame_bytes → t b b → flush",
                "numinlets": 1, "numoutlets": 0,
                "patching_rect": [30, 320, 300, 20.0],
                "fontsize": 11, "fontname": "Arial"
            }},
            {"box": {
                "id": "c-send", "maxclass": "comment",
                "text": "send = immediate single frame",
                "numinlets": 1, "numoutlets": 0,
                "patching_rect": [160, 140, 200, 20.0],
                "fontsize": 11, "fontname": "Arial"
            }}
        ],
        "lines": [
            {"patchline": {"source": ["msg-start", 0], "destination": ["send", 0], "hidden": 0, "midpoints": []}},
            {"patchline": {"source": ["msg-stop", 0], "destination": ["send", 0], "hidden": 0, "midpoints": []}},
            {"patchline": {"source": ["msg-send", 0], "destination": ["send", 0], "hidden": 0, "midpoints": []}},
            {"patchline": {"source": ["msg-frame1", 0], "destination": ["send", 0], "hidden": 0, "midpoints": []}},
            {"patchline": {"source": ["msg-frame2", 0], "destination": ["send", 0], "hidden": 0, "midpoints": []}},
            {"patchline": {"source": ["t-bf", 0], "destination": ["msg-frame1", 0], "hidden": 0, "midpoints": []}},
            {"patchline": {"source": ["t-bf", 1], "destination": ["msg-flush", 0], "hidden": 0, "midpoints": []}},
            {"patchline": {"source": ["msg-flush", 0], "destination": ["send", 0], "hidden": 0, "midpoints": []}}
        ]
    }
}
