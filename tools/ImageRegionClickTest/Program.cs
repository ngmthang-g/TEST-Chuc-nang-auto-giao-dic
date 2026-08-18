using System.Diagnostics;
using System.Drawing;
using System.Drawing.Imaging;
using System.Runtime.InteropServices;
using System.Text.Json;
using System.Windows.Forms;

namespace ThanLongImageRegionClickTest;

internal static class Program
{
    [STAThread]
    static void Main()
    {
        Application.SetHighDpiMode(HighDpiMode.PerMonitorV2);
        Application.EnableVisualStyles();
        Application.SetCompatibleTextRenderingDefault(false);
        Application.Run(new MainForm());
    }
}

internal sealed class MainForm : Form
{
    private IntPtr _targetHwnd = IntPtr.Zero;
    private string _targetTitle = "Chưa chọn";
    private Bitmap? _template;
    private string _templateSource = "Chưa có";
    private Rectangle _roiClient;
    private bool _roiSet;
    private bool _busy;

    private readonly Label _lblTarget = new();
    private readonly Label _lblTemplate = new();
    private readonly Label _lblRoi = new();
    private readonly Label _lblStatus = new();
    private readonly NumericUpDown _numThreshold = new();
    private readonly CheckBox _chkRestoreMouse = new();
    private readonly Button _btnFind = new();
    private readonly Button _btnFindClick = new();

