function greet() {
  const el = document.getElementById("status");
  el.textContent =
    "Halo dari script.js! Waktu: " + new Date().toLocaleTimeString("id-ID");
}
