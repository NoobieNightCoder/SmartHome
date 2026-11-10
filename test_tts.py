from pathlib import Path
import torch
import torchaudio as ta

from chatterbox.mtl_tts import (
    ChatterboxMultilingualTTS,
    Conditionals
)

# Выбор устройства
device = "cuda" if torch.cuda.is_available() else "cpu"

# Пути к файлам
VOICE_REFERENCE = Path("voice_reference.wav")
VOICE_CONDITIONS = Path("voice_conds.pt")

# Загружаем именно Multilingual V3
model = ChatterboxMultilingualTTS.from_pretrained(
    device=device,
    t3_model="v3"
)

# Загружаем сохранённые характеристики голоса
if VOICE_CONDITIONS.exists():
    model.conds = Conditionals.load(
        VOICE_CONDITIONS,
        map_location=device
    ).to(device)

# Если характеристик ещё нет — извлекаем их из записи
else:
    model.prepare_conditionals(
        str(VOICE_REFERENCE),
        exaggeration=0.5
    )

    model.conds.save(VOICE_CONDITIONS)

# Тестовая генерация
text = "Здравствуйте. Я ваш голосовой помощник. Чем могу помочь?"

wav = model.generate(
    text,
    language_id="ru",
    cfg_weight=0.5,
    exaggeration=0.5
)

ta.save("test_voice.wav", wav, model.sr)

print("Генерация завершена!")