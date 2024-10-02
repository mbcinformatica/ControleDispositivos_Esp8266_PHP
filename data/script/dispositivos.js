if (!window.dispositivosController) {
  const dispositivosController = {
    initialize: function () {
      var inicializado = false;
      const containerDispositivos = document.querySelector(".js-container-dispositivos");

      function renderDispositivos(dispositivos) {
        containerDispositivos.innerHTML = "";
        const listaDispositivos = document.createElement("ul");

        dispositivos.forEach(function (dispositivo) {
          const itemDispositivo = document.createElement("li");
          itemDispositivo.id = `dispositivo-${dispositivo.pino}`;

          const spanNome = document.createElement("span");
          spanNome.textContent = dispositivo.nome;
          itemDispositivo.appendChild(spanNome);

          const divLamp = document.createElement("div");
          divLamp.classList.add("lamp");
          const imgLamp  = document.createElement("img");
          imgLamp.src = dispositivo.estado ? dispositivo.imageon : dispositivo.imageoff;
          imgLamp.width = dispositivo.largura;
          imgLamp.height = dispositivo.altura;
          imgLamp.className = "imgDispositivo";        
          imgLamp.style.padding = dispositivo.preenchimento;

          divLamp.appendChild(imgLamp);
          itemDispositivo.appendChild(divLamp);

          const imgBotao = document.createElement("img");
          imgBotao.src = dispositivo.estado ? "img/Botao_Desliga.png" : "img/Botao_Liga.png";
          imgBotao.width = 70;
          imgBotao.height = 70;
          imgBotao.className = "btnDispositivo";
          imgBotao.style.padding  = "30px 35px";

          // Adiciona o evento de clique somente se estiver na página dispositivos
          if (window.location.hash === '#dispositivos') {
            imgBotao.addEventListener("click", function () {
              enviarComando(dispositivo.pino);
            });
          }
          itemDispositivo.appendChild(imgBotao);
          listaDispositivos.appendChild(itemDispositivo);
        });

        containerDispositivos.appendChild(listaDispositivos);
      }

      function atualizarDispositivosHTML(dispositivos) {
        dispositivos.forEach(dispositivo => {
          const itemDispositivo = document.querySelector(`#dispositivo-${dispositivo.pino}`);
          if (!itemDispositivo) return;
          const imgDispositivo = itemDispositivo.querySelector(".imgDispositivo");
          const btnDispositivo = itemDispositivo.querySelector(".btnDispositivo");
          imgDispositivo.src = dispositivo.estado ? dispositivo.imageon : dispositivo.imageoff;
          btnDispositivo.src = dispositivo.estado ? "img/Botao_Desliga.png" : "img/Botao_Liga.png";
        });
      }

      function atualizarDispositivos() {
        fetch("/api/dispositivos")
          .then((response) => response.json())
          .then((data) => {
            let dispositivos = data.dispositivos;
            if (inicializado) {
              atualizarDispositivosHTML(dispositivos);
            } else {
              renderDispositivos(dispositivos);
              inicializado = true;
            }
          })
          .catch((error) => console.error("Erro ao buscar dispositivos:", error));
      }

      function enviarComando(pino) {
        fetch('/api/comando', {
          method: 'POST',
          headers: {
            'Content-Type': 'application/json'
          },
          body: JSON.stringify({
            pino: pino,
          })
        })
          .then(response => response.json())
          .then(() => atualizarDispositivos())
          .catch(error => console.error('Erro ao enviar comando:', error));
      }

      setInterval(atualizarDispositivos, 1500);
      atualizarDispositivos();
    }
  };

  // Certifique-se de que o controlador está acessível globalmente
  window.dispositivosController = dispositivosController;
}
