 # Прошивка для платы HighBuoy

Подробнее о наборе: [Начало работы с HighBuoy](https://wiki.murproject.com/ru/HighBuoy/highbuoy-instruction)

## Установка библиотек

> [!WARNING] 
> При разработке прошивки использовались указанные версии библиотек и пакетов.
**С другими версиями работоспособность не проверялась!**

Для прошивки потребуются библиотеки:
|Библиотека|Автор|Версия|
|-|-|-|
|[**RTClib**](https://github.com/adafruit/RTClib)|Adafruit|2.1.4|
|[**Adafruit BusIO**](https://github.com/adafruit/Adafruit_BusIO)|Adafruit|1.17.0|
|[**Adafruit NeoPixel**](https://github.com/adafruit/Adafruit_NeoPixel)|Adafruit|1.12.5|
|[**ArduinoJson**](https://github.com/bblanchon/ArduinoJson)|Benoit Blanchon|7.3.1|
|[**sMQTTBroker**](https://github.com/terrorsl/sMQTTBroker)|Vyacheslav Shiryaev|0.1.8|

Чтобы установить эти библиотеки:
1. Откройте менеджер библиотек в Arduino IDE
2. Нажмите: Скетч → Подключить библиотеку → Управлять библиотеками. 
3. Введите в поиск **RTClib**, выберите версию библиотеки указанную выше и установите библиотеку. 
4. Аналогично выполните установку для остальных библиотек.

## Загрузка прошивки

1. Установите в менеджере плат пакет [esp32](https://github.com/espressif/arduino-esp32) версии 3.1.1. 
2. Затем подключите USB-кабель к плате и запустите **Arduino IDE**. 
3. Выберите нужный последовательный порт и плату **ESP32S3 Dev Module**.
4. Нажмите "Загрузить".