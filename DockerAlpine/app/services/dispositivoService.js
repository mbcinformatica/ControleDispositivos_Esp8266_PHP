const { Dispositivo } = require('../models/dispositivo');

async function findOrCreateDevice(topic, name) {
    try {       
        // Encontra ou cria um dispositivo baseado no tópico
        const [dispositivo, created] = await Dispositivo.findOrCreate({
            where: { topic: topic },
            defaults: { name: name }
        });
        
        return dispositivo;
    } catch (error) {
        console.error('Erro ao encontrar ou criar dispositivo:', error);
        throw error;
    }
}

async function sincronizarDispositivos(dispositivos) {
    try {
        for (const dispositivo of dispositivos) {
            await findOrCreateDevice(dispositivo.topic, dispositivo.name);
        }
    } catch (error) {
        console.error('Erro ao sincronizar dispositivos:', error);
        throw error;
    }
}

module.exports = {
    findOrCreateDevice,
    sincronizarDispositivos
};
