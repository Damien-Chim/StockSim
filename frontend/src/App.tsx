import { useEffect, useState } from "react";
import "./App.css";

import {
    getCandles,
    getMe,
    getMyOrders,
    getStockBasicInfo,
    getStocks,
    getTrades,
    type Candle,
    type Me,
    type OrderInfo,
    type StockSummary,
    type Trade,
} from "./api/api";

import CandlestickChart from "./components/CandlestickChart";
import OrderTicket from "./components/OrderTicket";
import RecentTrades from "./components/RecentTrades";
import StockSidebar from "./components/StockSidebar";
import UserPanel from "./components/UserPanel";
import MyOrders from "./components/MyOrders";

export default function App() {
    const [stocks, setStocks] = useState<StockSummary[]>([]);
    const [selectedId, setSelectedId] = useState<string | null>(null);
    const [selectedStock, setSelectedStock] =
        useState<StockSummary | null>(null);

    const [candles, setCandles] = useState<Candle[]>([]);
    const [trades, setTrades] = useState<Trade[]>([]);
    const [me, setMe] = useState<Me | null>(null);
    const [error, setError] = useState<string | null>(null);
    const [orders, setOrders] = useState<OrderInfo[]>([]);

    // Initial application load
    useEffect(() => {
        let cancelled = false;

        async function loadInitialData() {
            try {
                const [userData, orderData] = await Promise.all([
                    getMe(),
                    getMyOrders(),
                ]);

                if (!cancelled) {
                    setMe(userData);
                    setOrders(orderData);
                }
            }
            catch (e) {
                console.error("Failed to load account data:", e);
            }

            try {
                const stockData = await getStocks();

                if (cancelled) {
                    return;
                }

                setStocks(stockData);

                if (stockData.length > 0) {
                    setSelectedId(stockData[0].stock_id);
                }
            }
            catch (e) {
                if (!cancelled) {
                    setError(
                        e instanceof Error
                            ? e.message
                            : "Failed to load StockSim"
                    );
                }
            }
        }

        void loadInitialData();

        return () => {
            cancelled = true;
        };
    }, []);

    // Load + poll currently selected stock
    useEffect(() => {
        if (selectedId === null) {
            return;
        }

        let cancelled = false;

        async function loadSelectedStock() {
            try {
                const [
                    stockData,
                    basic,
                    candleData,
                    tradeData,
                    userData,
                    orderData
                ] = await Promise.all([
                    getStocks(),
                    getStockBasicInfo(selectedId!),
                    getCandles(selectedId!),
                    getTrades(selectedId!),
                    getMe(),
                    getMyOrders(),
                ]);

                if (cancelled) {
                    return;
                }

                setStocks(stockData);
                setSelectedStock(basic);
                setCandles(candleData);
                setTrades(tradeData);
                setMe(userData);
                setOrders(orderData);
            }
            catch (e) {
                if (!cancelled) {
                    setError(
                        e instanceof Error
                            ? e.message
                            : "Failed to load stock"
                    );
                }
            }
        }

        void loadSelectedStock();

        const intervalId = window.setInterval(() => {
            void loadSelectedStock();
        }, 1000);

        return () => {
            cancelled = true;
            window.clearInterval(intervalId);
        };
    }, [selectedId]);

    // Called after BUY / SELL.
    // This is an event-driven function, so updating state here is perfectly fine.
    async function refreshAfterOrder() {
        if (selectedId === null) {
            return;
        }

        try {
            const [
                stockData,
                basic,
                candleData,
                tradeData,
                userData,
                orderData,
            ] = await Promise.all([
                getStocks(),
                getStockBasicInfo(selectedId),
                getCandles(selectedId),
                getTrades(selectedId),
                getMe(),
                getMyOrders(),
            ]);

            setStocks(stockData);
            setSelectedStock(basic);
            setCandles(candleData);
            setTrades(tradeData);
            setMe(userData);
            setOrders(orderData);
        }
        catch (e) {
            setError(
                e instanceof Error
                    ? e.message
                    : "Failed to refresh data"
            );
        }
    }

    const selectedFromList =
        stocks.find(
            (stock) => stock.stock_id === selectedId
        ) ?? null;

    const stock =
        selectedStock ?? selectedFromList;

    return (
        <div className="app-shell">
            <StockSidebar
                stocks={stocks}
                selectedId={selectedId}
                onSelect={(selected) => {
                    setSelectedId(selected.stock_id);
                    setSelectedStock(selected);
                }}
            />

            <main className="main">
                <header className="topbar">
                    <div>
                        <span className="eyebrow">
                            Live simulated market
                        </span>

                        <h1>
                            {stock
                                ? `${stock.stock_symbol} · ${stock.stock_name}`
                                : "StockSim"}
                        </h1>
                    </div>

                    {stock && (
                        <div className="market-price">
                            <span>Last price</span>
                            <strong>
                                ${stock.market_price}
                            </strong>
                        </div>
                    )}
                </header>

                {error && (
                    <div className="global-error">
                        {error}

                        <button
                            onClick={() => setError(null)}
                        >
                            ×
                        </button>
                    </div>
                )}

                {stock ? (
                    <div className="workspace">
                        <section className="panel chart-panel">
                            <div className="chart-heading">
                                <div>
                                    <span className="eyebrow">
                                        10 second candles
                                    </span>

                                    <h2>
                                        Price Chart
                                    </h2>
                                </div>

                                <div className="live-dot">
                                    <span />
                                    LIVE
                                </div>
                            </div>

                            <CandlestickChart
                                candles={candles}
                            />
                        </section>

                        <div className="right-column">
                            <OrderTicket
                                key={stock.stock_id}
                                stock={stock}
                                onOrderComplete={refreshAfterOrder}
                            />

                            <UserPanel
                                me={me}
                                stocks={stocks}
                            />
                        </div>

                        <div className="bottom-grid">
                            <RecentTrades trades={trades} />

                            <MyOrders
                                orders={orders}
                                onCancelComplete={refreshAfterOrder}
                            />
                        </div>

                    </div>
                ) : (
                    <div className="loading">
                        Loading markets...
                    </div>
                )}
            </main>
        </div>
    );
}