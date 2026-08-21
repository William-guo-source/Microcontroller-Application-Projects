# 💻 AT89S52 Microcontroller Applications 

本儲存庫收錄了基於 **Atmel AT89S52 (8051 架構)** 微控制器與 **C 語言** 開發的進階嵌入式系統專案。專案內容涵蓋了多種硬體周邊控制、中斷系統 (Interrupts) 的安全架構設計，以及跨設備的非同步通訊 (UART) 實作。

## 🛠️ 開發環境與技術棧 (Tech Stack)

*   **微控制器 (MCU):** Atmel AT89S52 (8051 Architecture)
*   **開發語言:** C Language (C51)
*   **開發與編譯環境:** Keil µVision
*   **核心技術:** 硬體中斷與計時器 (Timers/Interrupts)、UART 串列通訊、LCD 驅動與矩陣鍵盤掃描 (Debouncing)。

---

## 📂 亮點專案介紹 (Featured Projects)

點擊下方專案名稱，可進入子資料夾查看完整的 C 語言原始碼與詳細設定說明：

### 1. [⏱️ 可調整數位時鐘 (Adjustable Digital Clock)](./adjustable-clock)
結合了 LCD 顯示器與 4x4 矩陣鍵盤的數位時鐘系統。
*   **技術亮點：** 
    *   利用 Timer 0 精準控制 1 秒鐘的計時中斷。
    *   使用專業的 **旗標法 (Flag-based)** ，徹底解決 8051 記憶體覆疊 (Memory Overlay) 的潛在衝突。
    *   內建按鍵防彈跳 (Debounce) 邏輯與直覺的 UI 閃爍調整模式。

### 2. [💣 UART 雙人對戰拆炸彈 (UART Battleship Game)](./UART)
透過實體線路連接兩塊 8051 開發板，實現宛如「踩地雷 / 海戰棋」的連線互動遊戲。
*   **技術亮點：** 
    *   利用 Timer 1 (Mode 2 Auto-reload) 設定 9600 bps 的 Baud Rate，實作穩定無延遲的 UART 雙向通訊。
    *   「人工回合制」的交握邏輯，避開單一 Byte 接收暫存器的溢位漏接問題。
    *   實作陣列比對演算法，即時透過 LCD 回饋 `HIT` (命中)、`MISS` (未擊中) 或 `WIN` (勝利) 狀態。

---

## 🚀 關於本儲存庫

本儲存庫專注於展示 MCU 底層韌體開發邏輯，程式碼皆已針對編譯器警告（如 `MULTIPLE CALL TO SEGMENT`）進行架構優化，確保系統具備高度穩定性與可維護性。