const { ipcRenderer } = require('electron')
const os = require('os')
const fs = require('fs')
const path = require('path')
// 先获取时间，防止报错
function updateTime() {
    try {
        const now = new Date()
        const timeEl = document.getElementById('clock-time')
        const dateEl = document.getElementById('clock-date')
        if(timeEl) timeEl.innerText = now.toLocaleTimeString('en-GB', {hour:'2-digit', minute:'2-digit'})
        if(dateEl) dateEl.innerText = now.toDateString()
    } catch(e) { console.error("Time Error", e) }
}
setInterval(updateTime, 1000)
updateTime()

// Status显示+存储
const DATA_FILE = path.join(__dirname, 'gamedata.json')

function loadData() {
    const defaultData = { pve: { win: 0, lose: 0 }, pvp: 0, countdown: { name: '流萤酱生日', date: '2026-01-01' }, theme: 'light' }
    try {
        if (fs.existsSync(DATA_FILE)) {
            const fileData = JSON.parse(fs.readFileSync(DATA_FILE, 'utf-8'))
            return { ...defaultData, ...fileData }
        }
    } catch (e) { console.error("Data Load Error", e) }
    return defaultData
}
let appData = loadData()

function saveData(data) {
    try { fs.writeFileSync(DATA_FILE, JSON.stringify(data, null, 2)) } catch(e){}
}

// 悬浮弹出提示框
let toastTimer = null
function showToast(text, icon = '✨') {
    const toastEl = document.getElementById('toast-msg')
    if (!toastEl) return
    const textEl = document.getElementById('toast-text')
    const iconEl = toastEl.querySelector('.toast-icon')

    textEl.innerText = text
    iconEl.innerText = icon
    toastEl.classList.add('show')

    if (toastTimer) clearTimeout(toastTimer)
    toastTimer = setTimeout(() => { toastEl.classList.remove('show') }, 3000)
}
setTimeout(() => showToast('欢迎回来，N1n3Bird', '👋'), 1000)

// 窗口开关最大化最小化按钮
document.getElementById('btn-close').onclick = () => ipcRenderer.send('window-close')
document.getElementById('btn-min').onclick = () => ipcRenderer.send('window-min')
document.getElementById('btn-max').onclick = () => ipcRenderer.send('window-max')

// 悬浮Dock
const navHighlight = document.getElementById('nav-highlight')
const navItems = document.querySelectorAll('.nav-item')
window.switchPage = (pageId, index) => {
    document.querySelectorAll('.page').forEach(p => p.classList.remove('active'))
    document.getElementById(`page-${pageId}`).classList.add('active')
    navItems.forEach(n => n.classList.remove('active'))
    navItems[index].classList.add('active')
    if(navHighlight) {
        navHighlight.style.top = `${index * 62}px`
        navHighlight.style.opacity = '1'
    }
}
switchPage('home', 0)

// 颜色主题
function applyTheme(dark) {
    const html = document.documentElement
    const icon = document.getElementById('theme-icon')
    const label = document.getElementById('theme-label')
    if (dark) {
        html.setAttribute('data-theme', 'dark')
        if(icon) icon.innerText = '🌙'
        if(label) label.innerText = 'Dark Mode'
    } else {
        html.removeAttribute('data-theme')
        if(icon) icon.innerText = '☀️'
        if(label) label.innerText = 'Light Mode'
    }
}

// 初始化
const savedFollow = localStorage.getItem('followSystem') === 'true'
const followCheck = document.getElementById('follow-system')
if(followCheck) followCheck.checked = savedFollow
if (savedFollow) {
    applyTheme(window.matchMedia('(prefers-color-scheme: dark)').matches)
    window.matchMedia('(prefers-color-scheme: dark)').addEventListener('change', e => {
        if(localStorage.getItem('followSystem')==='true') applyTheme(e.matches)
    })
} else {
    applyTheme(appData.theme === 'dark')
}

window.toggleTheme = () => {
    const followCheck = document.getElementById('follow-system')
    if (followCheck && followCheck.checked) {
        followCheck.checked = false
        localStorage.setItem('followSystem', 'false')
        showToast('已关闭跟随系统主题', '⚙️')
    }
    appData.theme = (appData.theme === 'dark') ? 'light' : 'dark'
    saveData(appData)
    applyTheme(appData.theme === 'dark')
    showToast(appData.theme==='dark'?'已切换至深色模式':'已切换至浅色模式', appData.theme==='dark'?'🌙':'☀️')
}

window.toggleFollowSystem = (checked) => {
    localStorage.setItem('followSystem', checked)
    if (checked) {
        applyTheme(window.matchMedia('(prefers-color-scheme: dark)').matches)
        showToast('已开启跟随系统主题', '💻')
    } else {
        showToast('已关闭跟随系统主题', '⚙️')
    }
}

