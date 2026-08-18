import type { StockSummary } from "../api/api";

export default function StockSidebar({
  stocks,
  selectedId,
  onSelect,
}: {
  stocks: StockSummary[];
  selectedId: string | null;
  onSelect: (stock: StockSummary) => void;
}) {
  return (
    <aside className="sidebar">
      <div className="brand">
        <div className="brand-mark">S</div>
        <div>
          <strong>StockSim</strong>
          <span>Simulated Exchange</span>
        </div>
      </div>

      <div className="sidebar-title">Markets</div>

      <div className="stock-list">
        {stocks.map((stock) => (
          <button
            key={stock.stock_id}
            className={`stock-row ${selectedId === stock.stock_id ? "selected" : ""}`}
            onClick={() => onSelect(stock)}
          >
            <div>
              <strong>{stock.stock_symbol}</strong>
              <span>{stock.stock_name}</span>
            </div>
            <div className="stock-price">${stock.market_price}</div>
          </button>
        ))}
      </div>
    </aside>
  );
}
