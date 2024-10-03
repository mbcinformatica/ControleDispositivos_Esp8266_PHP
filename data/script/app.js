document.addEventListener("DOMContentLoaded", () => {
  const app = document.getElementById("app");
  const navLinks = document.querySelectorAll(".topnav a");

  function navigate() {
    
    const hash = window.location.hash || "#home";
    const page = hash.substring(1); // Remove the '#' from the hash

    fetch(`pages/${page}.html`)
      .then((response) => {
        if (!response.ok) throw new Error("Page not found");
        return response.text();
      })
      .then((html) => {
        app.innerHTML = html;
        setActiveLink();
        // Clear previously loaded scripts
        clearScripts();
        // Load new scripts for the current page
        loadScripts(page);
      })
      .catch(() => {
        app.innerHTML = "<h1>404 - Page Not Found</h1>";
        setActiveLink();
      });
  }

  function setActiveLink() {
    const hash = window.location.hash || "#home";
    navLinks.forEach((link) => {
      if (link.getAttribute("href") === hash) {
        link.classList.add("active");
      } else {
        link.classList.remove("active");
      }
    });
  }

  function loadScripts(page) {
    const scriptNames = {
      home: ["home.js", "dispositivos.js", "sensores.js"],
      dispositivos: ["dispositivos.js"],
      sensores: ["sensores.js"],
      caddispositivos: ["caddispositivos.js"],
    };

    const scriptsToLoad = scriptNames[page] || [];
    const scripts = [];

    scriptsToLoad.forEach((scriptName) => {
      const script = document.createElement("script");
      script.src = `script/${scriptName}`;
      script.defer = true;
      document.body.appendChild(script);

      // After the script loads, call the pageController.initialize function
      script.onload = function () {
        const controllerName = `${page}Controller`;
        if (
          window[controllerName] &&
          typeof window[controllerName].initialize === "function"
        ) {
          window[controllerName].initialize();
        }
      };
    });

    return scripts;
  }

  function clearScripts() {
    const existingScripts = document.querySelectorAll('script[src^="script/"]');
    existingScripts.forEach((script) => script.remove());
  }

  window.addEventListener("hashchange", navigate);
  navigate(); // Initial load
});
