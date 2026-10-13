
import io
import threading

import soundfile as sf
import torch

from fastapi import FastAPI, HTTPException
from pydantic import BaseModel
from fastapi.responses import Response
from chatterbox.mtl_tts import ChatterboxMultilingualTTS


app = FastAPI(title="Local TTS Service")

device = "cuda" if torch.cuda.is_available() else "cpu"

print(f"Loading Chatterbox V3 on {device}...")
model = ChatterboxMultilingualTTS.from_pretrained(
    device=device,
    t3_model="v3",
)
print("TTS model is ready.")

generation_lock = threading.Lock()


class TTSRequest(BaseModel):
    text: str
    language_id: str = "ru"


@app.get("/health")
def health():
    return {"status": "ready", "device": device}


@app.post("/tts")
def synthesize(request: TTSRequest):
    text = request.text.strip()

    if not text:
        raise HTTPException(status_code=400, detail="Text is empty")

    if len(text) > 1000:
        raise HTTPException(
            status_code=400,
            detail="Text is too long"
        )

    if request.language_id != "ru":
        raise HTTPException(
            status_code=400,
            detail="Only Russian is enabled"
        )

    try:
        with generation_lock, torch.inference_mode():
            wav = model.generate(
                text,
                language_id="ru",
            )

        audio = wav.squeeze().float().cpu().numpy()

        buffer = io.BytesIO()
        sf.write(buffer, audio, model.sr, format="WAV")

        return Response(
            content=buffer.getvalue(),
            media_type="audio/wav",
        )

    except Exception as error:
        print(f"TTS error: {error}")
        raise HTTPException(
            status_code=500,
            detail="Speech generation failed",
        )


if __name__ == "__main__":
    import uvicorn

    uvicorn.run(
        app,
        host="127.0.0.1",
        port=8765,
    )