    private readonly string _appDir = Path.Combine(
        Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData),
        "ThanLongImageRegionClickTest");
    private string ConfigPath => Path.Combine(_appDir, "config.json");
    private string CapturedTemplatePath => Path.Combine(_appDir, "captured_template.png");

    public MainForm()
    {
        Text = "Thần Long - Click theo vùng + nhận diện ảnh v0.1.0";
        Width = 720;
        Height = 480;
        StartPosition = FormStartPosition.CenterScreen;
        FormBorderStyle = FormBorderStyle.FixedDialog;
        MaximizeBox = false;
        KeyPreview = true;
        Font = new Font("Segoe UI", 10F);

        BuildUi();
        Directory.CreateDirectory(_appDir);
        LoadConfig();

        Shown += (_, _) => RegisterHotKey(Handle, 1, 0, 0x75);
        FormClosed += (_, _) => UnregisterHotKey(Handle, 1);
    }

    private void BuildUi()
    {
        var title = new Label
        {
            Left = 18, Top = 14, Width = 670, Height = 32,
            Text = "TEST DUY NHẤT: tìm ảnh trong vùng rồi click đúng tâm",
            Font = new Font("Segoe UI", 14F, FontStyle.Bold)
        };
        Controls.Add(title);

        var note = new Label
        {
            Left = 18, Top = 48, Width = 670, Height = 42,
            Text = "Không inject / không đọc memory game. Ảnh mẫu phải cùng tỉ lệ render với nút trên màn hình. F6 = Tìm & Click 1 lần.",
            ForeColor = Color.DimGray
        };
        Controls.Add(note);

        var btnTarget = new Button { Left = 18, Top = 96, Width = 205, Height = 36, Text = "1. Chọn cửa sổ sau 1.5s" };
        btnTarget.Click += async (_, _) => await PickTargetWindowAsync();
        Controls.Add(btnTarget);

        _lblTarget.SetBounds(238, 100, 450, 30);
        _lblTarget.Text = "Cửa sổ: Chưa chọn";
        Controls.Add(_lblTarget);

        var btnLoad = new Button { Left = 18, Top = 144, Width = 205, Height = 36, Text = "2A. Chọn ảnh PNG/JPG" };
        btnLoad.Click += (_, _) => LoadTemplateFromFile();
        Controls.Add(btnLoad);

        var btnCapture = new Button { Left = 232, Top = 144, Width = 205, Height = 36, Text = "2B. Chụp nút mẫu" };
        btnCapture.Click += async (_, _) => await CaptureTemplateAsync();
        Controls.Add(btnCapture);

        _lblTemplate.SetBounds(18, 185, 670, 30);
        _lblTemplate.Text = "Ảnh mẫu: Chưa có";
        Controls.Add(_lblTemplate);

        var btnRoi = new Button { Left = 18, Top = 224, Width = 205, Height = 36, Text = "3. Khoanh vùng tìm kiếm" };
        btnRoi.Click += async (_, _) => await SelectRoiAsync();
        Controls.Add(btnRoi);

        _lblRoi.SetBounds(238, 228, 450, 30);
        _lblRoi.Text = "ROI: Chưa chọn";
        Controls.Add(_lblRoi);

        var lblTh = new Label { Left = 18, Top = 277, Width = 150, Height = 28, Text = "Ngưỡng khớp (%):" };
        Controls.Add(lblTh);
        _numThreshold.SetBounds(166, 274, 88, 30);
        _numThreshold.Minimum = 60;
        _numThreshold.Maximum = 100;
        _numThreshold.DecimalPlaces = 0;
        _numThreshold.Value = 88;
        Controls.Add(_numThreshold);

        _chkRestoreMouse.SetBounds(280, 274, 260, 30);
        _chkRestoreMouse.Text = "Trả chuột về vị trí cũ sau click";
        _chkRestoreMouse.Checked = true;
        Controls.Add(_chkRestoreMouse);

        _btnFind.SetBounds(18, 322, 205, 42);
        _btnFind.Text = "Tìm thử (không click)";
        _btnFind.Click += async (_, _) => await FindAndMaybeClickAsync(false);
        Controls.Add(_btnFind);

        _btnFindClick.SetBounds(232, 322, 205, 42);
        _btnFindClick.Text = "TÌM + CLICK 1 LẦN (F6)";
        _btnFindClick.Font = new Font("Segoe UI", 10F, FontStyle.Bold);
        _btnFindClick.Click += async (_, _) => await FindAndMaybeClickAsync(true);
        Controls.Add(_btnFindClick);

        var btnReset = new Button { Left = 447, Top = 322, Width = 120, Height = 42, Text = "Reset ROI" };
        btnReset.Click += (_, _) => { _roiSet = false; _lblRoi.Text = "ROI: Chưa chọn"; SaveConfig(); };
        Controls.Add(btnReset);

        _lblStatus.SetBounds(18, 382, 670, 54);
        _lblStatus.Text = "Trạng thái: Sẵn sàng";
        _lblStatus.BorderStyle = BorderStyle.FixedSingle;
        _lblStatus.Padding = new Padding(8);
        Controls.Add(_lblStatus);
    }

    protected override void WndProc(ref Message m)
    {
        const int WM_HOTKEY = 0x0312;
        if (m.Msg == WM_HOTKEY && m.WParam.ToInt32() == 1 && !_busy)
        {
            _ = FindAndMaybeClickAsync(true);
        }
        base.WndProc(ref m);
    }

    private async Task PickTargetWindowAsync()
    {
        if (_busy) return;
        _busy = true;
        SetStatus("Trong 1.5 giây hãy click vào cửa sổ game cần test...");
        Hide();
        await Task.Delay(1500);
        var hwnd = GetForegroundWindow();
        Show();
        Activate();
        _busy = false;

        if (hwnd == IntPtr.Zero || hwnd == Handle)
        {
            SetStatus("Không chọn được cửa sổ.");
            return;
        }

        _targetHwnd = hwnd;
        _targetTitle = GetWindowTitle(hwnd);
        _lblTarget.Text = $"Cửa sổ: {_targetTitle} | HWND 0x{hwnd.ToInt64():X}";
        if (_roiSet && GetClientRect(_targetHwnd, out var savedCr) &&
            _roiClient.Right <= savedCr.Right && _roiClient.Bottom <= savedCr.Bottom)
        {
            _lblRoi.Text = $"ROI client đã lưu: X={_roiClient.X}, Y={_roiClient.Y}, W={_roiClient.Width}, H={_roiClient.Height}";
            SetStatus("Đã chọn cửa sổ và dùng lại ROI đã lưu. Có thể khoanh lại nếu cần.");
        }
        else
        {
            _roiSet = false;
            _lblRoi.Text = "ROI: Chưa chọn";
            SetStatus("Đã chọn cửa sổ. Tiếp theo chọn/chụp ảnh mẫu và khoanh ROI.");
        }
        SaveConfig();
    }

    private void LoadTemplateFromFile()
    {
        using var dlg = new OpenFileDialog
        {
            Filter = "Ảnh (*.png;*.jpg;*.jpeg;*.bmp)|*.png;*.jpg;*.jpeg;*.bmp|Tất cả file|*.*",
            Title = "Chọn ảnh mẫu nút A"
        };
        if (dlg.ShowDialog(this) != DialogResult.OK) return;

        try
        {
            using var raw = new Bitmap(dlg.FileName);
            _template?.Dispose();
            _template = new Bitmap(raw.Width, raw.Height, PixelFormat.Format32bppArgb);
            using (var g = Graphics.FromImage(_template)) g.DrawImage(raw, 0, 0, raw.Width, raw.Height);
            _templateSource = dlg.FileName;
            _lblTemplate.Text = $"Ảnh mẫu: {Path.GetFileName(dlg.FileName)} | {_template.Width}x{_template.Height}";
            SaveConfig();
            SetStatus("Đã nạp ảnh mẫu.");
        }
        catch (Exception ex)
        {
            SetStatus("Lỗi mở ảnh: " + ex.Message);
        }
    }

    private async Task CaptureTemplateAsync()
    {
        if (_busy) return;
        _busy = true;
        SetStatus("Kéo chuột khoanh đúng nút A để chụp ảnh mẫu. ESC để hủy.");
        Hide();
        await Task.Delay(150);
        using var overlay = new SelectionOverlay("Kéo khoanh CHÍNH NÚT MẪU A - ESC để hủy");
        var result = overlay.ShowDialog();
        var rect = overlay.SelectedScreenRect;
        await Task.Delay(120);
        Show();
        Activate();
        _busy = false;

        if (result != DialogResult.OK || rect.Width < 4 || rect.Height < 4)
        {
            SetStatus("Đã hủy chụp ảnh mẫu.");
            return;
        }

        try
        {
            using var shot = CaptureScreen(rect);
            _template?.Dispose();
            _template = new Bitmap(shot);
            Directory.CreateDirectory(_appDir);
            _template.Save(CapturedTemplatePath, ImageFormat.Png);
            _templateSource = CapturedTemplatePath;
            _lblTemplate.Text = $"Ảnh mẫu: ảnh chụp | {_template.Width}x{_template.Height}";
            SaveConfig();
            SetStatus("Đã chụp ảnh mẫu. Giờ khoanh vùng mà nút có thể xuất hiện.");
        }
        catch (Exception ex)
        {
            SetStatus("Lỗi chụp ảnh mẫu: " + ex.Message);
        }
    }

    private async Task SelectRoiAsync()
    {
        if (_targetHwnd == IntPtr.Zero || !IsWindow(_targetHwnd))
        {
            SetStatus("Phải chọn cửa sổ trước.");
            return;
        }
        if (_busy) return;
        _busy = true;
        SetStatus("Kéo khoanh vùng mà nút A có thể xuất hiện. ESC để hủy.");

        SetForegroundWindow(_targetHwnd);
        await Task.Delay(150);
        Hide();
        await Task.Delay(120);
        using var overlay = new SelectionOverlay("Kéo khoanh VÙNG TÌM KIẾM - ESC để hủy");
        var result = overlay.ShowDialog();
        var screenRect = overlay.SelectedScreenRect;
        await Task.Delay(120);
        Show();
        Activate();
        _busy = false;

        if (result != DialogResult.OK || screenRect.Width < 8 || screenRect.Height < 8)
        {
            SetStatus("Đã hủy chọn ROI.");
            return;
        }

        if (!TryScreenRectToClient(_targetHwnd, screenRect, out var clientRect))
        {
            SetStatus("ROI không nằm hợp lệ trong vùng client của cửa sổ.");
            return;
        }

        _roiClient = clientRect;
        _roiSet = true;
        _lblRoi.Text = $"ROI client: X={clientRect.X}, Y={clientRect.Y}, W={clientRect.Width}, H={clientRect.Height}";
        SaveConfig();
        SetStatus("Đã chọn ROI theo tọa độ client. Cửa sổ di chuyển vẫn dùng được.");
    }

    private async Task FindAndMaybeClickAsync(bool click)
    {
        if (_busy) return;
        if (_targetHwnd == IntPtr.Zero || !IsWindow(_targetHwnd))
        {
            SetStatus("Cửa sổ mục tiêu không còn hợp lệ. Chọn lại cửa sổ.");
            return;
        }
        if (_template is null)
        {
            SetStatus("Chưa có ảnh mẫu.");
            return;
        }
        if (!_roiSet)
        {
            SetStatus("Chưa khoanh vùng tìm kiếm.");
            return;
        }
        if (_template.Width > _roiClient.Width || _template.Height > _roiClient.Height)
        {
            SetStatus("Ảnh mẫu lớn hơn ROI.");
            return;
        }

        _busy = true;
        ToggleButtons(false);
        var threshold = (double)_numThreshold.Value / 100.0;

        try
        {
            if (IsIconic(_targetHwnd)) ShowWindow(_targetHwnd, 9);
            SetForegroundWindow(_targetHwnd);
            await Task.Delay(150);

            if (!TryClientRectToScreen(_targetHwnd, _roiClient, out var roiScreen))
            {
                SetStatus("Không đổi được ROI sang tọa độ màn hình.");
                return;
            }

            Hide();
            await Task.Delay(100);
            using var roiShot = CaptureScreen(roiScreen);
            Show();

            SetStatus("Đang nhận diện ảnh trong ROI...");
            var templateCopy = new Bitmap(_template);
            var result = await Task.Run(() => TemplateMatcher.FindBest(roiShot, templateCopy));
            templateCopy.Dispose();

            var matchScreen = new Rectangle(
                roiScreen.X + result.Location.X,
                roiScreen.Y + result.Location.Y,
                _template.Width,
                _template.Height);

            if (result.Score < threshold)
            {
                SetStatus($"KHÔNG CLICK. Best match = {result.Score:P1}, thấp hơn ngưỡng {threshold:P0}.");
                return;
            }

            var center = new Point(matchScreen.Left + matchScreen.Width / 2, matchScreen.Top + matchScreen.Height / 2);

            if (!click)
            {
                SetStatus($"TÌM THẤY: {result.Score:P1} | tâm screen=({center.X},{center.Y}) | chưa click.");
                using var marker = new MatchMarker(matchScreen);
                marker.Show();
                await Task.Delay(700);
                marker.Close();
                return;
            }

            var old = Cursor.Position;
            SetForegroundWindow(_targetHwnd);
            await Task.Delay(60);
            Cursor.Position = center;
            MouseClickLeft();
            if (_chkRestoreMouse.Checked)
            {
                await Task.Delay(35);
                Cursor.Position = old;
            }

            SetStatus($"ĐÃ CLICK 1 LẦN | match={result.Score:P1} | tâm=({center.X},{center.Y}).");
        }
        catch (Exception ex)
        {
            Show();
            SetStatus("Lỗi: " + ex.Message);
        }
        finally
        {
            Show();
            _busy = false;
            ToggleButtons(true);
            SaveConfig();
        }
    }

    private void ToggleButtons(bool enabled)
    {
        _btnFind.Enabled = enabled;
        _btnFindClick.Enabled = enabled;
    }

    private void SetStatus(string text)
    {
        if (InvokeRequired)
        {
            BeginInvoke(new Action(() => SetStatus(text)));
            return;
        }
        _lblStatus.Text = "Trạng thái: " + text;
    }

    private static Bitmap CaptureScreen(Rectangle rect)
    {
        if (rect.Width <= 0 || rect.Height <= 0) throw new ArgumentException("ROI không hợp lệ");
        var bmp = new Bitmap(rect.Width, rect.Height, PixelFormat.Format32bppArgb);
        using var g = Graphics.FromImage(bmp);
        g.CopyFromScreen(rect.Left, rect.Top, 0, 0, rect.Size, CopyPixelOperation.SourceCopy);
        return bmp;
    }

    private static bool TryScreenRectToClient(IntPtr hwnd, Rectangle screenRect, out Rectangle clientRect)
    {
        clientRect = Rectangle.Empty;
        if (!GetClientRect(hwnd, out var cr)) return false;
        var p1 = new POINT { X = screenRect.Left, Y = screenRect.Top };
        var p2 = new POINT { X = screenRect.Right, Y = screenRect.Bottom };
        if (!ScreenToClient(hwnd, ref p1) || !ScreenToClient(hwnd, ref p2)) return false;

        var left = Math.Clamp(Math.Min(p1.X, p2.X), 0, cr.Right);
        var top = Math.Clamp(Math.Min(p1.Y, p2.Y), 0, cr.Bottom);
        var right = Math.Clamp(Math.Max(p1.X, p2.X), 0, cr.Right);
        var bottom = Math.Clamp(Math.Max(p1.Y, p2.Y), 0, cr.Bottom);
        if (right - left < 8 || bottom - top < 8) return false;
        clientRect = Rectangle.FromLTRB(left, top, right, bottom);
        return true;
    }

    private static bool TryClientRectToScreen(IntPtr hwnd, Rectangle clientRect, out Rectangle screenRect)
    {
        screenRect = Rectangle.Empty;
        var p1 = new POINT { X = clientRect.Left, Y = clientRect.Top };
        var p2 = new POINT { X = clientRect.Right, Y = clientRect.Bottom };
        if (!ClientToScreen(hwnd, ref p1) || !ClientToScreen(hwnd, ref p2)) return false;
        screenRect = Rectangle.FromLTRB(p1.X, p1.Y, p2.X, p2.Y);
        return screenRect.Width > 0 && screenRect.Height > 0;
    }

    private void LoadConfig()
    {
        try
        {
            if (!File.Exists(ConfigPath)) return;
            var cfg = JsonSerializer.Deserialize<AppConfig>(File.ReadAllText(ConfigPath));
            if (cfg is null) return;
            if (cfg.Threshold is >= 60 and <= 100) _numThreshold.Value = cfg.Threshold;
            _chkRestoreMouse.Checked = cfg.RestoreMouse;
            if (cfg.RoiW > 0 && cfg.RoiH > 0)
            {
                _roiClient = new Rectangle(cfg.RoiX, cfg.RoiY, cfg.RoiW, cfg.RoiH);
                _roiSet = true;
                _lblRoi.Text = $"ROI client đã lưu: X={cfg.RoiX}, Y={cfg.RoiY}, W={cfg.RoiW}, H={cfg.RoiH}";
            }
            if (!string.IsNullOrWhiteSpace(cfg.TemplatePath) && File.Exists(cfg.TemplatePath))
            {
                using var raw = new Bitmap(cfg.TemplatePath);
                _template = new Bitmap(raw);
                _templateSource = cfg.TemplatePath;
                _lblTemplate.Text = $"Ảnh mẫu: {Path.GetFileName(cfg.TemplatePath)} | {_template.Width}x{_template.Height}";
            }
        }
        catch { }
    }

    private void SaveConfig()
    {
        try
        {
            Directory.CreateDirectory(_appDir);
            var cfg = new AppConfig
            {
                Threshold = (int)_numThreshold.Value,
                RestoreMouse = _chkRestoreMouse.Checked,
                TemplatePath = _templateSource != "Chưa có" ? _templateSource : null,
                RoiX = _roiSet ? _roiClient.X : 0,
                RoiY = _roiSet ? _roiClient.Y : 0,
                RoiW = _roiSet ? _roiClient.Width : 0,
                RoiH = _roiSet ? _roiClient.Height : 0
            };
            File.WriteAllText(ConfigPath, JsonSerializer.Serialize(cfg, new JsonSerializerOptions { WriteIndented = true }));
        }
        catch { }
    }

    private sealed class AppConfig
    {
        public int Threshold { get; set; } = 88;
        public bool RestoreMouse { get; set; } = true;
        public string? TemplatePath { get; set; }
        public int RoiX { get; set; }
        public int RoiY { get; set; }
        public int RoiW { get; set; }
        public int RoiH { get; set; }
    }

    private static string GetWindowTitle(IntPtr hwnd)
    {
        var len = GetWindowTextLength(hwnd);
        var sb = new System.Text.StringBuilder(len + 1);
        GetWindowText(hwnd, sb, sb.Capacity);
        return string.IsNullOrWhiteSpace(sb.ToString()) ? "(không có tiêu đề)" : sb.ToString();
    }

    private static void MouseClickLeft()
    {
        var inputs = new INPUT[2];
        inputs[0].type = 0;
        inputs[0].U.mi.dwFlags = 0x0002;
        inputs[1].type = 0;
        inputs[1].U.mi.dwFlags = 0x0004;
        SendInput((uint)inputs.Length, inputs, Marshal.SizeOf<INPUT>());
    }

    [DllImport("user32.dll")] private static extern IntPtr GetForegroundWindow();
    [DllImport("user32.dll")] private static extern bool SetForegroundWindow(IntPtr hWnd);
    [DllImport("user32.dll")] private static extern bool IsWindow(IntPtr hWnd);
    [DllImport("user32.dll")] private static extern bool IsIconic(IntPtr hWnd);
    [DllImport("user32.dll")] private static extern bool ShowWindow(IntPtr hWnd, int nCmdShow);
    [DllImport("user32.dll", CharSet = CharSet.Unicode)] private static extern int GetWindowText(IntPtr hWnd, System.Text.StringBuilder text, int count);
    [DllImport("user32.dll")] private static extern int GetWindowTextLength(IntPtr hWnd);
    [DllImport("user32.dll")] private static extern bool GetClientRect(IntPtr hWnd, out RECT lpRect);
    [DllImport("user32.dll")] private static extern bool ScreenToClient(IntPtr hWnd, ref POINT lpPoint);
    [DllImport("user32.dll")] private static extern bool ClientToScreen(IntPtr hWnd, ref POINT lpPoint);
    [DllImport("user32.dll")] private static extern bool RegisterHotKey(IntPtr hWnd, int id, uint fsModifiers, uint vk);
    [DllImport("user32.dll")] private static extern bool UnregisterHotKey(IntPtr hWnd, int id);
    [DllImport("user32.dll")] private static extern uint SendInput(uint nInputs, INPUT[] pInputs, int cbSize);

    [StructLayout(LayoutKind.Sequential)] private struct RECT { public int Left, Top, Right, Bottom; }
    [StructLayout(LayoutKind.Sequential)] private struct POINT { public int X, Y; }

    [StructLayout(LayoutKind.Sequential)]
    private struct INPUT
    {
        public uint type;
        public InputUnion U;
    }

    [StructLayout(LayoutKind.Explicit)]
    private struct InputUnion
    {
        [FieldOffset(0)] public MOUSEINPUT mi;
    }

    [StructLayout(LayoutKind.Sequential)]
    private struct MOUSEINPUT
    {
        public int dx;
        public int dy;
        public uint mouseData;
        public uint dwFlags;
        public uint time;
        public UIntPtr dwExtraInfo;
    }
}

