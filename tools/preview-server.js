const http = require("http");
const fs = require("fs");
const path = require("path");

const root = __dirname;
const port = 8765;

const server = http.createServer((req, res) => {
  const name = decodeURIComponent((req.url || "/").split("?")[0].replace(/^\//, "")) ||
    "ui-preview.html";
  const file = path.join(root, path.basename(name));
  fs.readFile(file, (err, data) => {
    if (err) {
      res.writeHead(404);
      res.end("not found");
      return;
    }
    const type = file.endsWith(".html") ? "text/html; charset=utf-8" : "text/plain";
    res.writeHead(200, { "Content-Type": type });
    res.end(data);
  });
});

server.listen(port, "127.0.0.1", () => {
  console.log(`READY http://127.0.0.1:${port}/ui-preview.html`);
});
