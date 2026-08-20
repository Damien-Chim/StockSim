import type { Trade } from "../api/api";

export default function RecentTrades({ trades }: { trades: Trade[] }) {
  const recent = [...trades].reverse().slice(0, 12);

  return (
    <section className="panel trades-panel">
      <div className="panel-heading">
        <div>
          <span className="eyebrow">Tape</span>
          <h3>Recent trades</h3>
        </div>
      </div>

      <div className="trade-head">
        <span>Time</span>
        <span>Price</span>
        <span>Qty</span>
      </div>

      {recent.length === 0 ? (
        <div className="muted empty-row">No executed trades yet.</div>
      ) : (
        recent.map((trade, index) => (
          <div className="trade-row" key={`${trade.timestamp}-${index}`}>
            <span>{new Date(trade.timestamp * 1000).toLocaleTimeString()}</span>
            <strong>${trade.price}</strong>
            <span>{trade.quantity}</span>
          </div>
        ))
      )}
    </section>
  );
}
