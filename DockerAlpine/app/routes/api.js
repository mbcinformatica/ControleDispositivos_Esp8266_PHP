const express = require('express');
const router = express.Router();
const { Dispositivo } = require('../models/dispositivo');
const mqttService = require('../services/mqttService');

// Exemplo de rota GET
router.get('/hello', (req, res) => {
    try {
        let topico = req.query.topic;
        let estado = req.query.estado;
        
        const dispositivo = Dispositivo.findOne({
            where: { topic: topico }
        });
        console.log(dispositivo)
        mqttService.publish(dispositivo.topic, estado);


        res.json({ message: dispositivo });
    } catch (e) {
        console.log(e)
    }
});

module.exports = router;