internal sealed class SelectionOverlay : Form
{
    private Point _start;
    private Point _current;
    private bool _dragging;
    private readonly string _caption;
    public Rectangle SelectedScreenRect { get; private set; } = Rectangle.Empty;

    public SelectionOverlay(string caption)
    {
        _caption = caption;
        FormBorderStyle = FormBorderStyle.None;
        StartPosition = FormStartPosition.Manual;
        Bounds = SystemInformation.VirtualScreen;
        TopMost = true;
        ShowInTaskbar = false;
        BackColor = Color.Black;
        Opacity = 0.22;
        Cursor = Cursors.Cross;
        KeyPreview = true;
        DoubleBuffered = true;

        MouseDown += OnMouseDown;
        MouseMove += OnMouseMove;
        MouseUp += OnMouseUp;
        KeyDown += (_, e) => { if (e.KeyCode == Keys.Escape) { DialogResult = DialogResult.Cancel; Close(); } };
    }

    protected override void OnPaint(PaintEventArgs e)
    {
        base.OnPaint(e);
        using var font = new Font("Segoe UI", 16F, FontStyle.Bold);
        using var brush = new SolidBrush(Color.White);
        e.Graphics.DrawString(_caption, font, brush, 24, 24);

        if (!_dragging) return;
        var r = Normalize(_start, _current);
        using var pen = new Pen(Color.Lime, 3);
        e.Graphics.DrawRectangle(pen, r);
        using var sizeBrush = new SolidBrush(Color.Yellow);
        e.Graphics.DrawString($"{r.Width} x {r.Height}", Font, sizeBrush, r.Left + 5, r.Top + 5);
    }

