import { useState } from "react";
import {
    cancelOrder,
    type OrderInfo,
} from "../api/api";

type Props = {
    orders: OrderInfo[];
    onCancelComplete: () => Promise<void> | void;
};

export default function MyOrders({
    orders,
    onCancelComplete,
}: Props) {
    const [cancellingId, setCancellingId] =
        useState<string | null>(null);

    const [error, setError] =
        useState<string | null>(null);

    async function handleCancel(orderId: string) {
        setCancellingId(orderId);
        setError(null);

        try {
            await cancelOrder(orderId);
            await onCancelComplete();
        }
        catch (e) {
            setError(
                e instanceof Error
                    ? e.message
                    : "Cancel failed"
            );
        }
        finally {
            setCancellingId(null);
        }
    }

    return (
        <section className="panel orders-panel">
            <div className="panel-heading">
                <div>
                    <span className="eyebrow">
                        Account
                    </span>

                    <h3>My Orders</h3>
                </div>

                <span className="pill">
                    {orders.length}
                </span>
            </div>

            <div className="orders-head">
                <span>Stock</span>
                <span>Side</span>
                <span>Qty</span>
                <span>Limit</span>
                <span>Status</span>
                <span></span>
            </div>

            <div className="orders-list">
                {orders.length === 0 ? (
                    <div className="orders-empty">
                        No orders yet.
                    </div>
                ) : (
                    orders.map((order) => {
                        const cancellable =
                            order.status === "OPEN" ||
                            order.status === "PARTIALLY_FILLED";

                        return (
                            <div
                                className="order-row"
                                key={order.order_id}
                            >
                                <strong>
                                    {order.stock}
                                </strong>

                                <span
                                    className={
                                        order.side === "BUY"
                                            ? "order-side buy-side"
                                            : "order-side sell-side"
                                    }
                                >
                                    {order.side}
                                </span>

                                <span>
                                    {order.quantity}
                                </span>

                                <span>
                                    ${order.limit_price}
                                </span>

                                <span
                                    className={`order-status ${order.status.toLowerCase()}`}
                                >
                                    {order.status.replaceAll("_", " ")}
                                </span>

                                <div>
                                    {cancellable && (
                                        <button
                                            className="cancel-button"
                                            disabled={
                                                cancellingId ===
                                                order.order_id
                                            }
                                            onClick={() =>
                                                void handleCancel(
                                                    order.order_id
                                                )
                                            }
                                        >
                                            {cancellingId ===
                                                order.order_id
                                                ? "..."
                                                : "Cancel"}
                                        </button>
                                    )}
                                </div>
                            </div>
                        );
                    })
                )}
            </div>

            {error && (
                <div className="status error">
                    {error}
                </div>
            )}
        </section>
    );
}