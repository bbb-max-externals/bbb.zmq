{
    "patcher": {
        "fileversion": 1,
        "appversion": {
            "major": 9, "minor": 1, "revision": 3,
            "architecture": "x64", "modernui": 1
        },
        "classnamespace": "box",
        "rect": [50, 80, 650, 430],
        "bglocked": 1,
        "boxes": [
            {"box": {
                "id": "title", "maxclass": "comment",
                "text": "bbb.zmq.peek",
                "numinlets": 1, "numoutlets": 0,
                "patching_rect": [30, 20, 200, 24],
                "fontsize": 18, "fontname": "Arial", "fontface": 1
            }},
            {"box": {
                "id": "desc", "maxclass": "comment",
                "text": "Debug inspector for packet views.\nShows frame count, text preview, and hex dumps.\nConnect anywhere in a packet pipeline to inspect what's flowing.",
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
                "id": "peek", "maxclass": "newobj", "text": "bbb.zmq.peek",
                "numinlets": 1, "numoutlets": 1,
                "patching_rect": [30, 175, 100, 22],
                "outlettype": [""]
            }},
            {"box": {
                "id": "pr", "maxclass": "newobj", "text": "print peek",
                "numinlets": 1, "numoutlets": 0,
                "patching_rect": [30, 210, 70, 22]
            }},

            {"box": {"id": "sec-verbose", "maxclass": "comment", "text": "--- @verbose Levels ---", "numinlets": 1, "numoutlets": 0, "patching_rect": [200, 140, 170, 20], "fontsize": 12, "fontname": "Arial", "fontface": 1}},
            {"box": {
                "id": "c-v0", "maxclass": "comment",
                "text": "@verbose 0 — Summary only: view_id, packet_id, frame count",
                "numinlets": 1, "numoutlets": 0,
                "patching_rect": [200, 162, 400, 20], "fontsize": 11, "fontname": "Arial"
            }},
            {"box": {
                "id": "c-v1", "maxclass": "comment",
                "text": "@verbose 1 — + Frame preview: size, text or <binary N bytes>",
                "numinlets": 1, "numoutlets": 0,
                "patching_rect": [200, 182, 420, 20], "fontsize": 11, "fontname": "Arial"
            }},
            {"box": {
                "id": "c-v2", "maxclass": "comment",
                "text": "@verbose 2 — + Hex dump: first 64 bytes per frame (0–255 ints)",
                "numinlets": 1, "numoutlets": 0,
                "patching_rect": [200, 202, 420, 20], "fontsize": 11, "fontname": "Arial"
            }},

            {"box": {"id": "sec-out", "maxclass": "comment", "text": "--- Output Example (verbose 1) ---", "numinlets": 1, "numoutlets": 0, "patching_rect": [30, 250, 280, 20], "fontsize": 12, "fontname": "Arial", "fontface": 1}},
            {"box": {
                "id": "c-out", "maxclass": "comment",
                "text": "peek view_id 1 packet_id 42 start_frame 0 frames 3\nframe 0 size 5 /imu\nframe 1 size 24 <binary 24 bytes>\nframe 2 size 8 <binary 8 bytes>",
                "numinlets": 1, "numoutlets": 0,
                "patching_rect": [30, 270, 350, 74], "fontsize": 11, "fontname": "Arial", "linecount": 4
            }},

            {"box": {"id": "sec-attr", "maxclass": "comment", "text": "--- Attribute ---", "numinlets": 1, "numoutlets": 0, "patching_rect": [420, 270, 130, 20], "fontsize": 12, "fontname": "Arial", "fontface": 1}},
            {"box": {
                "id": "c-attr", "maxclass": "comment",
                "text": "@verbose 1  (default)\nVerbosity: 0, 1, or 2",
                "numinlets": 1, "numoutlets": 0,
                "patching_rect": [420, 290, 200, 34], "fontsize": 11, "fontname": "Arial", "linecount": 2
            }},

            {"box": {"id": "sec-io", "maxclass": "comment", "text": "--- Inlet / Outlet ---", "numinlets": 1, "numoutlets": 0, "patching_rect": [30, 360, 170, 20], "fontsize": 12, "fontname": "Arial", "fontface": 1}},
            {"box": {
                "id": "c-inlet", "maxclass": "comment",
                "text": "inlet 0: packet <view_id>",
                "numinlets": 1, "numoutlets": 0,
                "patching_rect": [30, 380, 200, 20], "fontsize": 11, "fontname": "Arial"
            }},
            {"box": {
                "id": "c-outlet", "maxclass": "comment",
                "text": "outlet 0: debug messages (peek, frame, hex)",
                "numinlets": 1, "numoutlets": 0,
                "patching_rect": [30, 400, 300, 20], "fontsize": 11, "fontname": "Arial"
            }}
        ],
        "lines": [
            {"patchline": {"source": ["msg-pkt", 0], "destination": ["peek", 0]}},
            {"patchline": {"source": ["peek", 0], "destination": ["pr", 0]}}
        ]
    }
}
