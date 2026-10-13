import torch
import torchaudio as ta

from chatterbox.mtl_tts import ChatterboxMultilingualTTS


def main():
    device = "cuda" if torch.cuda.is_available() else "cpu"
    print(f"Загрузка Chatterbox Multilingual V3 на {device}...")

    model = ChatterboxMultilingualTTS.from_pretrained(
        device=device,
        t3_model="v3",
    )

    text = (
        "Привет! Это проверка синтеза речи. "
        "Если ты слышишь эту фразу, значит модель успешно запустилась. "
        "Сегодня температура двадцать три градуса."
    )

    print("Генерация речи...")
    with torch.inference_mode():
        wav = model.generate(
            text,
            language_id="ru",
        )

    output_path = "test_output.wav"
    ta.save(output_path, wav.cpu(), model.sr)

    print(f"Готово: {output_path}")


if __name__ == "__main__":
    main()
