#include <Arduino.h>
#include <FS.h>          // Biblioteca para o sistema de arquivos SPIFFS
#include <ESP8266WiFi.h> // Biblioteca para WiFi
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h> // Biblioteca para servidor web
#include <WiFiManager.h>      // Biblioteca para gerenciamento de WiFi
#include <ESP8266mDNS.h>      // Biblioteca para mDNS
#include <ArduinoJson.h>

WiFiClient wifiClient;
char *site_url = "http://192.168.1.198:8080/api.php";

#include <device.h>
#include <sensor.h>

IPAddress staticIP(192, 168, 1, 155); // IP desejado
IPAddress gateway(192, 168, 1, 100);  // Gateway
IPAddress subnet(255, 255, 255, 0);   // Máscara de sub-rede
IPAddress webServer(192, 168, 1, 198);
unsigned long previousMillis = 0;  // Armazena o último tempo em que a ação foi executada
const long interval = 10000;        // Intervalo desejado (em milissegundos)
ESP8266WebServer server(80); // Cria um servidor na porta 80

// Função para lidar com a requisição raiz
void handleRoot()
{

    server.sendHeader("Location", "/index.html");
    server.send(302, "text/plain", "");
}

void handleComando()
{
    if (server.hasArg("plain"))
    {
        String body = server.arg("plain");
        DynamicJsonDocument jsonBuffer(1024);
        DeserializationError error = deserializeJson(jsonBuffer, body);
        if (error)
        {
            server.send(400, "application/json", "{\"status\":\"erro\",\"mensagem\":\"JSON inválido\"}");
            return;
        }
        int pino = jsonBuffer["pino"];
        for (int i = 0; i < nArrayDispositivos; i++)
        {
            if (deviceList[i].pin == pino)
            {
                toggleDeviceState(&deviceList[i]);
                inserirHistoricoDispositivo(&deviceList[i]);
            }
        }
        server.send(200, "application/json", "{\"status\":\"sucesso\",\"mensagem\":\"Comando recebido\"}");
    }
    else
    {
        server.send(400, "application/json", "{\"status\":\"erro\",\"mensagem\":\"Requisição sem corpo\"}");
    }
}

void handleDispositivos()
{
    String response;
    DynamicJsonDocument jsonBuffer = getDeviceData();
    serializeJson(jsonBuffer, response);
    server.send(200, "application/json", response);
}

void handleSensores()
{
    String response;
    DynamicJsonDocument jsonBuffer = getSensorData();
    serializeJson(jsonBuffer, response);
    server.send(200, "application/json", response);
}

void ReconfiguraWiFi();
void saveConfigCallback();
void configModeCallback(WiFiManager *myWiFiManager);
void atualizaHistoricoSensor();

void setup()
{
    Serial.begin(115200);

    // Inicializa o SPIFFS
    if (!SPIFFS.begin())
    {
        Serial.println("Erro ao montar o sistema de arquivos SPIFFS");
        return;
    }

    WiFi.config(staticIP, gateway, subnet);
    // Inicializa o WiFiManager e tenta conectar
    WiFiManager wifiManager;
    wifiManager.setConfigPortalTimeout(60);
    wifiManager.setAPCallback(configModeCallback);
    wifiManager.setSaveConfigCallback(saveConfigCallback);
    wifiManager.autoConnect("ESP8266_AP", "123456789");

    // Inicializa o mDNS
    if (!MDNS.begin("dispositivos"))
    {
        Serial.println("Erro ao configurar nDNS up MDNS responder!");
        while (1)
        {
            delay(100);
        }
    }
    Serial.println("Conectado à rede WiFi!");
    Serial.println("mDNS configurado. Acesse http://dispositivos.local");
    MDNS.addService("http", "tcp", 80);

    if (wifiClient.connect(webServer, 8080))
    {
        setupSensor();
        setupDevices();
    }
    // Configura o servidor web
    server.on("/", handleRoot);
    server.on("/api/sensores", HTTP_GET, handleSensores);
    server.on("/api/dispositivos", HTTP_GET, handleDispositivos);
    server.on("/api/comando", HTTP_POST, handleComando);
    server.serveStatic("/", SPIFFS, "/");
    server.begin();
    Serial.println("Servidor HTTP iniciado");
}

void loop()
{
    MDNS.update();
    if (WiFi.status() == WL_CONNECTED)
    {
        server.handleClient();
    }
    else
    {
        ReconfiguraWiFi();
    }
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
        // Salva o tempo atual
        previousMillis = currentMillis;

        // Ação a ser executada
        atualizaHistoricoSensor();
  }
    

}

void ReconfiguraWiFi()
{
    WiFiManager wifiManager;
    Serial.println(WiFi.status());
    if (WiFi.status() != WL_CONNECTED)
    {
        if (WiFi.status() == WL_NO_SSID_AVAIL || WiFi.status() == WL_CONNECT_FAILED)
        {
            wifiManager.setConfigPortalTimeout(60);
            if (!wifiManager.startConfigPortal("ESP8266_AP", "123456789"))
            {
                Serial.println("Falha ao iniciar o portal de configuração");
            }
        }
    }
}

void saveConfigCallback()
{
    Serial.println("Configuração Salva");
    Serial.println(WiFi.softAPIP());
}

void configModeCallback(WiFiManager *myWiFiManager)
{
    Serial.println("Entrou no Modo de Configuração!!!");
    Serial.println(WiFi.softAPIP());
    Serial.println(myWiFiManager->getConfigPortalSSID());
}