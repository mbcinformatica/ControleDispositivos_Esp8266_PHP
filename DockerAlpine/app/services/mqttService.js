const mqtt = require('mqtt');
const dispositivoService = require('./dispositivoService');

// Configurações do broker MQTT
const brokerUrl = 'mqtt://192.168.1.198';
const options = {
    username: 'root',
    password: '123456'
};

// Conectando ao broker
const client = mqtt.connect(brokerUrl, options);

// Quando conectado ao broker
client.on('connect', () => {
    console.log('Conectado ao broker MQTT!');
    client.subscribe('dispositivo/#', subscribeCallback);
    client.subscribe('cadastro/#', subscribeCallback);
});

function subscribeCallback(err) {
    if (err) {
        console.error('Falha ao assinar o tópico:', err);
    } else {
        console.log('Assinado o tópico dispositivo/#');
    }
}

// Quando uma mensagem é recebida
client.on('message', async (topic, message) => {
    console.log(`Mensagem recebida no tópico ${topic}: ${message.toString()}`);
    try {
        if (topic == "cadastro/dispositivo/lista") {
            let body = JSON.parse(message);
            await dispositivoService.sincronizarDispositivos(body.devices);
        }
    } catch (error) {
        console.error('Erro ao salvar dispositivo:', error);
    }
});

// Tratamento de erro
client.on('error', (err) => {
    console.error('Erro no cliente MQTT:', err);
});

function publish(topic, message) {
    client.publish(topic, message, (err) => {
        if (err) {
            console.error('Falha ao publicar no tópico:', topic, err);
        } else {
            console.log('Publicado com sucesso no tópico:', topic);
        }
    });
}

module.exports = {
    publish
};