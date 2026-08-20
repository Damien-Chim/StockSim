import type { Me, StockSummary } from "../api/api";

export default function UserPanel({
  me,
  stocks,
}: {
  me: Me | null;
  stocks: StockSummary[];
}) {
  if (!me) {
    return (
      <section className="panel">
        <span className="eyebrow">Account</span>
        <p className="muted">Loading...</p>
      </section>
    );
  }

  const symbolFor = (id: string) =>
    stocks.find((stock) => stock.stock_id === id)?.stock_symbol ?? id;

  const holdings = me.available_stocks ?? [];

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
        <div>
          <span>Available cash</span>
          <strong>${me.available_cash.toLocaleString()}</strong>
        </div>
        <div>
          <span>Reserved cash</span>
          <strong>${me.reserved_cash.toLocaleString()}</strong>
        </div>
      </div>

      <div className="holdings-title">Holdings</div>

      <div className="holdings">
        {holdings.length === 0 ? (
          <p className="muted">No available holdings.</p>
        ) : (
          holdings.map((holding) => (
            <div className="holding-row" key={holding.stock_id}>
              <strong>{symbolFor(holding.stock_id)}</strong>
              <span>{holding.quantity} shares</span>
            </div>
          ))
        )}
      </div>
    </section>
  );
}
