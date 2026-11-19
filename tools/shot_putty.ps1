# shot_putty.ps1 - capture ONLY the PuTTY window (not the whole screen)
#
# Usage:  powershell -NoProfile -ExecutionPolicy Bypass -File shot_putty.ps1 <output.png>
#
# Why: a full-screen grab picked up whatever window was in front (an unrelated
# application), which is useless as report evidence. This script finds the
# PuTTY window by its title, brings it to the foreground and copies only its
# client area.

param([Parameter(Mandatory=$true)][string]$Out)

Add-Type -AssemblyName System.Windows.Forms, System.Drawing
Add-Type @"
using System;
using System.Runtime.InteropServices;
public class W {
  [DllImport("user32.dll")] public static extern IntPtr FindWindow(string c, string t);
  [DllImport("user32.dll")] public static extern bool EnumWindows(EnumProc f, IntPtr p);
  [DllImport("user32.dll")] public static extern int GetWindowTextLength(IntPtr h);
  [DllImport("user32.dll")] public static extern int GetWindowText(IntPtr h, System.Text.StringBuilder s, int n);
  [DllImport("user32.dll")] public static extern bool IsWindowVisible(IntPtr h);
  [DllImport("user32.dll")] public static extern bool GetClientRect(IntPtr h, out RECT r);
  [DllImport("user32.dll")] public static extern bool ClientToScreen(IntPtr h, ref POINT p);
  [DllImport("user32.dll")] public static extern bool SetForegroundWindow(IntPtr h);
  [DllImport("user32.dll")] public static extern bool SetProcessDPIAware();
  [DllImport("user32.dll")] public static extern bool ShowWindow(IntPtr h, int c);
  public delegate bool EnumProc(IntPtr h, IntPtr p);
  public struct RECT { public int L, T, R, B; }
  public struct POINT { public int X, Y; }
}
"@

[void][W]::SetProcessDPIAware()   # physical pixels, otherwise the grab is shifted
$found = [IntPtr]::Zero
$cb = [W+EnumProc]{
    param($h, $p)
    if ([W]::IsWindowVisible($h)) {
        $len = [W]::GetWindowTextLength($h)
        if ($len -gt 0) {
            $sb = New-Object System.Text.StringBuilder ($len + 1)
            [void][W]::GetWindowText($h, $sb, $sb.Capacity)
            if ($sb.ToString() -match 'PuTTY') { $script:found = $h; return $false }
        }
    }
    return $true
}
[void][W]::EnumWindows($cb, [IntPtr]::Zero)

if ($found -eq [IntPtr]::Zero) { Write-Output "ERROR: PuTTY window not found"; exit 1 }

[void][W]::ShowWindow($found, 9)          # SW_RESTORE
[void][W]::SetForegroundWindow($found)
Start-Sleep -Milliseconds 400

$r = New-Object W+RECT
[void][W]::GetClientRect($found, [ref]$r)
$pt = New-Object W+POINT
[void][W]::ClientToScreen($found, [ref]$pt)
$w = $r.R - $r.L
$h = $r.B - $r.T
if ($w -le 0 -or $h -le 0) { Write-Output "ERROR: empty client rect"; exit 1 }

$bmp = New-Object System.Drawing.Bitmap $w, $h
$g = [System.Drawing.Graphics]::FromImage($bmp)
$g.CopyFromScreen($pt.X, $pt.Y, 0, 0, $bmp.Size)
$bmp.Save($Out, [System.Drawing.Imaging.ImageFormat]::Png)
$g.Dispose(); $bmp.Dispose()

Write-Output ("saved {0}  ({1}x{2})" -f $Out, $w, $h)