    private void OnMouseDown(object? sender, MouseEventArgs e)
    {
        if (e.Button != MouseButtons.Left) return;
        _start = e.Location;
        _current = e.Location;
        _dragging = true;
        Invalidate();
    }

    private void OnMouseMove(object? sender, MouseEventArgs e)
    {
        if (!_dragging) return;
        _current = e.Location;
        Invalidate();
    }

    private void OnMouseUp(object? sender, MouseEventArgs e)
    {
        if (!_dragging || e.Button != MouseButtons.Left) return;
        _dragging = false;
        _current = e.Location;
        var local = Normalize(_start, _current);
        if (local.Width < 4 || local.Height < 4)
        {
            Invalidate();
            return;
        }
        var p = PointToScreen(local.Location);
        SelectedScreenRect = new Rectangle(p, local.Size);
        DialogResult = DialogResult.OK;
        Close();
    }

    private static Rectangle Normalize(Point a, Point b) => Rectangle.FromLTRB(
        Math.Min(a.X, b.X), Math.Min(a.Y, b.Y), Math.Max(a.X, b.X), Math.Max(a.Y, b.Y));
}

internal sealed class MatchMarker : Form
{
    public MatchMarker(Rectangle screenRect)
    {
        FormBorderStyle = FormBorderStyle.None;
        StartPosition = FormStartPosition.Manual;
        Bounds = Rectangle.Inflate(screenRect, 3, 3);
        TopMost = true;
        ShowInTaskbar = false;
        BackColor = Color.Magenta;
        TransparencyKey = Color.Magenta;
    }

