{
    "patcher": {
        "fileversion": 1,
        "appversion": { "major": 9, "minor": 1, "revision": 3, "architecture": "x64", "modernui": 1 },
        "classnamespace": "box",
        "rect": [50, 50, 550, 420],
        "bglocked": 1,
        "boxes": [
            {"box": {"fontface": 1, "fontname": "Arial", "fontsize": 16, "id": "title", "maxclass": "comment", "numinlets": 1, "numoutlets": 0, "patching_rect": [20, 12, 400, 24], "text": "Test: Send (Max → Node)"}},
            {"box": {"fontname": "Arial", "fontsize": 11, "id": "desc", "maxclass": "comment", "numinlets": 1, "numoutlets": 0, "patching_rect": [20, 38, 450, 18], "text": "Max binds port 5556. Run 'node recv.js' to receive."}},

            {"box": {"fontface": 1, "fontname": "Arial", "fontsize": 12, "id": "sec-ctrl", "maxclass": "comment", "numinlets": 1, "numoutlets": 0, "patching_rect": [20, 70, 120, 20], "text": "=== Control ==="}},
            {"box": {"id": "msg-start", "maxclass": "message", "numinlets": 1, "numoutlets": 1, "outlettype": [""], "patching_rect": [20, 93, 50, 22], "text": "start"}},
            {"box": {"id": "msg-stop", "maxclass": "message", "numinlets": 1, "numoutlets": 1, "outlettype": [""], "patching_rect": [80, 93, 45, 22], "text": "stop"}},

            {"box": {"id": "sender", "maxclass": "newobj", "numinlets": 1, "numoutlets": 0, "patching_rect": [20, 130, 300, 22], "text": "bbb.zmq.send @type pub @endpoint tcp://*:5556 @bind 1"}},

            {"box": {"fontface": 1, "fontname": "Arial", "fontsize": 12, "id": "sec-single", "maxclass": "comment", "numinlets": 1, "numoutlets": 0, "patching_rect": [20, 170, 200, 20], "text": "=== Single Frame ==="}},
            {"box": {"id": "msg-hello", "maxclass": "message", "numinlets": 1, "numoutlets": 1, "outlettype": [""], "patching_rect": [20, 193, 140, 22], "text": "send hello world"}},
            {"box": {"id": "msg-sensor", "maxclass": "message", "numinlets": 1, "numoutlets": 1, "outlettype": [""], "patching_rect": [20, 223, 160, 22], "text": "send sensor 42 3.14"}},
            {"box": {"fontname": "Arial", "fontsize": 10, "id": "c-enc", "maxclass": "comment", "numinlets": 1, "numoutlets": 0, "patching_rect": [200, 193, 300, 18], "text": "symbol(11B) + int64(8B) + double(8B) = 27 bytes"}},

            {"box": {"fontface": 1, "fontname": "Arial", "fontsize": 12, "id": "sec-mp", "maxclass": "comment", "numinlets": 1, "numoutlets": 0, "patching_rect": [20, 265, 230, 20], "text": "=== Multipart (click button) ==="}},
            {"box": {"id": "btn-mp", "maxclass": "button", "numinlets": 1, "numoutlets": 1, "outlettype": ["bang"], "patching_rect": [20, 288, 20, 20]}},
            {"box": {"id": "t-mp", "maxclass": "newobj", "numinlets": 1, "numoutlets": 3, "outlettype": ["bang", "bang", "bang"], "patching_rect": [20, 316, 42, 22], "text": "t b b b"}},
            {"box": {"id": "msg-f1", "maxclass": "message", "numinlets": 1, "numoutlets": 1, "outlettype": [""], "patching_rect": [20, 345, 95, 22], "text": "frame /topic"}},
            {"box": {"id": "msg-f2", "maxclass": "message", "numinlets": 1, "numoutlets": 1, "outlettype": [""], "patching_rect": [130, 345, 100, 22], "text": "frame 99 1.23"}},
            {"box": {"id": "msg-flush", "maxclass": "message", "numinlets": 1, "numoutlets": 1, "outlettype": [""], "patching_rect": [245, 345, 50, 22], "text": "flush"}},
            {"box": {"fontname": "Arial", "fontsize": 10, "id": "c-mp", "maxclass": "comment", "numinlets": 1, "numoutlets": 0, "patching_rect": [310, 345, 200, 18], "text": "frame + frame + flush = multipart"}}
        ],
        "lines": [
            {"patchline": {"destination": ["sender", 0], "source": ["msg-start", 0]}},
            {"patchline": {"destination": ["sender", 0], "source": ["msg-stop", 0]}},
            {"patchline": {"destination": ["sender", 0], "source": ["msg-hello", 0]}},
            {"patchline": {"destination": ["sender", 0], "source": ["msg-sensor", 0]}},
            {"patchline": {"destination": ["t-mp", 0], "source": ["btn-mp", 0]}},
            {"patchline": {"destination": ["msg-f1", 0], "source": ["t-mp", 2]}},
            {"patchline": {"destination": ["msg-f2", 0], "source": ["t-mp", 1]}},
            {"patchline": {"destination": ["msg-flush", 0], "source": ["t-mp", 0]}},
            {"patchline": {"destination": ["sender", 0], "source": ["msg-f1", 0]}},
            {"patchline": {"destination": ["sender", 0], "source": ["msg-f2", 0]}},
            {"patchline": {"destination": ["sender", 0], "source": ["msg-flush", 0]}}
        ]
    }
}
