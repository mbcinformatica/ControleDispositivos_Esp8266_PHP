#include <ArduinoJson.h>

struct Device
{
    int pin;
    int state;
    const char *name;
    const char *imgon;
    const char *imgoff;
    int widthimg;
    int heightimg;
    String paddingimg;
    String identifier;
};

const int ledIndex = 3;
bool lastButtonState = false;
bool currentButtonState = false;
const int nArrayDispositivos = 6;

void setupDevices();
void inserirDispositivo(Device *device);
void inserirHistoricoDispositivo(Device *device);

Device deviceList[nArrayDispositivos] = {
    //{Pino-Arduino, Estado-Pino, "Nome Dispositivo", "Imagem Disp. On", "Imagem Disp. Off", Largura, Altura, "Preenchimento"}
    {12, false, "Lâmpada Cozinha", "img/Lampada_On.png", "img/Lampada_Off.png", 80, 80, "9px 0px", "lampada_cozinha"},
    {13, false, "Lâmpada Sala", "img/Lampada_On.png", "img/Lampada_Off.png", 80, 80, "9px 0px", "lampada_sala"},
    {14, false, "Lâmpada Garagem", "img/Lampada_On.png", "img/Lampada_Off.png", 80, 80, "9px 0px", "lampada_garagem"},
    {15, false, "Lâmpada Suite Casal", "img/Lampada_On.png", "img/Lampada_Off.png", 80, 80, "9px 0px", "lampada_suite_casal"},
    {16, false, "Aquecedor", "img/Aquec_On.png", "img/Aquec_Off.png", 70, 70, "9px 0px", "aquecedor"},
    {2, false, "Ventilador", "img/Ventilador_On.gif", "img/Ventilador_Off.gif", 80, 80, "9px 0px", "ventilador"}};

DynamicJsonDocument getDeviceData()
{
    DynamicJsonDocument jsonBuffer(1024);
    JsonArray dispositivos = jsonBuffer.createNestedArray("dispositivos");

    for (int i = 0; i < nArrayDispositivos; i++)
    {
        JsonObject dispositivo = dispositivos.createNestedObject();
        dispositivo["pino"] = deviceList[i].pin;
        dispositivo["estado"] = deviceList[i].state;
        dispositivo["nome"] = deviceList[i].name;
        dispositivo["imageon"] = deviceList[i].imgon;
        dispositivo["imageoff"] = deviceList[i].imgoff;
        dispositivo["largura"] = deviceList[i].widthimg;
        dispositivo["altura"] = deviceList[i].heightimg;
        dispositivo["preenchimento"] = deviceList[i].paddingimg;
    }
    return jsonBuffer;
}

String urlencode(const String &str)
{
    String encoded = "";
    for (size_t i = 0; i < str.length(); i++)
    {
        char c = str.charAt(i);
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~')
        {
            // Caracteres seguros
            encoded += c;
        }
        else if (c < 0)
        {
            // Codifica caracteres UTF-8
            encoded += "%";
            encoded += String((unsigned char)c, HEX);
        }
        else
        {
            // Para caracteres especiais
            encoded += "%";
            encoded += String((unsigned char)c, HEX);
        }
    }
    return encoded;
}

void toggleDeviceState(Device *device)
{
    device->state = !device->state;
    digitalWrite(device->pin, device->state);
}

void setupDevices()
{
    for (int i = 0; i < nArrayDispositivos; i++)
    {
        pinMode(deviceList[i].pin, OUTPUT);
        digitalWrite(deviceList[i].pin, deviceList[i].state);
        inserirDispositivo(&deviceList[i]);
    }
}

void inserirDispositivo(Device *device)
{
    HTTPClient http;
    String urlDispositivo = String(site_url) + "?action=inserir-dispositivo&name=" + String(urlencode(device->name)) + 
    "&pin=" + (device->pin) +
    "&imgon=" + String(urlencode(device->imgon)) +
    "&imgoff=" + String(urlencode(device->imgoff)) +
    "&widthimg=" + (device->widthimg) +
    "&heightimg=" + (device->heightimg) +
    "&paddingimg=" + String(urlencode(device->paddingimg)) +
    "&identifier=" + String(device->identifier);

    http.begin(wifiClient, urlDispositivo);
    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.GET();

    // Verifica a resposta do servidor
    if (httpResponseCode > 0)
    {
        String response = http.getString(); // Resposta do servidor
        Serial.println(httpResponseCode);   // Código de resposta
        Serial.println(response);           // Resposta do servidor
        Serial.println(urlDispositivo);           // Resposta do servidor

    }
    else
    {
        Serial.print("Erro ao enviar: ");
        Serial.println(httpResponseCode);
    }

    // Finaliza a conexão
    http.end();
}

void inserirHistoricoDispositivo(Device *device)
{
    HTTPClient http;
    String urlDispositivo = String(site_url) + "?action=inserir-historico-dispositivo&identifier=" + "&identifier=" + String(device->identifier) + "&state=" + (device->state);

    http.begin(wifiClient, urlDispositivo);
    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.GET();

    // Verifica a resposta do servidor
    if (httpResponseCode > 0)
    {
        String response = http.getString(); // Resposta do servidor
        Serial.println(httpResponseCode);   // Código de resposta
        Serial.println(response);           // Resposta do servidor
    }
    else
    {
        Serial.print("Erro ao enviar: ");
        Serial.println(httpResponseCode);
    }

    // Finaliza a conexão
    http.end();
}
