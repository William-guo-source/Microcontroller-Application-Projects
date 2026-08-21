# ⏱️ 可調整數位時鐘 (Adjustable Digital Clock)

這是一個基於 8051 (AT89S52) 微控制器的數位時鐘專案。結合了 LCD 顯示器、4x4 矩陣鍵盤，並運用內部計時器 (Timer) 與外部中斷 (External Interrupt) 實現精準計時與時間調整功能。

## 📸 實體展示 (Demo)
<!-- 請將下方的網址替換成你的實際照片或 GIF 連結 -->
![Demo 相片](圖片網址)

## ✨ 核心功能 (Features)

*   **即時時間顯示：** 於 LCD 上精準顯示 `時:分:秒 (HH:MM:SS)`。
*   **硬體精準計時：** 利用 8051 內建的 Timer 0 與中斷，達成精確的 1 秒鐘計時 (5000us * 200次)。
*   **中斷觸發設定：** 透過外部中斷 (INT0) 隨時暫停時鐘，並進入時間調整模式。
*   **矩陣鍵盤輸入與防彈跳：** 支援 4x4 矩陣鍵盤掃描，並內建軟體防彈跳 (Debounce) 處理，確保輸入數值穩定不誤觸。
*   **友善的 UI/UX 設計：** 在調整時間時，當下正在修改的欄位（時、分或秒）會在 LCD 上呈現閃爍 (Blinking) 效果，方便使用者辨識。

## 🔌 硬體腳位配置 (Hardware Pinout)

本專案的 I/O 腳位定義如下：

| 元件 / 功能 | 8051 腳位 | 說明 |
| :--- | :--- | :--- |
| **LCD Data Bus** | `P2` (P2.0 - P2.7) | 控制 LCD 的 8-bit 資料傳輸 |
| **LCD RS** | `P1.0` | LCD 暫存器選擇 (Register Select) |
| **LCD RW** | `P1.1` | LCD 讀寫控制 (Read/Write) |
| **LCD E** | `P1.2` | LCD 致能訊號 (Enable) |
| **矩陣鍵盤** | `P0` (P0.0 - P0.7) | 4x4 鍵盤掃描線與資料讀取 |
| **設定按鈕** | `INT0` (P3.2) | 觸發外部中斷，進入調整模式 |

## 🛠️ 軟體架構亮點 (Software Architecture)

*   **前背景架構 (Foreground/Background System)：** 
    為了避免 8051 的記憶體覆疊 (Memory Overlay) 衝突，本專案採用專業的旗標法 (Flag-based) 處理中斷。外部中斷 (ISR) 僅負責極速舉起 `time_adjust_flag` 旗標，複雜的鍵盤掃描與 LCD 刷新邏輯皆安全地交由主程式 (Main Loop) 執行，確保系統穩定不當機。
*   **模組化設計：** 
    將 LCD 驅動 (`LCD_CMD`, `LCD_DATA`)、鍵盤解碼 (`getIntData`) 與時間處理 (`adjtime`) 獨立封裝，提高程式碼的可讀性與可維護性。