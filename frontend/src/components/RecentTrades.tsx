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
        <span className="pill">{trades.length}</span>
      </div>

      <div className="trade-head"><span>Time</span><span>Price</span><span>Qty</span></div>

      {recent.length === 0 ? (
        <div className="muted empty-row">No executed trades yet.</div>
      ) : recent.map((t, i) => (
        <div className="trade-row" key={`${t.timestamp}-${i}`}>
          <span>{new Date(t.timestamp * 1000).toLocaleTimeString()}</span>
          <strong>${t.price}</strong>
          <span>{t.quantity}</span>
        </div>
      ))}
    </section>
  );
}
