// ===== Página web do RoboLight (fica guardada na memória flash) =====
// Controles: 4 direções (segure para andar, solte para parar), barra de velocidade e LED
const char webpageCode[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="pt">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>RoboLight - Módulo Clínico</title>
  <style>
    /* ---------- Cores usadas na página (mude aqui para trocar o tema) ---------- */
    :root {
      --bg-color: #f1f5f9;       /* fundo da página */
      --panel-bg: #ffffff;       /* fundo dos cartões */
      --header-bg: #0f172a;      /* azul escuro da barra e dos botões */
      --primary-blue: #0284c7;   /* azul de destaque */
      --primary-hover: #0369a1;  /* azul ao passar o mouse */
      --primary-active: #0c4a6e; /* azul ao apertar */
      --medical-green: #10b981;  /* verde (conectado / LED ligado) */
      --medical-red: #dc2626;    /* vermelho (desconectado) */
      --text-dark: #1e293b;      /* texto principal */
      --text-muted: #64748b;     /* texto secundário */
      --border-color: #cbd5e1;   /* bordas */
      --btn-bg: #f8fafc;         /* fundo do botão do LED */
    }

    /* Zera margens e usa a mesma fonte em tudo; user-select evita selecionar texto ao segurar o botão */
    * {
      box-sizing: border-box;
      margin: 0;
      padding: 0;
      font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, Arial, sans-serif;
      user-select: none;
    }

    /* Corpo da página: coluna centralizada */
    body {
      background-color: var(--bg-color);
      color: var(--text-dark);
      display: flex;
      flex-direction: column;
      align-items: center;
      min-height: 100vh;
      padding-bottom: 20px;
    }

    /* ---------- Barra superior ---------- */
    .top-bar {
      width: 100%;
      background-color: var(--header-bg);
      color: #ffffff;
      padding: 12px 20px;
      display: flex;
      justify-content: space-between; /* logo à esquerda, status à direita */
      align-items: center;
      box-shadow: 0 2px 4px rgba(0,0,0,0.1);
      margin-bottom: 20px;
    }

    .brand-icon {
      color: var(--primary-blue);
      font-size: 1.5rem;
      font-weight: bold;
    }

    /* Etiqueta "Conectado / Desconectado" */
    .status-badge {
      display: inline-flex;
      align-items: center;
      gap: 6px;
      font-size: 0.85rem;
      background: rgba(255,255,255,0.1);
      padding: 4px 10px;
      border-radius: 20px;
    }

    /* Bolinha do status: vermelha por padrão, verde quando tem a classe "online" */
    .status-dot {
      width: 8px;
      height: 8px;
      border-radius: 50%;
      background-color: var(--medical-red);
      transition: background-color 0.3s;
    }
    .status-dot.online { background-color: var(--medical-green); }

    /* ---------- Layout: no celular um cartão embaixo do outro, no PC lado a lado ---------- */
    .dashboard {
      display: flex;
      flex-direction: column;
      gap: 20px;
      width: 95%;
      max-width: 1200px;
    }
    @media (min-width: 768px) {
      .dashboard { flex-direction: row; align-items: stretch; }
    }

    /* ---------- Cartões ---------- */
    .card {
      background: var(--panel-bg);
      border-radius: 12px;
      border: 1px solid var(--border-color);
      box-shadow: 0 4px 6px -1px rgba(0, 0, 0, 0.05);
      flex: 1;
      display: flex;
      flex-direction: column;
      overflow: hidden;
    }
    .card-video { flex: 1.6; } /* o vídeo ocupa mais espaço que os controles */

    .card-header {
      background: #f8fafc;
      padding: 12px 16px;
      font-size: 0.9rem;
      font-weight: 600;
      color: var(--text-muted);
      border-bottom: 1px solid var(--border-color);
      text-transform: uppercase;
      letter-spacing: 0.5px;
      display: flex;
      align-items: center;
      justify-content: space-between;
    }

    .card-body {
      padding: 20px;
      display: flex;
      flex-direction: column;
      align-items: center;
      justify-content: center;
      flex: 1;
    }

    /* ---------- Vídeo da câmera (proporção 16:9) ---------- */
    .video-container {
      width: 100%;
      aspect-ratio: 16 / 9;
      min-height: 320px;
      border-radius: 8px;
      overflow: hidden;
      background: #0f172a;
      border: 1px solid var(--border-color);
    }
    iframe { width: 100%; height: 100%; border: none; }

    /* ---------- Botões de direção: grade 3x3, só as 4 setas ficam preenchidas ---------- */
    .dpad {
      display: grid;
      grid-template-columns: repeat(3, 1fr);
      grid-template-rows: repeat(3, 1fr);
      gap: 10px;
      width: 220px;
      height: 220px;
    }

    .btn-cmd {
      background-color: var(--header-bg);
      color: #ffffff;
      border: 1px solid var(--header-bg);
      border-radius: 10px;
      font-size: 1.8rem;
      font-weight: bold;
      cursor: pointer;
      display: flex;
      justify-content: center;
      align-items: center;
      transition: all 0.15s ease;
      box-shadow: 0 2px 4px rgba(1, 24, 36, 0.3);
      touch-action: none; /* no celular, segurar o botão não rola a página */
    }
    /* Mouse em cima */
    .btn-cmd:hover { background-color: var(--primary-hover); box-shadow: 0 4px 6px rgba(2, 132, 199, 0.4); }
    /* Botão apertado (mouse, dedo ou tecla do teclado) */
    .btn-cmd:active, .btn-cmd.active-press {
      background-color: var(--primary-active);
      transform: scale(0.96);
      box-shadow: none;
    }

    /* Botão de parar (centro das setas): vermelho */
    .btn-stop {
      background-color: var(--medical-red);
      border-color: #7e1010;
      font-size: 1.3rem;
      box-shadow: 0 2px 4px rgba(220, 38, 38, 0.3);
    }
    .btn-stop:hover { background-color: #b91c1c; box-shadow: 0 4px 6px rgba(220, 38, 38, 0.4); }
    .btn-stop:active, .btn-stop.active-press { background-color: #991b1b; }

    /* Dica de teclado embaixo dos botões */
    .kb-hint {
      margin-top: 15px;
      font-size: 0.8rem;
      color: var(--text-muted);
      text-align: center;
    }

    /* ---------- Barra de velocidade ---------- */
    .speed-section {
      width: 100%;
      max-width: 260px;
      margin-top: 20px;
      padding-top: 15px;
      border-top: 1px dashed var(--border-color);
      text-align: center;
    }
    .speed-label {
      font-size: 0.9rem;
      font-weight: 600;
      margin-bottom: 8px;
    }
    #velocidade {
      width: 100%;
      accent-color: var(--primary-blue); /* cor da barra e da bolinha */
      cursor: pointer;
    }

    /* ---------- Botão da luz auxiliar ---------- */
    .light-section {
      width: 100%;
      margin-top: 20px;
      padding-top: 15px;
      border-top: 1px dashed var(--border-color);
      display: flex;
      justify-content: center;
    }

    #btn-led {
      width: 100%;
      max-width: 220px;
      padding: 12px;
      border-radius: 8px;
      border: 1px solid var(--border-color);
      font-weight: 600;
      font-size: 0.9rem;
      color: var(--text-dark);
      background: var(--btn-bg);
      cursor: pointer;
      transition: all 0.25s ease;
      display: flex;
      align-items: center;
      justify-content: center;
      gap: 8px;
    }
    /* LED ligado: o botão fica verde */
    #btn-led.on {
      background: #ecfdf5;
      color: var(--medical-green);
      border-color: #a7f3d0;
      box-shadow: 0 0 10px rgba(16, 185, 129, 0.15);
    }
  </style>
