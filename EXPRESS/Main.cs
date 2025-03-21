using Binance.Net.Clients;
using Bybit.Net.Clients;
using Kucoin.Net.Clients;
using Bitget.Net.Clients;
using Binance.Net;
using Bitget.Net;
using Bybit.Net;
using Kucoin.Net;

/*тут комментарии по коду и превью, я  не знаю очевидные они или нет но мб пригодятся колонки загружаются последовательно поэтому если колонка пустая то нужно подождать 
 * и либо вылезет цена либо сообщение об ошибке, окно можно развернуть и масштабирование сработает на панели
 Касательно меня я С++ разработчик 1,6 года делаю тестовые в том числе на шарпе так как считаю с минимальной подготовкой могу написать код который будет как минимум средним на шарпе
(первое тестовое так и вышло у меня писал я его имея дней 10 знакомства с шарпом). С биткоином знаком до задания минимально никак был как я и говорил пару тестовых писал на С++, с сетью также был знаком
ничего на плюсах что касается сетевого взаимодействия не писал поэтому это мое первое сетевое задание, также я впервые узнал что такое WinForms тут и много чего интересного. 
Стартовые условия очень сложные то есть, мне надо было паралельно писать тестовое на C++(с библиотеками которы я первый раз вижу SFML и OPENGL) и код в школе 21 на C, часа 3 максимум 
мог в неделю тратить на задание на это задание шарпе, поэтому считаю что за неделю выкатил это неплохой успех ориентации в средне знакомой и агрессивной среде. 
Я нашел как сделать получение через сокет и как сделать выбор пар, поэтому если есть еще время могу добить но не стал добавлять так как 
код может быть сломается где нибудь и я не успею сдать поэтому сдаю минимальное ядро которое работате и может показать какой-то результат
Нехватка опыта замедляет меня(хотят тут все на поверхности и обьективно ничего сложного нет) и я мог бы все сделать на максимум но тупо из-за загруза, я не успел все допилить.
Буду рад как минимум обратной связи по код и в принципе а будет хорошо если будет возможность еще че нибудь допилить.  
 */
public class MainForm : Form
{
    private List<IExchange> _exchanges = new List<IExchange>
        {
        new BinanceExchange(),
        new BybitExchange(),
        new KucoinExchange(),
                new BitgetExchange()
        };
    private System.Windows.Forms.Timer updateTimer;

    private Panel binancePanel;
    private Label binanceLabel;

    private Panel bybitPanel;
    private Label bybitLabel;

    private Panel kucoinPanel;
    private Label kucoinLabel;

    private Panel bitgetPanel;
    private Label bitgetLabel;

    public MainForm()
    {
        // Установка свойств формы
        this.Text = "Crypto Price Tracker";
        this.Size = new Size(800, 600); // Установка размера формы
        this.StartPosition = FormStartPosition.CenterScreen; // Центрирование формы
        this.Resize += MainForm_Resize;
        CreatePanels();
        updateTimer = new System.Windows.Forms.Timer();
        updateTimer.Interval = 5000;
        updateTimer.Tick += async (sender, e) => await GetPrices();
    }

    private void CreatePanels()
    {
        // Создаем панели для каждой биржи
        binancePanel = CreatePanel(Color.LightGray, "Binance");
        bybitPanel = CreatePanel(Color.LightBlue, "Bybit");
        kucoinPanel = CreatePanel(Color.LightGreen, "Kucoin");
        bitgetPanel = CreatePanel(Color.LightCoral, "Bitget");

        // Добавляем панели на форму
        this.Controls.Add(binancePanel);
        this.Controls.Add(bybitPanel);
        this.Controls.Add(kucoinPanel);
        this.Controls.Add(bitgetPanel);
    }