    protected override void OnPaint(PaintEventArgs e)
    {
        using var pen = new Pen(Color.Lime, 3);
        e.Graphics.DrawRectangle(pen, 1, 1, Width - 3, Height - 3);
    }
}

internal readonly record struct MatchResult(Point Location, double Score);

internal static class TemplateMatcher
{
    private readonly record struct Anchor(int X, int Y);
    private readonly record struct Candidate(int X, int Y, long AnchorDiff);

    public static MatchResult FindBest(Bitmap haystackInput, Bitmap needleInput)
    {
        using var haystack = ToArgb(haystackInput);
        using var needle = ToArgb(needleInput);
        if (needle.Width > haystack.Width || needle.Height > haystack.Height)
            return new MatchResult(Point.Empty, 0);

        var h = ReadBytes(haystack, out var hStride);
        var n = ReadBytes(needle, out var nStride);
        var anchors = BuildAnchors(n, nStride, needle.Width, needle.Height);
        if (anchors.Count == 0) anchors.Add(new Anchor(needle.Width / 2, needle.Height / 2));

        const int keepCount = 32;
        var bestCandidates = new List<Candidate>(keepCount);
        var maxX = haystack.Width - needle.Width;
        var maxY = haystack.Height - needle.Height;

        for (var y = 0; y <= maxY; y++)
        {
            for (var x = 0; x <= maxX; x++)
            {
                long diff = 0;
                foreach (var a in anchors)
                {
                    var hi = (y + a.Y) * hStride + (x + a.X) * 4;
                    var ni = a.Y * nStride + a.X * 4;
                    diff += Math.Abs(h[hi] - n[ni]);
                    diff += Math.Abs(h[hi + 1] - n[ni + 1]);
                    diff += Math.Abs(h[hi + 2] - n[ni + 2]);
                }

                if (bestCandidates.Count < keepCount)
                {
                    bestCandidates.Add(new Candidate(x, y, diff));
                    if (bestCandidates.Count == keepCount)
                        bestCandidates.Sort((a, b) => a.AnchorDiff.CompareTo(b.AnchorDiff));
                }
                else if (diff < bestCandidates[^1].AnchorDiff)
                {
                    bestCandidates[^1] = new Candidate(x, y, diff);
                    bestCandidates.Sort((a, b) => a.AnchorDiff.CompareTo(b.AnchorDiff));
                }
            }
        }

        var bestScore = -1.0;
        var bestPoint = Point.Empty;
        var pixelStep = needle.Width * needle.Height > 45000 ? 2 : 1;

        foreach (var c in bestCandidates)
        {
            long diff = 0;
            long count = 0;
            for (var y = 0; y < needle.Height; y += pixelStep)
            {
                var hBase = (c.Y + y) * hStride + c.X * 4;
                var nBase = y * nStride;
                for (var x = 0; x < needle.Width; x += pixelStep)
                {
                    var hi = hBase + x * 4;
                    var ni = nBase + x * 4;
                    var alpha = n[ni + 3];
                    if (alpha < 32) continue;
                    diff += Math.Abs(h[hi] - n[ni]);
                    diff += Math.Abs(h[hi + 1] - n[ni + 1]);
                    diff += Math.Abs(h[hi + 2] - n[ni + 2]);
                    count += 3;
                }
            }

            if (count == 0) continue;
            var score = 1.0 - (double)diff / (count * 255.0);
            if (score > bestScore)
            {
                bestScore = score;
                bestPoint = new Point(c.X, c.Y);
            }
        }

        return new MatchResult(bestPoint, Math.Clamp(bestScore, 0, 1));
    }

