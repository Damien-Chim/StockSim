const API_BASE = import.meta.env.VITE_API_URL ?? "http://127.0.0.1:8080";

export type StockSummary = {
  stock_id: string;
  stock_name: string;
  stock_symbol: string;
  market_price: number;
  best_bid?: number | null;
  best_ask?: number | null;
};

export type Candle = {
  start_time: number;
  open: number;
  high: number;
  low: number;
  close: number;
  volume: number;
};

export type Trade = {
  timestamp: number;
  quantity: number;
  price: number;
};

export type Holding = {
  stock_id: string;
  quantity: number;
};

export type Me = {
  user_id: string;
  username: string;
  available_cash: number;
  reserved_cash: number;
  available_stocks?: Holding[];
  reserved_stocks?: Holding[];
};

async function request<T>(path: string, options?: RequestInit): Promise<T> {
  const response = await fetch(`${API_BASE}${path}`, {
    ...options,
    headers: {
      "Content-Type": "application/json",
      ...(options?.headers ?? {}),
    },
  });

  const body = await response.json().catch(() => null);

  if (!response.ok) {
    throw new Error(body?.error ?? `Request failed (${response.status})`);
  }

  return body as T;
}

export const getStocks = () => request<StockSummary[]>("/api/stocks");
export const getStockBasicInfo = (stockId: string) =>
  request<StockSummary>(`/api/stocks/${stockId}/basic_info`);
export const getCandles = (stockId: string) =>
  request<Candle[]>(`/api/stocks/${stockId}/candles`);
export const getTrades = (stockId: string) =>
  request<Trade[]>(`/api/stocks/${stockId}/trades`);
export const getMe = () => request<Me>("/api/me");

export function buyStock(stockId: string, quantity: number, limitPrice: number) {
  return request<{ message: string }>("/api/orders/buy", {
    method: "POST",
    body: JSON.stringify({ stock_id: stockId, quantity, limit_price: limitPrice }),
  });
}

export function sellStock(stockId: string, quantity: number, limitPrice: number) {
  return request<{ message: string }>("/api/orders/sell", {
    method: "POST",
    body: JSON.stringify({ stock_id: stockId, quantity, limit_price: limitPrice }),
  });
}

export function cancelOrder(orderId: string) {
    return request<{ message: string }>("/api/orders/cancel", {
        method: "POST",
        body: JSON.stringify({
            order_id: orderId,
        }),
    });
}

export type OrderInfo = {
    order_id: string;
    stock: string;
    side: "BUY" | "SELL";
    quantity: number;
    limit_price: number;
    status:
    | "OPEN"
    | "PARTIALLY_FILLED"
    | "FILLED"
    | "CANCELED";
};

export function getMyOrders() {
    return request<OrderInfo[]>("/api/me/orders");
}