</head>
<body oncontextmenu="return false"> <!-- bloqueia o menu do toque longo no celular -->

  <!-- Barra superior com logo e status da conexão -->
  <header class="top-bar">
    <span class="brand-icon">&#10010;</span>
    <div class="status-badge">
      <span id="status-dot" class="status-dot"></span>
      <span id="status-text">Desconectado</span>
    </div>
  </header>

  <div class="dashboard">

    <!-- Cartão de controle: setas, velocidade e LED -->
    <div class="card">
      <div class="card-header">
        <span>Controle de Movimentação</span>
        <span>&#8644;</span>
      </div>
      <div class="card-body">

        <!-- Setas: pointerdown = apertou (manda a direção), pointerup/leave/cancel = soltou (manda parar) -->
        <div class="dpad">
          <div></div>
          <button class="btn-cmd" id="btn-up"    onpointerdown="iniciar('F')" onpointerup="parar()" onpointerleave="parar()" onpointercancel="parar()">&uarr;</button>
          <div></div>

          <button class="btn-cmd" id="btn-left"  onpointerdown="iniciar('E')" onpointerup="parar()" onpointerleave="parar()" onpointercancel="parar()">&larr;</button>
          <!-- Parar: manda "P" assim que apertar -->
          <button class="btn-cmd btn-stop" id="btn-stop" onpointerdown="enviar('P')">&#9632;</button>
          <button class="btn-cmd" id="btn-right" onpointerdown="iniciar('D')" onpointerup="parar()" onpointerleave="parar()" onpointercancel="parar()">&rarr;</button>

          <div></div>
          <button class="btn-cmd" id="btn-down"  onpointerdown="iniciar('T')" onpointerup="parar()" onpointerleave="parar()" onpointercancel="parar()">&darr;</button>
          <div></div>
        </div>

        <div class="kb-hint">Segure para andar, solte para parar<br>Teclado: WASD ou Setas | Espaço para parar</div>

        <!-- Barra de velocidade de 0% a 100% -->
        <div class="speed-section">
          <div class="speed-label">Velocidade: <span id="velTexto">100</span>%</div>
          <!-- oninput: atualiza o número enquanto arrasta; onchange: envia ao robô quando soltar -->
          <input type="range" id="velocidade" min="0" max="100" step="5" value="100"
                 oninput="document.getElementById('velTexto').innerText = this.value"
                 onchange="enviar('V' + this.value)">
        </div>

        <!-- Botão da luz auxiliar -->
        <div class="light-section">
          <button id="btn-led" onclick="alternaLed()">
            <span>Iluminação Auxiliar: OFF</span>
          </button>
        </div>
      </div>
    </div>

    <!-- Cartão da câmera -->
    <div class="card card-video">
      <div class="card-header">
        <span>Monitoramento de Vídeo</span>
        <span>&#128065;</span>
      </div>
      <div class="card-body">
        <div class="video-container">
          <!-- Troque pelo IP da sua câmera -->
          <iframe src="http://192.168.0.176:8080/" title="Câmera do Robô"></iframe>
        </div>
      </div>
    </div>
  </div>

  <script>
    var websock;            // conexão com o robô
    var movendo = false;    // true enquanto algum botão de direção está apertado

    // Conecta no robô pelo WebSocket (porta 81, mesmo endereço da página)
    function InitWebSocket() {
      websock = new WebSocket('ws://' + window.location.hostname + ':81/');

      // Conectou: bolinha verde
      websock.onopen = function () {
        document.getElementById('status-dot').classList.add('online');
        document.getElementById('status-text').innerText = 'Conectado';
      };

      // O robô mandou algo: "ON", "OFF" (LED) ou "V50" (velocidade em %)
      websock.onmessage = function (evt) {
        var msg = evt.data;
        var btnLed = document.getElementById('btn-led');

        if (msg == 'ON') {
          btnLed.innerHTML = '<span>Iluminação Auxiliar: ON</span>';
          btnLed.classList.add('on');
        } else if (msg == 'OFF') {
          btnLed.innerHTML = '<span>Iluminação Auxiliar: OFF</span>';
          btnLed.classList.remove('on');
        } else if (msg[0] == 'V') {
          var vel = msg.substring(1);                       // tira o "V" e fica só o número
          document.getElementById('velocidade').value = vel;
          document.getElementById('velTexto').innerText = vel;
        }
      };

      // Caiu a conexão: bolinha vermelha e tenta de novo em 2 segundos
      websock.onclose = function () {
        document.getElementById('status-dot').classList.remove('online');
        document.getElementById('status-text').innerText = 'Desconectado';
        setTimeout(InitWebSocket, 2000);
      };
    }

    // Manda uma mensagem ao robô (só se a conexão estiver aberta)
    function enviar(msg) {
      if (websock && websock.readyState == 1) websock.send(msg);
    }

    // Apertou uma direção: marca que está andando e manda a letra (F, T, E ou D)
    function iniciar(cmd) {
      movendo = true;
      enviar(cmd);
    }

    // Soltou: se estava andando, manda "P" (parado) uma única vez
    function parar() {
      if (movendo) {
        movendo = false;
        enviar('P');
      }
    }

    // Liga ou desliga o LED conforme o estado atual do botão
    function alternaLed() {
      var ligado = document.getElementById('btn-led').classList.contains('on');
      enviar(ligado ? 'OFF' : 'ON');
    }

    // ---------- Teclado: cada tecla → [letra do comando, id do botão na tela] ----------
    var teclas = {
      'w': ['F', 'btn-up'],    'arrowup':    ['F', 'btn-up'],
      's': ['T', 'btn-down'],  'arrowdown':  ['T', 'btn-down'],
      'a': ['E', 'btn-left'],  'arrowleft':  ['E', 'btn-left'],
      'd': ['D', 'btn-right'], 'arrowright': ['D', 'btn-right'],
      ' ': ['P', 'btn-stop']     // barra de espaço = parar
    };

    // Apertou uma tecla: anda e acende o botão correspondente
    document.addEventListener('keydown', function (e) {
      var t = teclas[e.key.toLowerCase()];
      if (!t || e.repeat) return;       // ignora outras teclas e a repetição ao segurar
      e.preventDefault();               // impede a página de rolar com as setas
      iniciar(t[0]);
      document.getElementById(t[1]).classList.add('active-press');
    });

    // Soltou a tecla: para e apaga o botão
    document.addEventListener('keyup', function (e) {
      var t = teclas[e.key.toLowerCase()];
      if (!t) return;
      parar();
      document.getElementById(t[1]).classList.remove('active-press');
    });

    window.onload = InitWebSocket;      // conecta assim que a página carrega
  </script>
</body>
</html>
)=====";
