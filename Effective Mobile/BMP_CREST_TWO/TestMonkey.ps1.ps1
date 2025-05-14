# Импортируем необходимые типы для взаимодействия с Windows Forms
Add-Type -AssemblyName System.Windows.Forms

# Получаем путь текущего скрипта и формируем путь к исполняемому файлу
$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Definition
$exePath = Join-Path $scriptDir "BMP_CREST_TWO.exe"

# Функция для запуска программы и ввода команд
function Send-Command {
    param (
        [string]$argument,
        [string]$resultText
    )

    # Запускаем процесс с аргументом
    $process = Start-Process -FilePath $exePath -ArgumentList $argument -PassThru

    # Ждем, чтобы окно открылось
    Start-Sleep -Seconds 2

    # Получаем главное окно процесса
    $hwnd = $process.MainWindowHandle
    if ($hwnd -eq 0) {
        Start-Sleep -Seconds 1
        $hwnd = $process.MainWindowHandle
    }

    if ($hwnd -ne 0) {
        # Активируем окно
        [void][System.Windows.Forms.Form]::FromHandle($hwnd).Activate()
    } else {
        Write-Warning "Не удалось активировать окно процесса."
    }

    # Ждем немного перед вводом
    Start-Sleep -Milliseconds 500

    # Отправляем команду
    [System.Windows.Forms.SendKeys]::SendWait("$argument{ENTER}")

    # Ждем, пока завершится вывод (в данном случае 2 секунды, можно настроить)
    Start-Sleep -Seconds 2

    # Вводим результат
    [System.Windows.Forms.SendKeys]::SendWait("$resultText{ENTER}")
}

# Тестируем с аргументом "monkey.bmp"
Send-Command -argument "monkey.bmp" -resultText "Result"
