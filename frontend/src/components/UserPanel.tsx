import type { Me, StockSummary } from "../api/api";

export default function UserPanel({ me, stocks }: { me: Me | null; stocks: StockSummary[] }) {
  if (!me) return <section className="panel"><span className="eyebrow">Account</span><p className="muted">Loading...</p></section>;

  const symbolFor = (id: string) => stocks.find((s) => s.stock_id === id)?.stock_symbol ?? id;

  return (
    <section className="panel">
      <div className="panel-heading">
        <div>
          <span className="eyebrow">Account</span>
          <h3>{me.username}</h3>
        </div>
        <span className="user-id">{me.user_id}</span>
      </div>

      <div className="balance-grid">
        <div><span>Available cash</span><strong>${me.available_cash.toLocaleString()}</strong></div>
        <div><span>Reserved cash</span><strong>${me.reserved_cash.toLocaleString()}</strong></div>
      </div>

      <div className="holdings-title">Holdings</div>
      <div className="holdings">
        {(me.available_stocks ?? []).map((h) => (
          <div className="holding-row" key={h.stock_id}>
            <strong>{symbolFor(h.stock_id)}</strong>
            <span>{h.quantity} shares</span>
          </div>
        ))}
      </div>
    </section>
  );
}
