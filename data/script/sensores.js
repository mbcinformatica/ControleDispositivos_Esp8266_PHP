if (!window.sensoresController) {
  const sensoresController = {
    initialize: function () {
      let inicializado = false;
      const containersensores = document.querySelector(".js-container-sensores");

      function rendersensores(sensores) {
        containersensores.innerHTML = "";
        const listasensores = document.createElement("ul");
        listasensores.classList.add("device");

        sensores.forEach(function (sensor) {
          const itemsensor = document.createElement("li");
          itemsensor.classList.add("Sensor");
          itemsensor.id = `sensores-${sensor.id}`;

          const spanNome = document.createElement("span");
          spanNome.textContent = sensor.nome;
          itemsensor.appendChild(spanNome);

          const divLamp = document.createElement("div");
          divLamp.classList.add("lamp");

          const imgLamp = document.createElement("img");
          imgLamp.src = sensor.imagen;
          imgLamp.width = 80;
          imgLamp.className = "imgsensor";
          divLamp.appendChild(imgLamp);
          itemsensor.appendChild(divLamp);

          const inputsensor = document.createElement("input");
          inputsensor.value = `${sensor.valor} ${sensor.unidade}`;
          inputsensor.size = 4;
          inputsensor.readOnly = true;
          itemsensor.appendChild(inputsensor);
          listasensores.appendChild(itemsensor);
        });
        containersensores.appendChild(listasensores);
      }

      function atualizarsensoresHTML(sensores) {
        sensores.forEach(sensor => {
          const itemsensor = document.querySelector(`#sensores-${sensor.id}`);
          if (!itemsensor) return;
          const imgsensor = itemsensor.querySelector(".imgsensor");
          imgsensor.src = sensor.imagen;
          const inputsensor = itemsensor.querySelector("input");
          inputsensor.value = `${sensor.valor} ${sensor.unidade}`;
          
          console.log(inputsensor);
       
          
        });
      }

      function atualizarsensores() {
        fetch("/api/sensores")
          .then((response) => response.json())
          .then((data) => {
            const sensores = data.sensor;

            if (inicializado) {
              atualizarsensoresHTML(sensores);
              console.log(sensores);
            } else {
              rendersensores(sensores);
              inicializado = true;
            }

          })
          .catch((error) => console.error("Erro ao buscar sensores:", error));
      }
      setInterval(atualizarsensores, 1500);
      atualizarsensores();
    }
  };

  // Certifique-se de que o controlador está acessível globalmente
  window.sensoresController = sensoresController;
}