// Countdown
function initCountdown() {
    const { name, date } = appData.countdown
    const inName = document.getElementById('event-name-input')
    const inDate = document.getElementById('event-date-input')
    if(inName) inName.value = name
    if(inDate) inDate.value = date
    updateCountdownUI(name, date)
}
function updateCountdownUI(name, date) {
    const diff = new Date(date) - new Date()
    const days = Math.ceil(diff / 86400000)

    // 对应新 HTML 结构
    const title = document.getElementById('countdown-title') // 显示事件名
    const label = document.getElementById('countdown-date-label') // 显示日期
    const num = document.getElementById('countdown-days') // 显示天数

    if(title) title.innerText = name // 大字体事件名
    if(label) label.innerText = date // 小字体日期
    if(num) num.innerText = days > 0 ? days : "0"
}
window.saveCountdownSettings = () => {
    const name = document.getElementById('event-name-input').value || 'Event'
    const date = document.getElementById('event-date-input').value || '2026-01-01'
    appData.countdown = { name, date }
    saveData(appData)
    updateCountdownUI(name, date)
    showToast('倒数日事件已更新', '📅')
}
initCountdown()

// 瞎写的系统状态监视
function getCpuInfo() {
    const cpus = os.cpus()
    let idle=0, total=0
    cpus.forEach(c=>{ idle+=c.times.idle; total+=Object.values(c.times).reduce((a,b)=>a+b,0) })
    return {idle, total}
}
let startCpu = getCpuInfo()

function updateSystemMonitor() {
    try {
        const totalMem = os.totalmem(); const freeMem = os.freemem()
        const mem = Math.round(((totalMem - freeMem) / totalMem) * 100)

        const endCpu = getCpuInfo()
        const totalDiff = endCpu.total - startCpu.total; const idleDiff = endCpu.idle - startCpu.idle
        const cpu = totalDiff ? Math.round(100 - (idleDiff/totalDiff*100)) : 0
        startCpu = endCpu

        const time = Date.now()/1000
        const gpu = Math.min(100, Math.round(30 + Math.sin(time)*15 + Math.random()*10))
        const total = Math.round((cpu+mem+gpu)/3)

        const elCpu = document.getElementById('val-cpu'); if(elCpu) { elCpu.innerText=cpu+'%'; document.getElementById('bar-cpu').style.width=cpu+'%' }
        const elMem = document.getElementById('val-mem'); if(elMem) { elMem.innerText=mem+'%'; document.getElementById('bar-mem').style.width=mem+'%' }
        const elGpu = document.getElementById('val-gpu'); if(elGpu) { elGpu.innerText=gpu+'%'; document.getElementById('bar-gpu').style.width=gpu+'%' }

        const elTotal = document.getElementById('sys-total'); if(elTotal) elTotal.innerText=total+'%'
        const elCircle = document.getElementById('sys-circle-path')
        if(elCircle) {
            elCircle.style.strokeDashoffset = 251 - (total/100)*251
            if(total<50) elCircle.style.stroke='var(--pastel-blue)'
            else if(total<80) elCircle.style.stroke='var(--pastel-orange)'
            else elCircle.style.stroke='var(--pastel-pink)'
        }
    } catch(e) {}
}
setInterval(updateSystemMonitor, 2000)

// =Status
function updateStatsUI() {
    try {
        const { pve } = appData
        const total = pve.win + pve.lose
        const rate = total === 0 ? 0 : Math.round((pve.win / total) * 100)

        const elRate = document.getElementById('pve-rate')
        const elBar = document.getElementById('pve-bar')
        const elText = document.getElementById('stat-text')

        if(elRate) elRate.innerText = rate + '%'
        if(elBar) elBar.style.width = rate + '%'
        if(elText) elText.innerText = `${pve.win} 胜 / ${pve.lose} 负 (Total ${total})`
    } catch(e){}
}
updateStatsUI()

// Initialize Game Board
let currentMode = 'PVE'; let isGameActive = false; let timeLeft = 480; let timerId = null
const layer = document.getElementById('pieces-layer')

// 初始化格子
if(layer) {
    layer.innerHTML = ''
    for(let y=0; y<15; y++) {
        for(let x=0; x<15; x++) {
            const d = document.createElement('div')
            d.className='cell'
            d.style.left=x*40+'px'
            d.style.top=y*40+'px'
            d.onclick=()=>handleMove(x,y)
            d.id=`c-${x}-${y}`
            layer.appendChild(d)
        }
    }
}

