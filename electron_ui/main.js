const { app, BrowserWindow, ipcMain } = require('electron')
const { spawn } = require('child_process')
const path = require('path')

let mainWindow
let backendProcess // C++ 进程

function createWindow() {
    mainWindow = new BrowserWindow({
        width: 900,
        height: 900,
        webPreferences: {
            nodeIntegration: true, // 允许页面使用 Node API
            contextIsolation: false // 关闭隔离以便直接通讯
        }
    })

    mainWindow.loadFile('index.html')

    // --- 关键：启动 C++ 程序 ---
    // 确保你的 exe 文件名是 gomoku_core.exe，并且就在当前目录下
    const exePath = path.join(__dirname, 'gomoku_core.exe')
    console.log(`正在启动 C++ 核心: ${exePath}`)

    backendProcess = spawn(exePath)

    // 1. 监听 C++ 的正常说话 (stdout)
    backendProcess.stdout.on('data', (data) => {
        // 数据可能是 Buffer，转成字符串
        const output = data.toString()
        console.log(`[C++]: ${output}`)

        // C++ 可能会一次发多条消息（比如连发 PLAYER_MOVED 和 AI_MOVED）
        // 所以按换行符切分一下
        const lines = output.split(/\r?\n/)
        lines.forEach(line => {
            if (line.trim()) {
                mainWindow.webContents.send('cpp-msg', line.trim())
            }
        })
    })

    // 2. 监听 C++ 的报错 (stderr)
    backendProcess.stderr.on('data', (data) => {
        console.error(`[C++ Error]: ${data}`)
    })

    // 3. C++ 挂了
    backendProcess.on('close', (code) => {
        console.log(`C++ 进程退出，代码: ${code}`)
    })

    // 窗口关闭时，记得杀掉 C++ 进程，否则它会留在后台占内存
    mainWindow.on('closed', () => {
        backendProcess.kill()
        mainWindow = null
    })
}

app.whenReady().then(createWindow)

// --- 接收网页发来的指令，转发给 C++ ---
ipcMain.on('ui-cmd', (event, arg) => {
    // arg 格式: "MOVE 3,4"
    if (backendProcess && !backendProcess.killed) {
        console.log(`[UI -> C++]: ${arg}`)
        backendProcess.stdin.write(arg + '\n') // 必须加 \n，否则 C++ 的 getline 读不到
    }
})