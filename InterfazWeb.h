#ifndef INTERFAZ_WEB_H
#define INTERFAZ_WEB_H

#include <Arduino.h>

// ---------------- Página web (HTML + CSS + JavaScript) ----------------
// Toda la comunicación con el ESP8266 es por AJAX (fetch), sin recargar la página.

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="es">
  <head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>Control HEX3653 &amp; TEA5767</title>
    <style>
      :root {
        font-family: Arial, Helvetica, sans-serif;
        --fondo: #f2f4f8;
        --tarjeta: #ffffff;
        --borde: #dde2ea;
        --acento: #1a73e8;
        --texto: #1f2430;
        --gris: #5b6472;
      }
      * { box-sizing: border-box; }
      body {
        margin: 0;
        background: var(--fondo);
        color: var(--texto);
      }
      .page { max-width: 920px; margin: 0 auto; padding: 24px 16px 8px; }
      h1 { text-align: center; margin: 8px 0 4px; font-size: 26px; }
      .selector {
        text-align: center;
        margin: 14px 0 22px;
      }
      .selector label {
        font-weight: bold;
        margin-right: 8px;
        color: var(--gris);
      }
      .selector select {
        font-size: 16px;
        padding: 8px 12px;
        border-radius: 8px;
        border: 1px solid var(--borde);
        background: var(--tarjeta);
        cursor: pointer;
      }
      .volumen {
        display: flex; align-items: center; gap: 12px;
        margin-bottom: 20px;
      }
      .volumen label { font-weight: bold; color: var(--gris); }
      .volumen input { flex: 1; min-width: 0; }
      .volumen button.btn-mute { width: auto; min-width: 110px; }
      .volumen button.btn-mute.activo { border-color: var(--acento); color: var(--acento); font-weight: bold; }
      .volumen span { min-width: 3.5em; text-align: right; font-variant-numeric: tabular-nums; }
      .sel-msg { min-height: 1.2em; margin-top: 6px; font-size: 14px; color: #c62828; }
      .card {
        background: var(--tarjeta);
        border: 1px solid var(--borde);
        border-radius: 12px;
        padding: 18px;
        box-shadow: 0 2px 6px rgba(0,0,0,.06);
      }
      .panel { margin-bottom: 20px; }
      .panel h2 { margin: 0 0 14px; font-size: 20px; color: var(--acento); }
      .grid {
        display: grid;
        grid-template-columns: repeat(auto-fit, minmax(150px, 1fr));
        gap: 12px;
      }
      button.btn {
        width: 100%;
        height: 56px;
        font-size: 16px;
        border-radius: 10px;
        border: 1px solid var(--borde);
        background: var(--tarjeta);
        cursor: pointer;
      }
      button.btn:hover { border-color: var(--acento); color: var(--acento); }
      button.btn:active { background: #e8f0fe; }
      .fila {
        display: flex;
        gap: 8px;
        align-items: center;
        justify-content: center;
        flex-wrap: wrap;
        margin-top: 12px;
      }
      .fila input[type="number"] {
        width: 140px;
        height: 42px;
        font-size: 16px;
        text-align: center;
        border: 1px solid var(--borde);
        border-radius: 8px;
        padding: 0 8px;
      }
      .fila button.btn { width: auto; min-width: 64px; height: 42px; }
      .status { margin-top: 16px; }
      .rows { display: grid; gap: 10px; }
      .row { display: flex; justify-content: space-between; align-items: center; }
      .key { color: var(--gris); }
      .val { font-weight: bold; }
      .barra-wrap {
        height: 14px;
        width: 100%;
        background: #e7ebf1;
        border-radius: 7px;
        overflow: hidden;
      }
      .barra {
        height: 100%;
        width: 0%;
        border-radius: 7px;
        transition: width .3s;
      }
      .barra.bad { background: #d93025; }
      .barra.mid { background: #f9ab00; }
      .barra.good { background: #188038; }
      .statusbar {
        margin-top: 12px;
        color: var(--gris);
        font-size: 14px;
        min-height: 18px;
      }
      .hidden { display: none; }
      footer {
        margin: 24px auto 16px;
        text-align: center;
        font-size: 14px;
        color: var(--gris);
        line-height: 1.5;
      }
      footer .title { font-weight: bold; font-size: 15px; color: var(--texto); }
    </style>
  </head>
  <body>
    <div class="page">
      <h1>Consola de radio</h1>
      <div class="selector">
        <label for="sel-radio">Radio:</label>
        <select id="sel-radio">
          <option value="hex">HEX3653</option>
          <option value="tea">TEA5767</option>
        </select>
        <div class="sel-msg" id="sel-msg"></div>
      </div>

      <div class="card volumen">
        <label for="vol">Volumen</label>
        <input type="range" id="vol" min="0" max="100" step="1" value="50">
        <span id="vol-valor">50%</span>
        <button class="btn btn-mute" id="btn-mute" onclick="alternarMute()">Mute</button>
      </div>

      <section id="panel-hex" class="card panel">
        <h2>Control HEX3653</h2>
        <div class="grid">
          <button class="btn" onclick="enviarPulsoHEX('s_up')">S_UP (D8)</button>
          <button class="btn" onclick="enviarPulsoHEX('s_down')">S_DOWN (D3)</button>
          <button class="btn" onclick="enviarPulsoHEX('on_off')">ON/OFF (D0)</button>
        </div>
      </section>

      <section id="panel-tea" class="card panel hidden">
        <h2>Control TEA5767</h2>
        <div class="grid">
          <button class="btn" onclick="enviarPulsoTEA('/_up')">Seek ↑</button>
          <button class="btn" onclick="enviarPulsoTEA('/_down')">Seek ↓</button>
          <button class="btn" onclick="enviarPulsoTEA('/stand_by')">Standby ON/OFF</button>
        </div>
        <div class="fila">
          <button class="btn" onclick="sintonizarFina(-0.1)">−0.1</button>
          <input id="valorEntero" type="number" step="0.1" min="87.5" max="108" inputmode="decimal" placeholder="Ej: 101.9">
          <button class="btn" onclick="sintonizarFina(0.1)">+0.1</button>
          <button class="btn" onclick="enviarValor()">Enviar</button>
        </div>
        <div class="card status">
          <div class="rows">
            <div class="row">
              <span class="key">Standby:</span>
              <span class="val" id="tea-standby">—</span>
            </div>
            <div id="tea-details" class="rows hidden">
              <div class="row">
                <span class="key">Frecuencia:</span>
                <span class="val" id="tea-frequency">—</span>
              </div>
              <div class="row">
                <span class="key">Calidad:</span>
                <span class="val" id="tea-qual">—</span>
              </div>
              <div class="barra-wrap"><div class="barra" id="tea-barra"></div></div>
            </div>
          </div>
          <div class="statusbar" id="status-msg">—</div>
        </div>
      </section>
    </div>

    <footer>
      <div class="title">Taller de Proyecto 2</div>
      <div>Facultad de Informática - UNLP</div>
      <div>Grupo F2 - Prof. Fernando Tinetti</div>
      <div>Autores: Javier del Blanco, Armando Pereira y Álvaro Fricke</div>
    </footer>

    <script>
      const STORAGE_KEY = 'radioActiva'; // Última radio elegida, guardada en el navegador
      let ultimaFreq = 87.5;
      let teaTimer = null;

      // -------- Estado TEA5767 --------

      function cargarEstadoTEA() {
        return fetch('/tea/status')
          .then(r => r.json())
          .then(json => {
            renderTEA(json);
            document.getElementById('status-msg').textContent = 'Estado actualizado correctamente';
          })
          .catch(err => {
            console.error('No se pudo obtener /tea/status', err);
            document.getElementById('status-msg').textContent = 'Error al actualizar estado';
          });
      }

      function renderTEA(state) {
        const standby = !!state.standby;
        document.getElementById('tea-standby').textContent = standby ? 'Pausado' : 'Activo';
        const details = document.getElementById('tea-details');
        if (standby) {
          details.classList.add('hidden');
          document.getElementById('tea-frequency').textContent = '—';
          document.getElementById('tea-qual').textContent = '—';
          document.getElementById('tea-barra').style.width = '0%';
          return;
        }
        details.classList.remove('hidden');
        const f = Number(state.frequency);
        if (Number.isFinite(f)) {
          ultimaFreq = f;
          document.getElementById('tea-frequency').textContent = f.toFixed(2) + ' MHz';
        }
        const q = Number(state.quality);
        if (Number.isFinite(q)) {
          document.getElementById('tea-qual').textContent = q;
          const bar = document.getElementById('tea-barra');
          bar.style.width = q + '%';
          bar.classList.remove('bad', 'mid', 'good');
          bar.classList.add(q < 40 ? 'bad' : (q < 70 ? 'mid' : 'good'));
        }
      }

      // -------- Envío de comandos --------

      // HEX3653: sus rutas solo confirman el pulso, no hay estado que mostrar
      function enviarPulsoHEX(pin) {
        fetch('/' + pin)
          .catch(err => console.error('ERR:', pin, err));
      }

      // TEA5767: el ESP8266 responde con el estado actualizado, se dibuja directamente
      function enviarPulsoTEA(url) {
        return fetch(url)
          .then(r => r.json())
          .then(renderTEA)
          .catch(err => console.error('ERR:', url, err));
      }

      function enviarValor() {
        const raw = document.getElementById('valorEntero').value;
        if (!raw) { alert('Ingrese un número primero'); return; }
        const f = Number(raw.replace(',', '.'));
        if (!Number.isFinite(f) || f < 87.5 || f > 108) {
          alert('La frecuencia debe estar entre 87.5 y 108 MHz');
          return;
        }
        enviarPulsoTEA('/set_freq?v=' + f.toFixed(2))
          .then(() => { document.getElementById('valorEntero').value = ''; });
      }

      function sintonizarFina(delta) {
        const nueva = Math.round((ultimaFreq + delta) * 100) / 100;
        const f = Math.min(108, Math.max(87.5, nueva));
        enviarPulsoTEA('/set_freq?v=' + f.toFixed(2))
          .then(() => { document.getElementById('valorEntero').value = f.toFixed(2); });
      }

      // -------- Consulta periódica del estado del TEA (solo con la TEA seleccionada) --------
      function iniciarPolling() {
        if (teaTimer) return;
        teaTimer = setInterval(cargarEstadoTEA, 500);
      }

      function detenerPolling() {
        if (teaTimer) {
          clearInterval(teaTimer);
          teaTimer = null;
        }
      }

      // -------- Selección de radio (multiplexor CD4052) --------
      let radioActual = null;

      // Muestra el panel de la radio indicada (solo interfaz, no toca el hardware)
      function mostrarRadio(radio) {
        const esTea = radio === 'tea';
        radioActual = radio;
        document.getElementById('panel-hex').classList.toggle('hidden', esTea);
        document.getElementById('panel-tea').classList.toggle('hidden', !esTea);
        document.getElementById('sel-radio').value = radio;
        try { localStorage.setItem(STORAGE_KEY, radio); } catch (e) {}
        if (esTea) {
          document.getElementById('status-msg').textContent = 'Cargando estado…';
          cargarEstadoTEA();
          iniciarPolling();
        } else {
          detenerPolling();
          document.getElementById('status-msg').textContent = '';
        }
      }

      // Pide al ESP8266 la radio activa (o la cambia si se indica una); devuelve 'tea' o 'hex'
      function pedirMux(radio) {
        const ctrl = new AbortController();
        const t = setTimeout(() => ctrl.abort(), 2000); // Sin respuesta en 2 s se toma como error
        const url = radio ? '/mux?radio=' + radio : '/mux';
        return fetch(url, { signal: ctrl.signal })
          .then(r => { if (!r.ok) throw new Error('HTTP ' + r.status); return r.text(); })
          .then(txt => {
            txt = txt.trim();
            if (txt !== 'tea' && txt !== 'hex') throw new Error('Respuesta inválida: ' + txt);
            return txt;
          })
          .finally(() => clearTimeout(t));
      }

      // Conmuta el CD4052 y cambia el panel solo cuando el ESP8266 lo confirma
      function seleccionarRadio(radio) {
        const sel = document.getElementById('sel-radio');
        const msg = document.getElementById('sel-msg');
        sel.disabled = true;
        msg.textContent = '';
        return pedirMux(radio)
          .then(activa => {
            mostrarRadio(activa);
            if (activa !== radio) msg.textContent = 'No se pudo cambiar de radio, intentá de nuevo';
          })
          .catch(err => {
            console.error('ERR mux:', err);
            msg.textContent = 'No se pudo cambiar de radio, intentá de nuevo';
            if (radioActual) {
              sel.value = radioActual; // Vuelve a la radio que estaba sonando
              return;
            }
            // Primera carga: se pregunta qué radio está sonando realmente
            return pedirMux()
              .then(mostrarRadio)
              .catch(() => mostrarRadio('tea')); // Sin conexión: la radio con la que arranca la placa
          })
          .finally(() => { sel.disabled = false; });
      }

      // -------- Volumen y mute (X9C103S, común a ambas radios) --------
      let volTimer = null;
      function renderVol(estado) {
        // El firmware responde {"volume": N, "muted": bool}
        estado = { volumen: estado.volume, mute: !!estado.muted };
        const vol = document.getElementById('vol');
        const btn = document.getElementById('btn-mute');
        vol.value = estado.volumen;
        document.getElementById('vol-valor').textContent = estado.mute ? 'Mute' : estado.volumen + '%';
        btn.textContent = estado.mute ? 'Quitar mute' : 'Mute';
        btn.classList.toggle('activo', estado.mute);
      }
      function pedirVol(url) {
        return fetch(url).then(r => r.json()).then(renderVol)
          .catch(err => console.error('ERR volumen:', err));
      }
      function enviarVolumen(v) {
        clearTimeout(volTimer);
        // Se espera a que el usuario deje de mover el control para no saturar al ESP8266
        volTimer = setTimeout(() => pedirVol('/volumen?v=' + v), 120);
      }
      function alternarMute() {
        clearTimeout(volTimer);
        pedirVol('/mute');
      }
      function iniciarVolumen() {
        const vol = document.getElementById('vol');
        vol.addEventListener('input', () => {
          // Mover el volumen quita el mute
          renderVol({ volume: parseInt(vol.value, 10), muted: false });
          enviarVolumen(vol.value);
        });
        pedirVol('/volumen');
      }

      // -------- Inicio --------
      window.addEventListener('DOMContentLoaded', () => {
        iniciarVolumen();
        let sel = 'tea'; // Por defecto, la radio con la que arranca la placa
        try { sel = localStorage.getItem(STORAGE_KEY) || 'tea'; } catch (e) {}
        document.getElementById('sel-radio').addEventListener('change', e => seleccionarRadio(e.target.value));
        seleccionarRadio(sel);
      });
    </script>
  </body>
</html>
)rawliteral";

#endif // INTERFAZ_WEB_H
