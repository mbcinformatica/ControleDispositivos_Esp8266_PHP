#include <DHT.h>
#include <DHT_U.h>
#include <ArduinoJson.h>

#define DHTPIN 5          // D1
#define DHTTYPE DHT11     // DHT 11
#define PinSensorChamas 4 // D2

DHT dht(DHTPIN, DHTTYPE);

struct DeviceSensor
{
    const char *idtipo;
    const char *name;
    const char *unit;
    const char *imgon;
    const char *imgoff;
    String (*getValue)();
    String identifier;

};

const int nArraySensor = 3;

void inserirSensor(DeviceSensor *devicesensor);
void inserirHistoricoSensor(DeviceSensor *devicesensor);

String getTemperatura();
String getHumidity();
String getChamas();

DeviceSensor DeviceSensorList[nArraySensor] = {
    {"temperatura", "Sensor Temperatura", "°C", "img/Termometro_01.png", "", getTemperatura, "sensor_temperatura"},
    {"umidade", "Sensor Umidade", "%", "img/Umidade.png", "", getHumidity, "sensor_umidade"},
    {"chamas", "Sensor Chamas", "", "img/Chamas_On.gif", "img/Chamas_Off.gif", getChamas, "sensor_chamas"}};

String getTemperatura()
{
    float temperatura = dht.readTemperature();
    if (isnan(temperatura))
    {
        return "0.00"; // Retorna 0.00 se a leitura for inválida
    }
    return String(temperatura, 2); // Retorna com 2 casas decimais
}

String getHumidity()
{
    float umidade = dht.readHumidity();
    if (isnan(umidade))
    {
        return "0"; // Retorna 0 se a leitura for inválida
    }
    return String((int)umidade); // Retorna sem casas decimais
}

String getChamas()
{
    int chamas = digitalRead(PinSensorChamas);
    return (chamas == 1) ? "Normal" : "Anormal"; // Retorna "Normal" ou "Anormal"
}

const char *getImagem(const char *valor, const char *imgon, const char *imgoff)
{
    if (strcmp(valor, "Normal") == 0)
    {
        return imgoff;
    }
    else if (strcmp(valor, "Anormal") == 0)
    {
        return imgon;
    }
    else
    {
        return imgon; // Retorna imgon para valores diferentes de "Normal" ou "Anormal"
    }
}
DynamicJsonDocument getSensorData()
{
    DynamicJsonDocument jsonBuffer(1024);
    JsonArray sensor = jsonBuffer.createNestedArray("sensor");

    for (int i = 0; i < nArraySensor; i++)
    {

        JsonObject Sensor = sensor.createNestedObject();
        Sensor["id"] = DeviceSensorList[i].idtipo;
        Sensor["nome"] = DeviceSensorList[i].name;
        Sensor["unidade"] = DeviceSensorList[i].unit;
        Sensor["valor"] = DeviceSensorList[i].getValue();
        Sensor["imagen"] = getImagem(Sensor["valor"], DeviceSensorList[i].imgon, DeviceSensorList[i].imgoff);
    }
    return jsonBuffer;
}

void setupSensor()
{
    dht.begin();
    pinMode(PinSensorChamas, INPUT);
    for (int i = 0; i < nArraySensor; i++)
    {
        inserirSensor(&DeviceSensorList[i]);
    }
}

void atualizaHistoricoSensor()
{
    for (int i = 0; i < nArraySensor; i++)
    {
        inserirHistoricoSensor(&DeviceSensorList[i]);
    }
}

void inserirSensor(DeviceSensor *devicesensor)
{
    HTTPClient http;
    String urlSensor = String(site_url) + "?action=inserir-sensor&name=" + String(urlencode(devicesensor->name)) +
                       "&unit=" + String(urlencode(devicesensor->unit)) +
                       "&imgon=" + String(urlencode(devicesensor->imgon)) +
                       "&imgoff=" + String(urlencode(devicesensor->imgoff)) +
                       "&identifier=" + String(devicesensor->identifier);

    http.begin(wifiClient, urlSensor);
    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.GET();

    // Verifica a resposta do servidor
    if (httpResponseCode > 0)
    {
        String response = http.getString(); // Resposta do servidor
        Serial.println(httpResponseCode);   // Código de resposta
        Serial.println(response);           // Resposta do servidor
        Serial.println(urlSensor);          // Resposta do servidor
    }
    else
    {
        Serial.print("Erro ao enviar: ");
        Serial.println(httpResponseCode);
    }

    // Finaliza a conexão
    http.end();
}

void inserirHistoricoSensor(DeviceSensor *devicesensor)
{

    HTTPClient http;

    String urlSensor = String(site_url) + "?action=inserir-historico-sensor&identifier=" + String(devicesensor->identifier) + "&valor=" + String(urlencode(devicesensor->getValue()));
    Serial.println(urlSensor); // Resposta do servidor

    http.begin(wifiClient, urlSensor);
    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.GET();

    // Verifica a resposta do servidor
    if (httpResponseCode > 0)
    {
        String response = http.getString(); // Resposta do servidor
        Serial.println(httpResponseCode);   // Código de resposta
        Serial.println(response);           // Resposta do servidor
        Serial.println(urlSensor);          // Resposta do servidor
    }
    else
    {
        Serial.print("Erro ao enviar: ");
        Serial.println(httpResponseCode);
    }

    // Finaliza a conexão
    http.end();
}