window.switchMode = (mode) => {
    currentMode = mode
    const slider = document.getElementById('mode-slider')
    const pve = document.getElementById('mode-pve')
    const pvp = document.getElementById('mode-pvp')
    if (mode === 'PVE') {
        slider.style.left = '4px'; pve.classList.add('active'); pvp.classList.remove('active')
    } else {
        slider.style.left = '50%'; pve.classList.remove('active'); pvp.classList.add('active')
        showToast('切换至：人人对战模式', '👥')
    }
    resetGame()
}

// Game Reset
window.resetGame = () => {
    ipcRenderer.send('ui-cmd', 'SET_MODE ' + currentMode)
    document.querySelectorAll('.piece').forEach(el => el.classList.remove('piece','black','white','last'))

    const winText = document.getElementById('winner-text')
    if(winText) winText.innerText = ''

    isGameActive = true; timeLeft = 480; updateTimer()
    if(timerId) clearInterval(timerId)
    timerId = setInterval(() => { if(!isGameActive) return; timeLeft--; updateTimer(); if(timeLeft<=0) endGame("TIME OUT") }, 1000)

    const blocker = document.getElementById('blocker')
    if(blocker) blocker.style.display = 'none'
    updateTurnUI(1)

    // ★ 如果是 PVE，无论是切换还是点击开始，都弹窗 ★
    if (currentMode === 'PVE') {
        showToast('流萤酱先下，嘻嘻！', '✨')
    }
}

function handleMove(x, y) {
    if(!isGameActive) return
    const cell = document.getElementById(`c-${x}-${y}`)
    if(cell && cell.classList.contains('piece')) return
    ipcRenderer.send('ui-cmd', `MOVE ${x},${y}`)
}

// 接收 C++ 消息
ipcRenderer.on('cpp-msg', (e, msg) => {
    const parts = msg.split(' ')
    const cmd = parts[0]

    if (cmd === 'MOVED') {
        const [x, y, color] = parts[1].split(',')
        const cell = document.getElementById(`c-${x}-${y}`)
        if(cell) {
            document.querySelectorAll('.last').forEach(el => el.classList.remove('last'))
            cell.classList.add('piece', color==='1'?'black':'white', 'last')
        }
        updateTurnUI(color==='1' ? 2 : 1)

        const blocker = document.getElementById('blocker')
        if (blocker) {
            if (currentMode === 'PVE') {
                if (color === '1') blocker.style.display = 'none' // 黑(AI)下完 -> 解锁
                else blocker.style.display = 'block' // 白(玩家)下完 -> 锁住
            }
        }
    }
    else if (cmd === 'AI_THINKING') {
        const turnText = document.getElementById('turn-text')
        const blocker = document.getElementById('blocker')
        if(turnText) turnText.innerText = "流萤酱 思考中..."
        if(blocker) blocker.style.display = 'block'
    }
    else if (cmd.startsWith('WINNER')) {
        endGame(parts[1])
    }
})

function updateTurnUI(turn) {
    const dot = document.getElementById('turn-dot'); const txt = document.getElementById('turn-text')
    if(!dot || !txt) return
    if (turn === 1) {
        dot.style.background='#333';
        txt.innerText = (currentMode==='PVE') ? "流萤酱 (执黑)" : "黑方回合"
    } else {
        dot.style.background='#fff'; dot.style.border='1px solid #ccc';
        txt.innerText = (currentMode==='PVE') ? "玩家 (执白)" : "白方回合"
    }
}

function updateTimer() {
    const m = Math.floor(timeLeft/60).toString().padStart(2,'0')
    const s = (timeLeft%60).toString().padStart(2,'0')
    const timer = document.getElementById('game-timer')
    if(timer) timer.innerText = `${m}:${s}`
}

function endGame(winner) {
    isGameActive = false; if(timerId) clearInterval(timerId)
    // 记得去除遮罩

    const blocker = document.getElementById('blocker')
    if(blocker) blocker.style.display = 'none'

    let text = `${winner} WIN!`
    if (winner==='BLACK') text=(currentMode==='PVE')?"⚫ 流萤酱获胜！":"⚫ 黑方获胜！"
    if (winner==='WHITE') text=(currentMode==='PVE')?"⚪ 你赢了！":"⚪ 白方获胜！"

    const winText = document.getElementById('winner-text')
    if(winText) winText.innerText = text

    showToast(text, '🏆')
    if(currentMode==='PVE') {
        if(winner==='BLACK') appData.pve.lose++
        else if(winner==='WHITE') appData.pve.win++
        saveData(appData)
        updateStatsUI()
    }
}

// 每日一言api
fetch('https://v.api.aa1.cn/api/yiyan/index.php').then(r=>r.text()).then(t=>{
    const el = document.getElementById('daily-quote')
    if(el) el.innerText=t.replace(/<[^>]*>?/gm,'')
}).catch(()=>{})