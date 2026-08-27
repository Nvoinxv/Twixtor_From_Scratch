from fastapi import FastAPI, UploadFile, File
import shutil

app = FastAPI()

@app.post("/upload")
async def upload_video(video: UploadFile = File(...)):

    path = f"uploads/{video.filename}"

    with open(path, "wb") as buffer:
        shutil.copyfileobj(video.file, buffer)

    return {
        "message": "Video berhasil diupload",
        "filename": video.filename
    }