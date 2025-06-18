### RHVoice-api

- [RHVoice](https://github.com/RHVoice/RHVoice)

```bash
apt install cmake g++ libssl-dev libcpprest-dev libboost-system-dev
```

```bash
mkdir -p build && cd build

cmake ..

make

ldconfig
```

---

``` http
POST http://ip:8000/api/v1/tts
```

#### Запрос:

```json
{
  "text": "Привет мир",
  "voice": "Anna"
}
```

#### Ответ: .wav-файл (MIME: audio/wav)