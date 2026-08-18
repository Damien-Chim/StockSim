import { useEffect, useState } from "react";
import { buyStock, sellStock, type StockSummary } from "../api/api";

export default function OrderTicket({
  stock,
  onOrderComplete,
}: {
  stock: StockSummary;
  onOrderComplete: () => Promise<void> | void;
}) {
  const [side, setSide] = useState<"buy" | "sell">("buy");
  const [quantity, setQuantity] = useState("1");
  const [limitPrice, setLimitPrice] = useState(String(stock.market_price));
  const [message, setMessage] = useState<string | null>(null);
  const [error, setError] = useState<string | null>(null);

  useEffect(() => {
    setLimitPrice(String(stock.market_price));
    setMessage(null);
    setError(null);
  }, [stock.stock_id, stock.market_price]);

  async function submit() {
    const qty = Number(quantity);
    const price = Number(limitPrice);
    setMessage(null);
    setError(null);

    try {
      const result =
        side === "buy"
          ? await buyStock(stock.stock_id, qty, price)
          : await sellStock(stock.stock_id, qty, price);

      setMessage(result.message);
      await onOrderComplete();
    } catch (e) {
      setError(e instanceof Error ? e.message : "Order failed");
    }
  }

  return (
    <section className="panel">
      <div className="panel-heading">
        <div>
          <span className="eyebrow">Order ticket</span>
          <h3>{stock.stock_symbol}</h3>
        </div>
        <span className="pill">${stock.market_price}</span>
      </div>

      <div className="side-switch">
        <button className={side === "buy" ? "active buy" : ""} onClick={() => setSide("buy")}>Buy</button>
        <button className={side === "sell" ? "active sell" : ""} onClick={() => setSide("sell")}>Sell</button>
      </div>

      <label>
        Quantity
        <input value={quantity} onChange={(e) => setQuantity(e.target.value)} />
      </label>

      <label>
        Limit price
        <input value={limitPrice} onChange={(e) => setLimitPrice(e.target.value)} />
      </label>

      <button className={`submit-order ${side}`} onClick={submit}>
        {side === "buy" ? "Buy" : "Sell"} {stock.stock_symbol}
      </button>

      {message && <div className="status ok">{message}</div>}
      {error && <div className="status error">{error}</div>}
    </section>
  );
}
