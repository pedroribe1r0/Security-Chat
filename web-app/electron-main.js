// web-app/electron-main.js
const { app, BrowserWindow } = require('electron');
const path = require('path');
const { fork } = require('child_process');

let mainWindow;
let backendProcess;

function startBackend() {
  const backendPath = path.join(__dirname, 'backend', 'dist', 'main.js');
  
  console.log("Iniciando Backend em:", backendPath);

  backendProcess = fork(backendPath, [], {
    env: { ...process.env, PORT: 3000 },
    stdio: 'pipe' 
  });

  backendProcess.stdout.on('data', (data) => console.log(`[Nest]: ${data}`));
  backendProcess.stderr.on('data', (data) => console.error(`[Nest Error]: ${data}`));
}

function createWindow() {
  mainWindow = new BrowserWindow({
    width: 1000,
    height: 800,
    backgroundColor: '#050505', 
    webPreferences: {
      nodeIntegration: true,
      contextIsolation: false,
    },
    autoHideMenuBar: true, 
  });

  mainWindow.loadFile(path.join(__dirname, 'frontend', 'dist', 'index.html'));
}

app.whenReady().then(() => {
  startBackend();
  setTimeout(createWindow, 1000); 
});

app.on('before-quit', () => {
  if (backendProcess) {
    backendProcess.kill();
  }
});

app.on('window-all-closed', () => {
  if (process.platform !== 'darwin') app.quit();
});