    private void MainForm_Resize(object sender, EventArgs e)
    {
        int panelHeight = this.ClientSize.Height / 4;

        binancePanel.Size = new Size(this.ClientSize.Width, panelHeight);
        binancePanel.Location = new Point(0, 0);

        bybitPanel.Size = new Size(this.ClientSize.Width, panelHeight);
        bybitPanel.Location = new Point(0, panelHeight);

        kucoinPanel.Size = new Size(this.ClientSize.Width, panelHeight);
        kucoinPanel.Location = new Point(0, panelHeight * 2);

        bitgetPanel.Size = new Size(this.ClientSize.Width, panelHeight);
        bitgetPanel.Location = new Point(0, panelHeight * 3);
        
    }

    private Panel CreatePanel(Color color, string exchangeName)
    {
        var panel = new Panel
        {
            Dock = DockStyle.Top,
            Height = this.ClientSize.Height / 4,
            BackColor = color
        };

        var label = new Label
        {
            Dock = DockStyle.Fill,
            Font = new Font("Arial", 24, FontStyle.Bold),
            TextAlign = ContentAlignment.MiddleCenter
        };

        panel.Controls.Add(label);

        // Устанавливаем текст для каждой биржи
        switch (exchangeName)
        {
            case "Binance":
                binanceLabel = label;
                break;
            case "Bybit":
                bybitLabel = label;
                break;
            case "Kucoin":
                kucoinLabel = label;
                break;
            case "Bitget":
                bitgetLabel = label;
                break;
        }

        return panel;
    }

    protected override async void OnLoad(EventArgs e)
    {
        base.OnLoad(e);
        await GetPrices();
        updateTimer.Start();
    }

    protected override void OnFormClosed(FormClosedEventArgs e)
    {
        updateTimer.Stop();
        base.OnFormClosed(e);
    }

    private async Task GetPrices()
    {
        foreach (var exchange in _exchanges)
        {
            string price = await exchange.GetPrice();
            switch (exchange.GetType().Name)
            {
                case "BinanceExchange":
                    binanceLabel.Invoke((MethodInvoker)(() => binanceLabel.Text = $"Binance: {price}"));
                    break;
                case "BybitExchange":
                    bybitLabel.Invoke((MethodInvoker)(() => bybitLabel.Text = $"Bybit: {price}"));
                    break;
                case "KucoinExchange":
                    kucoinLabel.Invoke((MethodInvoker)(() => kucoinLabel.Text = $"Kucoin: {price}"));
                    break;
                case "BitgetExchange":
                    bitgetLabel.Invoke((MethodInvoker)(() => bitgetLabel.Text = $"Bitget: {price}"));
                    break;

            }
        }
    }
}

class Program
{
    static async Task Main(string[] args)
    {

        Application.EnableVisualStyles();
        Application.SetCompatibleTextRenderingDefault(false);
        Application.Run(new MainForm()); 




        //Дублирующая проверка в консоли получения 
        //Binance 
        var client = new BinanceRestClient();
        var price = await client.SpotApi.ExchangeData.GetPriceAsync("ETHUSDT");
        Console.WriteLine($"Binance Rest client ticker price for ETHUSDT: {price.Data.Price}");


        // Bybit
        {
            var restClient = new BybitRestClient();
            var ticker = await restClient.V5Api.ExchangeData.GetSpotTickersAsync("ETHUSDT");
            Console.WriteLine($"Bybit Rest client ticker price for ETHUSDT: {ticker.Data.List.First().LastPrice}");
        }

        // Kucoin
        {
            var restClient = new KucoinRestClient();
            var ticker = await restClient.SpotApi.ExchangeData.GetTickerAsync("ETH-USDT");
            Console.WriteLine($"Kucoin Rest client ticker price for ETHUSDT: {ticker.Data.LastPrice}");
        }

        // Bitget
        {
            var restClient = new BitgetRestClient();
            var ticker = await restClient.SpotApi.ExchangeData.GetTickerAsync("ETHUSDT_SPBL");
            Console.WriteLine($"Bitget Rest client ticker price for ETHUSDT: {ticker.Data.ClosePrice}");
        }
    }
}



