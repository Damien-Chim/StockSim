import { useState } from "react";
import { cancelOrder } from "../api/api";

type Props = {
    onCancelComplete: () => Promise<void> | void;
};

export default function CancelOrder({
    onCancelComplete,
}: Props) {
    const [orderId, setOrderId] = useState("");
    const [message, setMessage] = useState<string | null>(null);
    const [error, setError] = useState<string | null>(null);
    const [submitting, setSubmitting] = useState(false);

    async function handleCancel() {
        if (!orderId.trim()) {
            setError("Enter an order ID");
            return;
        }

        setSubmitting(true);
        setMessage(null);
        setError(null);

        try {
            const result = await cancelOrder(orderId.trim());

            setMessage(result.message);
            setOrderId("");

            await onCancelComplete();
        }
        catch (e) {
            setError(
                e instanceof Error
                    ? e.message
                    : "Cancel request failed"
            );
        }
        finally {
            setSubmitting(false);
        }
    }

    return (
        <section className="panel">
            <div className="panel-heading">
                <div>
                    <span className="eyebrow">
                        Orders
                    </span>

                    <h3>Cancel order</h3>
                </div>
            </div>

            <label>
                Order ID

                <input
                    value={orderId}
                    placeholder="ORDER_12"
                    onChange={(e) =>
                        setOrderId(e.target.value)
                    }
                />
            </label>

            <button
                className="cancel-order"
                onClick={handleCancel}
                disabled={submitting}
            >
                {submitting
                    ? "Cancelling..."
                    : "Cancel Order"}
            </button>

            {message && (
                <div className="status ok">
                    {message}
                </div>
            )}

            {error && (
                <div className="status error">
                    {error}
                </div>
            )}
        </section>
    );
}