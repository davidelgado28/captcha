import os, subprocess

os.makedirs("assets/sounds", exist_ok=True)
for c in "abcdefghjkmnpqrstuvwxyz23456789":
    subprocess.run([
        "espeak", "-v", "pt", "-s", "130", "-w", f"assets/sounds/{c}.wav",
        c
    ], check=True)

print("Pronto! Rode o ffmpeg para converter para .pcm")
