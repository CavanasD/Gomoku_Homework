const { ipcRenderer } = require('electron')

const boardEl = document.getElementById('board')
const statusEl = document.getElementById('status')
const BOARD_SIZE = 15
const CELL_SIZE = 40 // 40px

// 1. 动态生成 15x15 的棋盘格子
// 注意：我们的 CSS 画线是从 (20,20) 开始的，所以棋子也要偏移 20px
const OFFSET = 0; // 如果你发现棋子没对准线，调这个。目前CSS布局应该不需要偏移。

for (let y = 0; y < BOARD_SIZE; y++) {
    for (let x = 0; x < BOARD_SIZE; x++) {
        const cell = document.createElement('div')
        cell.className = 'cell'

        // 绝对定位
        cell.style.left = `${x * CELL_SIZE}px`
        cell.style.top = `${y * CELL_SIZE}px`

        // 绑定点击事件
        cell.dataset.x = x
        cell.dataset.y = y
        cell.onclick = handleMove

        cell.id = `c-${x}-${y}`
        boardEl.appendChild(cell)
    }
}

let isGameOver = false

// 玩家点击处理
function handleMove(e) {
    if (isGameOver) return
    const cell = e.target
    // 如果已经有棋子了，不许点
    if (cell.classList.contains('black') || cell.classList.contains('white')) return

    const x = cell.dataset.x
    const y = cell.dataset.y

    console.log(`点击了: ${x}, ${y}`)

    // 发送指令给后台
    ipcRenderer.send('ui-cmd', `MOVE ${x},${y}`)

    // 可以在这里先让 UI 变一下，防止等待感？
    // 最好还是等 C++ 确认 'PLAYER_MOVED' 再画，这样逻辑更严谨
}

// 接收 C++ 的回复
ipcRenderer.on('cpp-msg', (event, msg) => {
    console.log('收到:', msg)
    const parts = msg.split(' ')
    const cmd = parts[0]

    if (cmd === 'PLAYER_MOVED' || cmd === 'AI_MOVED') {
        // 格式: PLAYER_MOVED 3,4
        const coords = parts[1].split(',')
        const x = coords[0]
        const y = coords[1]
        const cell = document.getElementById(`c-${x}-${y}`)

        // 清除上一步的高亮
        document.querySelector('.last-move')?.classList.remove('last-move')
        cell.classList.add('last-move')

        if (cmd === 'PLAYER_MOVED') {
            cell.classList.add('black')
            statusEl.innerText = "可爱的流萤酱正在思考..."
        } else {
            cell.classList.add('white')
            statusEl.innerText = "轮到你了"
        }
    }
    else if (cmd === 'WINNER') {
        const who = parts[1] // BLACK / WHITE
        statusEl.innerText = `游戏结束！${who} 获胜！`
        isGameOver = true
        setTimeout(() => alert(`${who} 赢了!`), 100)
    }
})