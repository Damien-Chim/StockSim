import { useEffect, useRef } from "react";
import {
    createChart,
    CandlestickSeries,
    type UTCTimestamp
} from "lightweight-charts";

function CandlestickChart() {
    const chartContainerRef = useRef<HTMLDivElement>(null);

    useEffect(() => {
        if (!chartContainerRef.current) {
            return;
        }

        const chart = createChart(chartContainerRef.current, {
            width: 800,
            height: 500,
        });

        const candleSeries = chart.addSeries(CandlestickSeries);

        candleSeries.setData([
            {
                time: 1786948200 as UTCTimestamp,
                open: 100,
                high: 108,
                low: 97,
                close: 105,
            },
            {
                time: 1786948210 as UTCTimestamp,
                open: 105,
                high: 112,
                low: 101,
                close: 103,
            },
            {
                time: 1786948220 as UTCTimestamp,
                open: 103,
                high: 110,
                low: 102,
                close: 109,
            },
        ]);

        chart.timeScale().fitContent();

        return () => {
            chart.remove();
        };
    }, []);

    return <div ref={chartContainerRef} />;
}

export default CandlestickChart;