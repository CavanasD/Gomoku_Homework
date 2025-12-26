const { app, BrowserWindow, ipcMain } = require('electron')
const { spawn } = require('child_process')
const path = require('path')

let mainWindow
let backendProcess

function createWindow() {
    mainWindow = new BrowserWindow({
        width: 1200,
        height: 800,
        minWidth: 1000,
        minHeight: 700,
        frame: false,
        transparent: true,
        resizable: true, // 必须开启
        hasShadow: true,
        webPreferences: {
            nodeIntegration: true,
            contextIsolation: false
        }
    })

    mainWindow.loadFile('index.html')

    const exePath = path.join(__dirname, 'gomoku_core.exe')
    backendProcess = spawn(exePath)

    backendProcess.stdout.on('data', (data) => {
        const lines = data.toString().split(/\r?\n/)
        lines.forEach(line => {
            if (line.trim()) mainWindow.webContents.send('cpp-msg', line.trim())
        })
    })

    mainWindow.on('closed', () => {
        if (backendProcess) backendProcess.kill()
        mainWindow = null
    })
}

app.whenReady().then(createWindow)

ipcMain.on('window-min', (event) => {
    const win = BrowserWindow.fromWebContents(event.sender)
    if (win) win.minimize()
})

ipcMain.on('window-max', (event) => {
    const win = BrowserWindow.fromWebContents(event.sender)
    if (win) {
        if (win.isMaximized()) {
            win.unmaximize() // 还原
        } else {
            win.maximize()   // 最大化
        }
    }
})

ipcMain.on('window-close', (event) => {
    const win = BrowserWindow.fromWebContents(event.sender)
    if (win) win.close()
})

ipcMain.on('ui-cmd', (event, arg) => {
    if (backendProcess) backendProcess.stdin.write(arg + '\n')
})