    private static List<Anchor> BuildAnchors(byte[] pixels, int stride, int width, int height)
    {
        var result = new List<(Anchor A, int Strength)>();
        const int gx = 8;
        const int gy = 6;
        for (var iy = 0; iy < gy; iy++)
        {
            var y = Math.Clamp((int)Math.Round((iy + 0.5) * height / gy), 0, height - 1);
            for (var ix = 0; ix < gx; ix++)
            {
                var x = Math.Clamp((int)Math.Round((ix + 0.5) * width / gx), 0, width - 1);
                var i = y * stride + x * 4;
                if (pixels[i + 3] < 32) continue;
                var b = pixels[i];
                var g = pixels[i + 1];
                var r = pixels[i + 2];
                var max = Math.Max(r, Math.Max(g, b));
                var min = Math.Min(r, Math.Min(g, b));
                var saturation = max - min;
                var brightnessEdge = Math.Abs(((r + g + b) / 3) - 128);
                result.Add((new Anchor(x, y), saturation * 2 + brightnessEdge));
            }
        }
        return result.OrderByDescending(x => x.Strength).Take(36).Select(x => x.A).ToList();
    }

    private static Bitmap ToArgb(Bitmap input)
    {
        var bmp = new Bitmap(input.Width, input.Height, PixelFormat.Format32bppArgb);
        using var g = Graphics.FromImage(bmp);
        g.DrawImageUnscaled(input, 0, 0);
        return bmp;
    }

    private static byte[] ReadBytes(Bitmap bmp, out int stride)
    {
        var rect = new Rectangle(0, 0, bmp.Width, bmp.Height);
        var data = bmp.LockBits(rect, ImageLockMode.ReadOnly, PixelFormat.Format32bppArgb);
        try
        {
            stride = data.Stride;
            var bytes = new byte[Math.Abs(stride) * bmp.Height];
            Marshal.Copy(data.Scan0, bytes, 0, bytes.Length);
            return bytes;
        }
        finally
        {
            bmp.UnlockBits(data);
        }
    }
}
