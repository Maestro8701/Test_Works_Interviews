using Binance.Net.Clients;
using Bitget.Net.Clients;
using Bybit.Net.Clients;
using Kucoin.Net.Clients;

public interface IExchange
{
    Task<string> GetPrice(); //async 
}

public class BinanceExchange : IExchange
{
    public BinanceExchange() { }
    public async Task<string> GetPrice()
    {
        var client = new BinanceRestClient();
        var price = await client.SpotApi.ExchangeData.GetPriceAsync("ETHUSDT");
        if (price.Success)
        {
            return price.Data.Price.ToString();
        }
        else
        {
            return $"Ошибка получения цены с Binance: {price.Error}";
        }
    }
}

public class BybitExchange : IExchange
{
    public BybitExchange() { }

    public async Task<string> GetPrice()
    {
        var restClient = new BybitRestClient();
        var ticker = await restClient.V5Api.ExchangeData.GetSpotTickersAsync("ETHUSDT");
        if (ticker.Success)
        {
            return ticker.Data.List.First().LastPrice.ToString();
        }
        else
        {
            return $"Ошибка получения цены с Binance: {ticker.Error}";
        }
    }
}

class KucoinExchange : IExchange
{

    public KucoinExchange() { }

    public async Task<string> GetPrice()
    {
        var restClient = new KucoinRestClient();
        var ticker = await restClient.SpotApi.ExchangeData.GetTickerAsync("ETH-USDT");
        if (ticker.Success)
        {
            return ticker.Data.LastPrice.ToString();
        }
        else
        {
            return $"Ошибка получения цены с Binance: {ticker.Error}";
        }
    }
}

public class BitgetExchange : IExchange
{
    public BitgetExchange() { }

    public async Task<string> GetPrice()
    {
        var restClient = new BitgetRestClient();
        var ticker = await restClient.SpotApi.ExchangeData.GetTickerAsync("ETHUSDT_SPBL");
        if (ticker.Success)
        {
            return ticker.Data.ClosePrice.ToString();
        }
        else
        {
            return $"Ошибка получения цены с Binance: {ticker.Error}";

        }
    }
}



