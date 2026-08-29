from fastapi import FastAPI, UploadFile, File
from typing import List
import shutil
import os
import subprocess

app = FastAPI()

# Memastikan folder uploads tersedia
UPLOAD_DIR = "uploads"
os.makedirs(UPLOAD_DIR, exist_ok=True)

@app.post("/upload")
async def upload_video(videos: List[UploadFile] = File(...)):
    hasil_upload = []

    for video in videos:
        path = os.path.join(UPLOAD_DIR, video.filename)

        with open(path, "wb") as buffer:
            shutil.copyfileobj(video.file, buffer)

        hasil_upload.append(video.filename)

    return {
        "message": f"{len(hasil_upload)} video berhasil diupload",
        "files": hasil_upload
    }

@app.get("/list_video")
async def list_video():
    files = [f for f in os.listdir(UPLOAD_DIR) if os.path.isfile(os.path.join(UPLOAD_DIR, f))]
    return {
        "total": len(files),
        "files": files
    }

@app.post("/proses_otomatis")
async def proses_otomatis_cpp(nama_video: str):
    path = os.path.join(UPLOAD_DIR, nama_video)
    if not os.path.exists(path):
        return {"status": "gagal", "error": "File video tidak ditemukan"}

    # Panggil executable C++ Twixtor jika tersedia
    cmd = ["../build/Twixtor_From_Scratch_Cpp", path]
    try:
        proses = subprocess.run(cmd, capture_output=True, text=True)
        return {
            "status": "sukses",
            "output": proses.stdout
        }
    except Exception as e:
        return {
            "status": "info",
            "message": f"Video {nama_video} siap diproses oleh C++ Twixtor",
            "path": path
        }