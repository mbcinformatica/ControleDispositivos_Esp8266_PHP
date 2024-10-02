if (!window.homeController) {
    const homeController = {
    initialize: function() {
      // Inicializar dispositivos
      if (window.dispositivosController && typeof window.dispositivosController.initialize === "function") {
        window.dispositivosController.initialize();
      }

      // Inicializar sensores
      if (window.sensoresController && typeof window.sensoresController.initialize === "function") {
        window.sensoresController.initialize();
      }
    }
  };

  // Certifique-se de que o controlador está acessível globalmente
  window.homeController = homeController;
}
