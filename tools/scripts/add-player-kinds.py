#!/usr/bin/env python3
"""
Adds a "kind: user|buddy" line to every player in a players.yaml that does
not have one, so the file loads with the current royalcraps.

Players delivered with the game (arthur, paige, lenny, winnifred) become
buddy players. Everyone else becomes a user player. A backup is kept as
players.yaml.bak.

Usage: add-player-kinds.py ~/.local/share/royalcraps/players/players.yaml
"""
import re
import shutil
import sys

BUDDIES = {"arthur.yaml", "paige.yaml", "lenny.yaml", "winnifred.yaml"}


def main(path):
    text = open(path).read()
    blocks = re.split(r"(?=^  - playerId:)", text, flags=re.M)
    out, added = [blocks[0]], 0
    for b in blocks[1:]:
        if re.search(r"^\s+kind:", b, flags=re.M):
            out.append(b)
            continue
        m = re.search(r"^\s+fileName:\s*(\S+)", b, flags=re.M)
        kind = "buddy" if m and m.group(1) in BUDDIES else "user"
        b = re.sub(r"^(  - playerId:[^\n]*\n)", r"\1    kind: " + kind + "\n",
                   b, count=1, flags=re.M)
        out.append(b)
        added += 1
    if added:
        shutil.copyfile(path, path + ".bak")
        open(path, "w").write("".join(out))
    print(f"{path}: added kind to {added} player(s)")


if __name__ == "__main__":
    if len(sys.argv) != 2:
        sys.exit(__doc__)
    main(sys.argv[1])
